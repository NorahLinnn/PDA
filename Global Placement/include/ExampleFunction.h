#ifndef EXAMPLEFUNCTION_H
#define EXAMPLEFUNCTION_H

#include "NumericalOptimizerInterface.h"
#include "Wrapper.hpp"

class ExampleFunction : public NumericalOptimizerInterface
{
public:
    ExampleFunction();

    void evaluateFG(const vector<double> &x, double &f, vector<double> &g);
    void evaluateF(const vector<double> &x, double &f);
    
    unsigned dimension();
private:
};
// class LSE
// {
// public:
//     LSE(wrapper::Placement &placement);

//     // void lseloss(const vector<double> &x, double &f, vector<double> &g, double alpha);
//     void lseloss(const vector<double> &x, double alpha);
//     vector<double> create_exp_vector(const vector<double> &x, vector<double> &exp_vector, double alpha);
//     double WL(const vector<double> &x, double alpha);
// private:
//     wrapper::Placement &_placement;
// };


#endif // EXAMPLEFUNCTION_H
