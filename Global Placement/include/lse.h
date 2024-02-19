#ifndef lse_H
#define lse_H

#include "NumericalOptimizerInterface.h"
#include "Wrapper.hpp"

class LSE: public NumericalOptimizerInterface
{
public:
    LSE(wrapper::Placement &placement, double &f_value, vector<double> &g_value, double eta, double beta, int bin_num_width, int bin_num_height);

    // void lseloss(const vector<double> &x, double &f, vector<double> &g, double eta);
    void lseloss(const vector<double> &x, double eta, double beta);
    void create_exp_vector(const vector<double> &x, vector<double> &exp_vector, double eta);
    void create_cal_vector(const vector<double> &x, vector<double> &exp_vector,vector<double> &cal_vector, double eta, vector<vector<double>> &exp_sum);
    
    double WL(const vector<double> &x, double eta, vector<double> &exp_vector, vector<vector<double>> &exp_sum);
    // vector<double> WL_cal(const vector<double> &x, double eta, double &wo, vector<double> &exp_vector,vector<double> &cal_vector, vector<double> &wl_cal_result, vector<vector<double>> &exp_sum);
    void WL_cal(const vector<double> &x, double eta, double &wo, vector<double> &exp_vector,vector<double> &cal_vector, vector<double> &wl_cal_result, vector<vector<double>> &exp_sum);
    
    // std::pair<double, vector<double>> density_constraint(const vector<double> &x, double beta);
    double density_constraint(const vector<double> &x, double beta, vector<double> &density_cal_result);
    double condtionA(double &alpha, double &d_value);
    double condtionB(double &beta, double &d_value, double &bin_edge, double &module_edge);
    double condtionC();
    double condtionA_cal(double &alpha, double x_i, double x_b);
    double condtionB_cal(double &beta, double &d_value, double &bin_edge, double &module_edge, double x_i, double x_b);
    double condtionC_cal();

    void evaluateFG(const vector<double> &x, double &f, vector<double> &g);
    void evaluateF(const vector<double> &x, double &f);
    
    
    unsigned dimension();
// private:
    
    double eta;
    double beta;
    int bin_num_height;
    int bin_num_width;
    double target_density;
    vector<double> c_vector;
private:
    wrapper::Placement &_placement;
    double f_value;
    vector<double> g_value; 
};


#endif