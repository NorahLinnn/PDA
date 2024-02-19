#ifndef GLOBALPLACER_H
#define GLOBALPLACER_H

#include "Wrapper.hpp"
#include <cstdlib>
#include <vector>

class GlobalPlacer
{
public:
    GlobalPlacer(wrapper::Placement &placement);

    void randomPlace(); // An example of random placement implemented by TA
    void initPlace(std::vector<double> &solution_vector);
    void place();

private:
    wrapper::Placement &_placement;
};

#endif // GLOBALPLACER_H
