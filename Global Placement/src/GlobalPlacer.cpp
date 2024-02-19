#include "GlobalPlacer.h"
#include "ExampleFunction.h"
#include "NumericalOptimizer.h"
#include "lse.h"
#include <cstdlib>
#include <iostream>

GlobalPlacer::GlobalPlacer(wrapper::Placement &placement)
    : _placement(placement)
{
}

void GlobalPlacer::randomPlace()
{
    srand(0);
    double coreWidth = _placement.boundryRight() - _placement.boundryLeft();
    double coreHeight = _placement.boundryTop() - _placement.boundryBottom();
    for (size_t i = 0; i < _placement.numModules(); ++i)
    {
        if (_placement.module(i).isFixed())
            continue;

        double width = _placement.module(i).width();
        double height = _placement.module(i).height();
        double x = rand() % static_cast<int>(coreWidth - width) + _placement.boundryLeft();
        double y = rand() % static_cast<int>(coreHeight - height) + _placement.boundryBottom();
        _placement.module(i).setPosition(x, y);
    }
}
void GlobalPlacer::initPlace(std::vector<double> &x)
{
    srand(0);
    double coreWidth = _placement.boundryRight() - _placement.boundryLeft();
    double coreHeight = _placement.boundryTop() - _placement.boundryBottom();
    for (size_t i = 0; i < _placement.numModules(); ++i)
    {
        if (_placement.module(i).isFixed())
            continue;

        double width = _placement.module(i).width();
        double height = _placement.module(i).height();
        x[i*2] = rand() % static_cast<int>(coreWidth - width) + _placement.boundryLeft();
        x[i*2+1] = rand() % static_cast<int>(coreHeight - height) + _placement.boundryBottom();
        // _placement.module(i).setPosition(x[i*2], x[i*2+1]);
        // _placement.module(i).setCenterPosition(x[i*2]+width/2, x[i*2+1]+height/2);
    }
}

