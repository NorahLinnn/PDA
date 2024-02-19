#include "ExampleFunction.h"
#include <cmath>
#include <iostream>
#include <cstdlib>
// minimize 3*x^2 + 2*x*y + 2*y^2 + 7

ExampleFunction::ExampleFunction()
{
    
}

void ExampleFunction::evaluateFG(const vector<double> &x, double &f, vector<double> &g)
{
    f = 3 * x[0] * x[0] + 2 * x[0] * x[1] + 2 * x[1] * x[1] + 7; // objective function
    g[0] = 6 * x[0] + 2 * x[1];                                  // gradient function of X
    g[1] = 2 * x[0] + 4 * x[1];                                  // gradient function of Y
}

void ExampleFunction::evaluateF(const vector<double> &x, double &f)
{
    f = 3 * x[0] * x[0] + 2 * x[0] * x[1] + 2 * x[1] * x[1] + 7; // objective function
}

unsigned ExampleFunction::dimension()
{
    return 2; // num_blocks*2
    // each two dimension represent the X and Y dimensions of each block
}
// LSE::LSE(wrapper::Placement &placement)
//     : _placement(placement)
// {
// }
// vector<double> LSE::create_exp_vector(const vector<double> &x, vector<double> &exp_vector, double alpha)
// {
//     std::cout << exp_vector.size() << std::endl;
//     std::cout << x.size() << std::endl;
//     for (size_t i = 0; i < _placement.numModules(); ++i)
//     {
        
//         // std::cout << "i: " << i << std::endl;
//         if (_placement.module(i).isFixed())
//         {
//             auto m = _placement.module(i);
//             // std::cout << "i: " << i*4 << " " << i*4+1 << " " << i*4+2 << " " << i*4+3 << std::endl;
//             exp_vector[i*4] = exp(m.centerX()/alpha);
//             exp_vector[i*4+1] = exp(-m.centerX()/alpha);
//             exp_vector[i*4+2] = exp(m.centerY()/alpha);
//             exp_vector[i*4+3] = exp(-m.centerY()/alpha);
//         }
//         else
//         {
//             // std::cout << "i: " << i << std::endl;
//             // std::cout << "i: " << i*4 << " " << i*4+1 << " " << i*4+2 << " " << i*4+3 << std::endl;
//             // std::cout << "i: " << i*2 << " " << i*2+1 << std::endl;
//             exp_vector[i*4] = exp(x[i*2]/alpha);
//             exp_vector[i*4+1] = exp(-x[i*2]/alpha);
//             exp_vector[i*4+2] = exp(x[i*2+1]/alpha);
//             exp_vector[i*4+3] = exp(-x[i*2+1]/alpha);
            
//         }
//     }
//     std::cout << "finish" << std::endl;
// }
// double LSE::WL(const vector<double> &x, double alpha)
// {
//     vector<double> exp_vector(_placement.numModules()*4);
//     double lseloss_1=0,lseloss_2=0,lseloss_3=0,lseloss_4=0;
//     create_exp_vector(x,exp_vector, alpha);
//     std::cout << "finish in wl" << std::endl;
//     for(size_t i=0; i< _placement.numNets(); ++i)
//     {
//         // std::cout << "i: " << std::endl;
//         auto current_net = _placement.net(i);
//         for(size_t j = 0; j<current_net.numPins();++j)
//         {
//             auto current_pin = current_net.pin(j);
//             lseloss_1 += exp_vector[current_pin.moduleId()*4];
//             lseloss_2 += exp_vector[current_pin.moduleId()*4+1];
//             lseloss_3 += exp_vector[current_pin.moduleId()*4+2];
//             lseloss_4 += exp_vector[current_pin.moduleId()*4+3];
//         }
//     }
//     return log(lseloss_1)+log(lseloss_2)+log(lseloss_3)+log(lseloss_4);
// }
// // void LSE::lseloss(const vector<double> &x, double &f, vector<double> &g, double alpha)
// void LSE::lseloss(const vector<double> &x, double alpha)
// {
//     std::cout <<  WL(x,alpha) << std::endl;
// }
