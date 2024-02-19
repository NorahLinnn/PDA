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
    // for(size_t i = 0; i<_placement.numModules(); ++i)
    // {
    //     auto m = _placement.module(i);
    //     target_density += m.area();
    //     c_vector[i] = m.area()/(bin_width*bin_height);
    // }
    // target_density = target_density/(coreWidth*coreHeight);
}
void LSE::create_exp_vector(const vector<double> &x, vector<double> &exp_vector, double eta)
{
    // std::cout << exp_vector.size() << std::endl;
    // std::cout << x.size() << std::endl;
    for (size_t i = 0; i < _placement.numModules(); ++i)
    {
        if (_placement.module(i).isFixed())
        {
            auto m = _placement.module(i);
            // std::cout << "i: " << i*4 << " " << i*4+1 << " " << i*4+2 << " " << i*4+3 << std::endl;
            exp_vector[i*4] = exp(m.centerX()/eta);
            exp_vector[i*4+1] = exp(-m.centerX()/eta);
            exp_vector[i*4+2] = exp(m.centerY()/eta);
            exp_vector[i*4+3] = exp(-m.centerY()/eta);
        }
        else
        {
            // std::cout << "i: " << i*4 << " " << i*4+1 << " " << i*4+2 << " " << i*4+3 << std::endl;
            // std::cout << "i: " << i*2 << " " << i*2+1 << std::endl;
            // if(i*4+3>exp_vector.size()) std::cout<<"i*4: " <<i << std::endl;
            // if(i*2+1>x.size()) std::cout<<"i*2: " <<i << std::endl;
            auto m = _placement.module(i);
            exp_vector[i*4] = exp(((x[i*2])+m.width()/2)/eta);
            exp_vector[i*4+1] = exp(-((x[i*2])+m.width()/2)/eta);
            exp_vector[i*4+2] = exp(((x[i*2+1])+m.height()/2)/eta);
            exp_vector[i*4+3] = exp(-((x[i*2+1])+m.height()/2)/eta);  
        }
        // std::cout << "i: " << exp_vector[i*4] << " " << exp_vector[i*4+1] << " " << exp_vector[i*4+2] << " " << exp_vector[i*4+3] << std::endl;

    }
    // std::cout << "finish" << std::endl;
}
double LSE::WL(const vector<double> &x, double eta, vector<double> &exp_vector, vector<vector<double>> &exp_sum)
{
    
    create_exp_vector(x,exp_vector, eta);
    double return_value=0;
    for(size_t i=0; i< _placement.numNets(); ++i)
    {
        // std::cout << "i: " << std::endl;
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
        // std::cout << return_value << std::endl;   
    }
    return return_value*eta;
}
// vector<double> LSE::WL_cal(const vector<double> &x, double eta, double &wl, vector<double> &exp_vector,vector<double> &cal_vector, vector<vector<double>> &exp_sum)
void LSE::WL_cal(const vector<double> &x, double eta, double &wl, vector<double> &exp_vector,vector<double> &cal_vector, vector<double> &wl_cal_result, vector<vector<double>> &exp_sum)
{
    // vector<double> wl_cal_result(_placement.numModules()*2,0);
    // create_cal_vector(x, exp_vector, cal_vector,eta, exp_sum);
    for(size_t i=0; i< _placement.numNets(); ++i)
    {
        // std::cout << "i: " << std::endl;
        auto current_net = _placement.net(i);
        for(size_t j = 0; j<current_net.numPins();++j)
        {
            auto current_pin = current_net.pin(j);
            // wl_cal_result[current_pin.moduleId()*2] += (1/lseloss_1)*(1/eta)*exp_vector[current_pin.moduleId()*4];
            // wl_cal_result[current_pin.moduleId()*2] -= (1/lseloss_2)*(1/eta)*exp_vector[current_pin.moduleId()*4+1];
            // wl_cal_result[current_pin.moduleId()*2+1] += (1/lseloss_3)*(1/eta)*exp_vector[current_pin.moduleId()*4+2];
            // wl_cal_result[current_pin.moduleId()*2+1] -= (1/lseloss_4)*(1/eta)*exp_vector[current_pin.moduleId()*4+3];
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
    // vector<double> theta_vector(_placement.numModules()*2, 0);
    vector<vector<vector<double>>> theta_vector(_placement.numModules()*2, vector<vector<double>>(bin_num_width,vector<double>(bin_num_height,0)));
    vector<vector<vector<double>>> theta_cal_vector(_placement.numModules()*2, vector<vector<double>>(bin_num_width,vector<double>(bin_num_height,0)));
    // vector<double> theta_cal_vector(_placement.numModules()*2,0);
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
    // vector<double> density_cal_result(_placement.numModules()*2, 0);
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
                    // theta_vector[i*2][a][b] = 1-alphaX*pow(d_X,2);
                    
                    // theta_cal_vector[i*2][a][b] = condtionA_cal(alphaX,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2);
                }else if(d_X >= (bin_width/2+w_i/2) && d_X <= (bin_width+w_i/2))
                {
                    theta_vector[i*2][a][b] = condtionB(betaX,d_X,bin_width,w_i);
                    // theta_vector[i*2][a][b] = betaX*pow((d_X-bin_width-w_i/2),2);
                    
                    // theta_cal_vector[i*2][a][b] = condtionB_cal(betaX,d_X, bin_width,w_i,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2);
                }else if((bin_width+w_i/2) <= d_X)
                {
                    // theta_vector[i*2][a][b] = condtionC();
                    theta_vector[i*2][a][b] = 0;
                    // theta_cal_vector[i*2][a][b] = condtionC_cal();
                }

                if(d_Y >= 0 && d_Y <= (bin_height/2+h_i/2))
                {
                    theta_vector[i*2+1][a][b] = condtionA(alphaY, d_Y);
                    // theta_vector[i*2+1][a][b] = 1-alphaY*pow(d_Y,2);
                    
                    // theta_cal_vector[i*2+1][a][b] = condtionA_cal(alphaY, x[i*2+1]+h_i/2,(boundryBottom+b*bin_height)+bin_height/2);
                }else if(d_Y >= (bin_height/2+h_i/2) && d_Y <= (bin_height+h_i/2))
                {
                    theta_vector[i*2+1][a][b] = condtionB(betaY, d_Y, bin_height, h_i);
                    // theta_vector[i*2+1][a][b] = betaY*pow((d_Y-bin_height-h_i/2),2);
                    
                    // theta_cal_vector[i*2+1][a][b] = condtionB_cal(betaY, d_Y, bin_height, h_i,x[i*2+1]+h_i/2,(boundryBottom+b*bin_height)+bin_height/2);
                }else if((bin_height+h_i/2) <= d_Y)
                {
                    // theta_vector[i*2+1][a][b] = condtionC();
                    theta_vector[i*2+1][a][b] = 0;
                    // theta_cal_vector[i*2+1][a][b] = condtionC_cal();
                }
                //grad
                if(d_X >= 0 && d_X <= (bin_width/2+w_i/2))
                {
                    // theta_vector[i*2][a][b] = condtionA(alphaX, d_X);
                    // theta_cal_vector[i*2][a][b] = condtionA_cal(alphaX,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2)*theta_vector[i*2+1][a][b];
                    
                    // theta_cal_vector[i*2][a][b] = condtionA_cal(alphaX,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2)*theta_vector[i*2+1][a][b];
                    // if((x[i*2]+w_i/2)-(boundryLeft+a*bin_width+bin_width/2) >= 0 )
                    // {
                    //     theta_cal_vector[i*2][a][b] = -2*alphaX*(d_X)*theta_vector[i*2+1][a][b];
                    // }else{
                    //     theta_cal_vector[i*2][a][b] = -2*alphaX*(d_X)*(-1)*theta_vector[i*2+1][a][b];
                    // }
                    theta_cal_vector[i*2][a][b] = condtionA_cal(alphaX,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2)*theta_vector[i*2+1][a][b];

                    // -2*alphaX*(d_X)*(d_X/(x_i-x_b));
                }else if(d_X >= (bin_width/2+w_i/2) && d_X <= (bin_width+w_i/2))
                {
                    // theta_vector[i*2][a][b] = condtionB(betaX,d_X,bin_width,w_i);
                    // theta_cal_vector[i*2][a][b] = condtionB_cal(betaX,d_X, bin_width,w_i,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2)*theta_vector[i*2+1][a][b];
                    // theta_cal_vector[i*2][a][b] = condtionB_cal(betaX,d_X, bin_width,w_i,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2)*theta_vector[i*2+1][a][b];
                    theta_cal_vector[i*2][a][b] = condtionB_cal(betaX,d_X, bin_width,w_i,x[i*2]+w_i/2,(boundryLeft+a*bin_width)+bin_width/2)*theta_vector[i*2+1][a][b];
                    // betaX*(2*d_X-2*bin_width-w_i)*(abs(x_i-x_b)/(x_i-x_b));
                    // if((x[i*2]+w_i/2)-(boundryLeft+a*bin_width+bin_width/2) >= 0 )
                    // {
                    //     theta_cal_vector[i*2][a][b] = betaX*(2*d_X-2*bin_width-w_i)*theta_vector[i*2+1][a][b];
                    // }else{
                    //     theta_cal_vector[i*2][a][b] = betaX*(2*d_X-2*bin_width-w_i)*(-1)*theta_vector[i*2+1][a][b];
                    // }
                }else if((bin_width+w_i/2) <= d_X)
                {
                    // theta_vector[i*2][a][b] = condtionC();
                    // theta_cal_vector[i*2][a][b] = condtionC_cal();
                    theta_cal_vector[i*2][a][b] = 0;
                }

                if(d_Y >= 0 && d_Y <= (bin_height/2+h_i/2))
                {
                    // theta_vector[i*2+1][a][b] = condtionA(alphaY, d_Y);
                    // theta_cal_vector[i*2+1][a][b] = condtionA_cal(alphaY, x[i*2+1]+h_i/2,(boundryBottom+b*bin_height)+bin_height/2)*theta_vector[i*2][a][b];
                    theta_cal_vector[i*2+1][a][b] = condtionA_cal(alphaY, x[i*2+1]+h_i/2,(boundryBottom+b*bin_height)+bin_height/2)*theta_vector[i*2][a][b];
                    // if((x[i*2+1]+h_i/2)-(boundryBottom+b*bin_height+bin_height/2)>=0)
                    // {
                    //     theta_cal_vector[i*2+1][a][b] = -2*alphaY*(d_Y)*theta_vector[i*2][a][b];
                    // }else{
                    //     theta_cal_vector[i*2+1][a][b] = -2*alphaY*(d_Y)*(-1)*theta_vector[i*2][a][b];
                    // }

                }else if(d_Y >= (bin_height/2+h_i/2) && d_Y <= (bin_height+h_i/2))
                {
                    // theta_vector[i*2+1][a][b] = condtionB(betaY, d_Y, bin_height, h_i);
                    // theta_cal_vector[i*2+1][a][b] = condtionB_cal(betaY, d_Y, bin_height, h_i,x[i*2+1]+h_i/2,(boundryBottom+b*bin_height)+bin_height/2)*theta_vector[i*2][a][b];
                    theta_cal_vector[i*2+1][a][b] = condtionB_cal(betaY, d_Y, bin_height, h_i,x[i*2+1]+h_i/2,(boundryBottom+b*bin_height)+bin_height/2)*theta_vector[i*2][a][b];
                    // if((x[i*2+1]+h_i/2)-(boundryBottom+b*bin_height+bin_height/2) >= 0)
                    // {
                    //     theta_cal_vector[i*2+1][a][b] = beta*(2*(d_Y)-2*bin_height-h_i)*theta_vector[i*2][a][b];
                    // }else{
                    //     theta_cal_vector[i*2+1][a][b] = beta*(2*(d_Y)-2*bin_height-h_i)*(-1)*theta_vector[i*2][a][b];
                    // }
                    
                }else if((bin_height+h_i/2) <= d_Y)
                {
                    // theta_vector[i*2+1][a][b] = condtionC();
                    // theta_cal_vector[i*2+1][a][b] = condtionC_cal();
                    theta_cal_vector[i*2+1][a][b] = 0;
                }
                // std::cout << "x: " << theta_vector[i*2][a][b] << "y: " << theta_vector[i*2+1][a][b] << std::endl;
                // std::cout << "y: " << theta_vector[i*2+1][a][b] << std::endl;
                // theta_sum[i] += (theta_vector[i*2][a][b]*theta_vector[i*2+1][a][b]);
                c_vector[i] = current_module.area()/(bin_width*bin_height);
                // target_density += m.area();
                
                density_result[bin_num_width*a+b] += c_vector[i]*theta_vector[i*2][a][b]*theta_vector[i*2+1][a][b];
            }
            // target_density = target_density/(coreWidth*coreHeight);
            return_value += pow(density_result[bin_num_width*a+b]-target_density,2);
            // for (size_t i = 0; i < _placement.numModules(); ++i)
            // {
            //     if(_placement.module(i).isFixed())
            //         continue;
            //     auto m = _placement.module(i);
                
            //     density_cal_result[i*2] += 2*c_vector[i]*theta_cal_vector[i*2][a][b]*(density_result[bin_num_width*a+b]-target_density)*beta;
            //     density_cal_result[i*2+1] += 2*c_vector[i]*theta_cal_vector[i*2+1][a][b]*(density_result[bin_num_width*a+b]-target_density)*beta;
            // }
        }
        // c_vector[i] = m.area()/(bin_width*bin_height);
        // c_vector[i] = 1;
        // std::cout << "theta sum: " << theta_sum << std::endl;
        // std::cout << c_vector[i] << std::endl;
        
    }
    for (size_t a = 0; a < bin_num_width; ++a)
    {
        for (size_t b = 0; b < bin_num_height; ++b)
        {
            for (size_t i = 0; i < _placement.numModules(); ++i)
            {
                if(_placement.module(i).isFixed())
                    continue;
                // auto current_module = _placement.module(i);
                
                density_cal_result[i*2] += 2*c_vector[i]*theta_cal_vector[i*2][a][b]*(density_result[bin_num_width*a+b]-target_density)*beta;
                density_cal_result[i*2+1] += 2*c_vector[i]*theta_cal_vector[i*2+1][a][b]*(density_result[bin_num_width*a+b]-target_density)*beta;
            }
            // return_value += pow(density_result[bin_num_width*a+b]-target_density,2);
        }
    }
    // for(auto v:density_cal_result)
    // {
    //     std::cout << v << std::endl;
    // }
    // return std::make_pair(return_value*beta, density_cal_result);
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
        // f_value = dc.first+wl;
        // std::cout<<dc/beta << std::endl;
        f_value = dc+wl;
        for(auto& v: g_value)
        {
            // g_value[idx] = wl_cal[idx]+dc.second[idx];
            g_value[idx] = wl_cal_result[idx]+density_cal_result[idx];
            
            // std::cout << "wl_cal: " << wl_cal[idx] << " ,dc_cal: " << dc.second[idx] << std::endl;
            idx += 1;
        }
        // f_value = dc;
        // g_value = density_cal_result;
    }else{
        f_value = wl;
        g_value = wl_cal_result;
    }
    
    // std::cout << wl <<  std::endl;
    // std::cout << "dc: " << dc.first << std::endl;
    // f_value = wl;
    // g_value = wl_cal;
    
    // dc = density_constraint(x,beta);
    
    // f_value = dc.first;
    // g_value = dc.second;
    // g_value = dc.second+wl_cal;
    // g_value.reserve(wl_cal.size());

    // std::transform(wl_cal.begin(), wl_cal.end(), dc.second.begin(), std::back_inserter(g_value), std::plus<double>());

}
void LSE::evaluateFG(const vector<double> &x, double &f, vector<double> &g)
{
    // f = 3 * x[0] * x[0] + 2 * x[0] * x[1] + 2 * x[1] * x[1] + 7; // objective function
    // g[0] = 6 * x[0] + 2 * x[1];                                  // gradient function of X
    // g[1] = 2 * x[0] + 4 * x[1];                                  // gradient function of Y
    lseloss(x, eta,beta);
    // std::cout << f_value <<  " " <<  g_value[0] <<  std::endl;
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