void GlobalPlacer::place()
{
    ///////////////////////////////////////////////////////////////////
    // The following example is only for analytical methods.
    // if you use other methods, you can skip and delete it directly.
    //////////////////////////////////////////////////////////////////

    ExampleFunction ef; // require to define the object function and gradient function
    
    vector<double> x(_placement.numModules()*2); // solution vector, size: num_blocks*2
                         // each 2 variables represent the X and Y dimensions of a block
    vector<double> g_value(_placement.numModules()*2,0);
    double f_value = 0;        
    std::cout << _placement.numModules() << std::endl;
    initPlace(x);  // initialize the solution vector
    std::cout << "bottom: " << _placement.boundryBottom() << " ,top: " << _placement.boundryTop() <<  " , right: " << _placement.boundryRight() << ", left: " << _placement.boundryLeft() << std::endl;
    double width_middle = _placement.boundryTop() - _placement.boundryBottom();
    double height_middle = _placement.boundryRight() - _placement.boundryLeft();
    double min_middle = (width_middle+height_middle)/2;
    std::cout << "alpha: " << min_middle*0.008 << std::endl;
    // LSE ls(_placement,f_value,g_value, min_middle*0.07, 5000000, 6, 6);
    LSE ls(_placement,f_value,g_value,  min_middle*0.08, 9000000, 6, 6);
    // notion version alpha:0.06

    // NumericalOptimizer no(ef);
    NumericalOptimizer no(ls);

    // no.setX(x);             // set initial solution
    // // ls.lseloss(x,1000);
    
    // no.setNumIteration(80); // user-specified parameter
    
    // no.setStepSizeBound((_placement.boundryRight() - _placement.boundryLeft())*5); // user-specified parameter
    // no.solve();             // Conjugate Gradient solver
    // // no.setX(x);
    // for (size_t i = 0; i < _placement.numModules(); ++i)
    // {
    //     if (_placement.module(i).isFixed())
    //         continue;
    //     double cur_x = no.x(2 * i);
    //     double cur_y = no.x(2 * i + 1);
    //     if(cur_x + _placement.module(i).width() > _placement.boundryRight()){
    //         cur_x = _placement.boundryRight() - _placement.module(i).width();
    //     }
    //     else if(cur_x - _placement.module(i).width() < _placement.boundryLeft()){
    //         cur_x = _placement.boundryLeft();
    //     }
    //     else{
    //         //Null
    //     }
        
    //     if(cur_y + _placement.module(i).height() > _placement.boundryTop()){
    //         cur_y = _placement.boundryTop() - _placement.module(i).height();
    //     }
    //     else if(cur_y - _placement.module(i).height() < _placement.boundryBottom()){
    //         cur_y = _placement.boundryBottom();
    //     }
    //     else{
    //         //Null
    //     }
        
    //     _placement.module(i).setPosition(cur_x, cur_y);

    //     x[2 * i] = cur_x; 
    //     x[2 * i + 1] = cur_y; 
    // }
    
    // ls.eta = 1000;
    int round = 5;
    if ((_placement.boundryTop() - _placement.boundryBottom())/100000>1 && _placement.numModules()<=80000){
        ls.bin_num_height = 8;
        ls.bin_num_width = 8;
        round = 4;
    }

    int bin_iternum = 30;

    for(int i = 0; i< round; ++i)
    {
        
        
        // ls.beta = i*9000000;
        if(i){
            // ls.beta = 9000000;
            if ((_placement.boundryTop() - _placement.boundryBottom())/5000<1)
            {
                // ls.beta = 900000;
                ls.beta = 900000;
            }else{
                // ls.beta = 9000000;
                ls.beta = 9000000;
            }
        }else{
            ls.beta = i;
        }
        // ls.eta = min_middle*0.02-i*min_middle*0.001;
        // ls.beta = 500000;
        //public 1 0.008 0, 20, 20, 50000
        no.setX(x);             // set initial solution
        // ls.lseloss(x,1000);
        if(!i){
            no.setNumIteration(80); // user-specified parameter
        }else{
            no.setNumIteration(bin_iternum);
        }
        
        no.setStepSizeBound((_placement.boundryRight() - _placement.boundryLeft())*5); // user-specified parameter
        no.solve();             // Conjugate Gradient solver
        // no.setX(x);
        for (size_t i = 0; i < _placement.numModules(); ++i)
        {
            if (_placement.module(i).isFixed())
                continue;
            auto current_module = _placement.module(i);
            double cur_x = no.x(2 * i);
		    double cur_y = no.x(2 * i + 1);
            //check module
            if(cur_x + current_module.width() > _placement.boundryRight()) cur_x = _placement.boundryRight() - current_module.width();
			else if(cur_x - current_module.width() < _placement.boundryLeft()) cur_x = _placement.boundryLeft();
			if(cur_y + current_module.height() > _placement.boundryTop()) cur_y = _placement.boundryTop() - current_module.height();
			else if(cur_y - current_module.height() < _placement.boundryBottom()) cur_y = _placement.boundryBottom();
			
			current_module.setPosition(cur_x, cur_y);

			x[2 * i] = cur_x; 
			x[2 * i + 1] = cur_y; 
            
        }
    }
    

    // cout << "Current solution:\n";
    // for (unsigned i = 0; i < no.dimension(); i++)
    // {
    //     cout << "x[" << i << "] = " << no.x(i) << "\n";
    // }
    // cout << "Objective: " << no.objective() << "\n";
    ////////////////////////////////////////////////////////////////

    // An example of random placement implemented by TA.
    // If you want to use it, please uncomment the folllwing 1 line.
    // randomPlace();

    /* @@@ TODO
     * 1. Understand above example and modify ExampleFunction.cpp to implement the analytical placement
     * 2. You can choose LSE or WA as the wirelength model, the former is easier to calculate the gradient
     * 3. For the bin density model, you could refer to the lecture notes
     * 4. You should first calculate the form of wirelength model and bin density model and the forms of their gradients ON YOUR OWN
     * 5. Replace the value of f in evaluateF() by the form like "f = alpha*WL() + beta*BinDensity()"
     * 6. Replace the form of g[] in evaluateG() by the form like "g = grad(WL()) + grad(BinDensity())"
     * 7. Set the initial vector x in place(), set step size, set #iteration, and call the solver like above example
     * */
}
