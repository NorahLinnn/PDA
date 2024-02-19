#include <cmath>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include "lse.h"

LSE::LSE(wrapper::Placement &placement,double &f_value, vector<double> &g_value, double eta, double beta, int bin_num_width, int bin_num_height)
    : _placement(placement), f_value(f_value), g_value(g_value), eta(eta), beta(beta), bin_num_height(bin_num_height), bin_num_width(bin_num_width)
{
    double coreWidth = _placement.boundryRight() - _placement.boundryLeft();
    double coreHeight = _placement.boundryTop() - _placement.boundryBottom();
    double bin_width = coreWidth/bin_num_width;
    double bin_height = coreHeight/bin_num_height;
    vector<double> c_vector(_placement.numModules(), 0);
    
}
void LSE::create_exp_vector(const vector<double> &x, vector<double> &exp_vector, double eta)
{
    
    for (size_t i = 0; i < _placement.numModules(); ++i)
    {
        if (_placement.module(i).isFixed())
        {
            auto m = _placement.module(i);
            exp_vector[i*4] = exp(m.centerX()/eta);
            exp_vector[i*4+1] = exp(-m.centerX()/eta);
            exp_vector[i*4+2] = exp(m.centerY()/eta);
            exp_vector[i*4+3] = exp(-m.centerY()/eta);
        }
        else
        {
            
            auto m = _placement.module(i);
            exp_vector[i*4] = exp(((x[i*2])+m.width()/2)/eta);
            exp_vector[i*4+1] = exp(-((x[i*2])+m.width()/2)/eta);
            exp_vector[i*4+2] = exp(((x[i*2+1])+m.height()/2)/eta);
            exp_vector[i*4+3] = exp(-((x[i*2+1])+m.height()/2)/eta);  
        }

    }
    // std::cout << "finish" << std::endl;
}
double LSE::WL(const vector<double> &x, double eta, vector<double> &exp_vector, vector<vector<double>> &exp_sum)
{
    
    create_exp_vector(x,exp_vector, eta);
    double return_value=0;
    for(size_t i=0; i< _placement.numNets(); ++i)
    {
        auto current_net = _placement.net(i);
        for(size_t j = 0; j<current_net.numPins();++j)
        {
            auto current_pin = current_net.pin(j);
            exp_sum[i][0] += exp_vector[current_pin.moduleId()*4];
            exp_sum[i][1] += exp_vector[current_pin.moduleId()*4+1];
            exp_sum[i][2] += exp_vector[current_pin.moduleId()*4+2];
            exp_sum[i][3] += exp_vector[current_pin.moduleId()*4+3];
        }
        return_value += (log(exp_sum[i][0])+log(exp_sum[i][1])+log(exp_sum[i][2])+log(exp_sum[i][3]));   
    }
    return return_value*eta;
}
void LSE::WL_cal(const vector<double> &x, double eta, double &wl, vector<double> &exp_vector,vector<double> &cal_vector, vector<double> &wl_cal_result, vector<vector<double>> &exp_sum)
{
    
    for(size_t i=0; i< _placement.numNets(); ++i)
    {
        auto current_net = _placement.net(i);
        for(size_t j = 0; j<current_net.numPins();++j)
        {
            auto current_pin = current_net.pin(j);
            
            wl_cal_result[current_pin.moduleId()*2] += exp_vector[current_pin.moduleId()*4]/exp_sum[i][0];
            wl_cal_result[current_pin.moduleId()*2] -= exp_vector[current_pin.moduleId()*4+1]/exp_sum[i][1];
            wl_cal_result[current_pin.moduleId()*2+1] += exp_vector[current_pin.moduleId()*4+2]/exp_sum[i][2];
            wl_cal_result[current_pin.moduleId()*2+1] -= exp_vector[current_pin.moduleId()*4+3]/exp_sum[i][3];
        }
    }
    // return wl_cal_result;
}
// void LSE::lseloss(const vector<double> &x, double &f, vector<double> &g, double eta)
double LSE::condtionA(double& alpha, double& d_value)
{
    return 1-alpha*pow(d_value,2);
}
double LSE::condtionB(double& beta, double& d_value, double& bin_edge, double& module_edge)
{
    return beta*pow((d_value-bin_edge-module_edge/2),2);
}
double LSE::condtionC()
{
    return 0;
}
double LSE::condtionA_cal(double &alpha, double x_i, double x_b)
{
    // return -2*alpha*(abs(x_i-x_b))*(abs(x_i-x_b)/(x_i-x_b));
    if((x_i-x_b)>=0)
    {
        return -2*alpha*(abs(x_i-x_b));
    }else{
        return -2*alpha*(abs(x_i-x_b))*(-1);
    }
    // return -2*alpha*(abs(x_i-x_b))*(abs(x_i-x_b)/(x_i-x_b));
}
double LSE::condtionB_cal(double &beta, double &d_value, double &bin_edge, double &module_edge, double x_i, double x_b)
{
    // return beta*(2*(abs(x_i-x_b))-2*bin_edge-module_edge)*(abs(x_i-x_b)/(x_i-x_b));
    if((x_i-x_b)>=0)
    {
        return beta*(2*(abs(x_i-x_b))-2*bin_edge-module_edge);
    }else{
        return beta*(2*(abs(x_i-x_b))-2*bin_edge-module_edge)*(-1);
    }
}
double LSE::condtionC_cal()
{
    return 0;
}
double LSE::density_constraint(const vector<double> &x, double beta, vector<double> &density_cal_result)
{
    double return_value = 0;
    double return_cal_value = 0;
    vector<vector<vector<double>>> theta_vector(_placement.numModules()*2, vector<vector<double>>(bin_num_width,vector<double>(bin_num_height,0)));
    vector<vector<vector<double>>> theta_cal_vector(_placement.numModules()*2, vector<vector<double>>(bin_num_width,vector<double>(bin_num_height,0)));
    vector<double> c_vector(_placement.numModules(), 0);
    
    double coreWidth = _placement.boundryRight() - _placement.boundryLeft();
    double coreHeight = _placement.boundryTop() - _placement.boundryBottom();
    auto boundryLeft = _placement.boundryLeft();
    auto boundryBottom = _placement.boundryBottom();
    double bin_width = coreWidth/bin_num_width;
    double bin_height = coreHeight/bin_num_height;
    double target_density = 0;
    vector<double> theta_sum(_placement.numModules(), 0);
    vector<double> density_result(bin_num_height*bin_num_width, 0);
    for (size_t i = 0; i < _placement.numModules(); ++i)
    {
        auto current_module = _placement.module(i);
        target_density += current_module.area();
    }
    target_density = target_density/(coreWidth*coreHeight);


    
    for (size_t a = 0; a < bin_num_width; ++a)
    {
        for (size_t b = 0; b < bin_num_height; ++b)
        {
            for (size_t i = 0; i < _placement.numModules(); ++i)
            {
                
                
                if(_placement.module(i).isFixed())
                {
                    // target_density += m.area();
                    continue;
                }
                auto current_module = _placement.module(i);
                double w_i = current_module.width();
                double h_i = current_module.height();
                double d_X = abs((x[i*2]+w_i/2)-(boundryLeft+a*bin_width+bin_width/2));
                double noabs_d_x = (x[i*2]+w_i/2)-(boundryLeft+a*bin_width+bin_width/2);
                double d_Y = abs((x[i*2+1]+h_i/2)-(boundryBottom+b*bin_height+bin_height/2));
                double noabs_d_y = (x[i*2+1]+h_i/2)-(boundryBottom+b*bin_height+bin_height/2);
                double alphaX = 4/((bin_width+w_i)*(2*bin_width+w_i));
                double alphaY = 4/((bin_height+h_i)*(2*bin_height+h_i));
                double betaX = 4/(bin_width*(2*bin_width+w_i));
                double betaY = 4/(bin_height*(2*bin_height+h_i));
                if(d_X >= 0 && d_X <= (bin_width/2+w_i/2))
                {
                    theta_vector[i*2][a][b] = condtionA(alphaX, d_X);
                }else if(d_X >= (bin_width/2+w_i/2) && d_X <= (bin_width+w_i/2))
                {
                    theta_vector[i*2][a][b] = condtionB(betaX,d_X,bin_width,w_i);
                }else if((bin_width+w_i/2) <= d_X)
                {
                    theta_vector[i*2][a][b] = 0;
                }

                if(d_Y >= 0 && d_Y <= (bin_height/2+h_i/2))
                {
                    theta_vector[i*2+1][a][b] = condtionA(alphaY, d_Y);
                    
                }else if(d_Y >= (bin_height/2+h_i/2) && d_Y <= (bin_height+h_i/2))
                {
                    theta_vector[i*2+1][a][b] = condtionB(betaY, d_Y, bin_height, h_i);
                }else if((bin_height+h_i/2) <= d_Y)
                {
                    theta_vector[i*2+1][a][b] = 0;
                }
                //grad
                if(d_X >= 0 && d_X <= (bin_width/2+w_i/2))
                {
                    
                    theta_cal_vector[i*2][a][b] = condtionA_cal(alphaX,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2)*theta_vector[i*2+1][a][b];

                    
                }else if(d_X >= (bin_width/2+w_i/2) && d_X <= (bin_width+w_i/2))
                {
                    
                    theta_cal_vector[i*2][a][b] = condtionB_cal(betaX,d_X, bin_width,w_i,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2)*theta_vector[i*2+1][a][b];
                    
                }else if((bin_width+w_i/2) <= d_X)
                {
                    
                    theta_cal_vector[i*2][a][b] = 0;
                }

                if(d_Y >= 0 && d_Y <= (bin_height/2+h_i/2))
                {
                    
                    theta_cal_vector[i*2+1][a][b] = condtionA_cal(alphaY, x[i*2+1]+h_i/2,(boundryBottom+b*bin_height)+bin_height/2)*theta_vector[i*2][a][b];
                    

                }else if(d_Y >= (bin_height/2+h_i/2) && d_Y <= (bin_height+h_i/2))
                {
                    
                    theta_cal_vector[i*2+1][a][b] = condtionB_cal(betaY, d_Y, bin_height, h_i,x[i*2+1]+h_i/2,(boundryBottom+b*bin_height)+bin_height/2)*theta_vector[i*2][a][b];
                    
                    
                }else if((bin_height+h_i/2) <= d_Y)
                {
                    theta_cal_vector[i*2+1][a][b] = 0;
                }
                c_vector[i] = current_module.area()/(bin_width*bin_height);
                
                density_result[bin_num_width*a+b] += c_vector[i]*theta_vector[i*2][a][b]*theta_vector[i*2+1][a][b];
            }
            return_value += pow(density_result[bin_num_width*a+b]-target_density,2);
            
        }
        
        
    }
    for (size_t a = 0; a < bin_num_width; ++a)
    {
        for (size_t b = 0; b < bin_num_height; ++b)
        {
            for (size_t i = 0; i < _placement.numModules(); ++i)
            {
                if(_placement.module(i).isFixed())
                    continue;
                
                density_cal_result[i*2] += 2*c_vector[i]*theta_cal_vector[i*2][a][b]*(density_result[bin_num_width*a+b]-target_density)*beta;
                density_cal_result[i*2+1] += 2*c_vector[i]*theta_cal_vector[i*2+1][a][b]*(density_result[bin_num_width*a+b]-target_density)*beta;
            }
            
        }
    }
    
    return return_value*beta;
    
}
void LSE::lseloss(const vector<double> &x, double eta, double beta)
{
    
    vector<double> exp_vector(_placement.numModules()*4);
    vector<vector<double>> exp_sum(_placement.numNets(),vector<double>(4));
    auto wl = WL(x,eta,exp_vector, exp_sum);
    vector<double> cal_vector(_placement.numModules()*4);
    vector<double> wl_cal_result(_placement.numModules()*2,0);
    WL_cal(x,eta,wl, exp_vector,cal_vector,wl_cal_result, exp_sum);
    
    double dc;
    vector<double> density_cal_result(_placement.numModules()*2, 0);
    if(beta!=0)
    {
        dc = density_constraint(x,beta, density_cal_result);
        int idx = 0;
        
        f_value = dc+wl;
        for(auto& v: g_value)
        {
            g_value[idx] = wl_cal_result[idx]+density_cal_result[idx];
            idx += 1;
        }
    }else{
        f_value = wl;
        g_value = wl_cal_result;
    }
    
    

}
void LSE::evaluateFG(const vector<double> &x, double &f, vector<double> &g)
{
   
    lseloss(x, eta,beta);
    f = f_value;
    g = g_value;
}

void LSE::evaluateF(const vector<double> &x, double &f)
{   
    lseloss(x, eta,beta);
    f = f_value; // objective function
}

unsigned LSE::dimension()
{
    return _placement.numModules()*2; // num_blocks*2
    // each two dimension represent the X and Y dimensions of each block
}
