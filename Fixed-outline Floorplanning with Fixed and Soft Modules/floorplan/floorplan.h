#ifndef PARTITION_H
#define PARTITION_H

#include "../Parser/Parser.h"
class Floorplan{
public:
    void createInitial(ParsedData &parsedData);
    void countWirelength(ParsedData &parsedData);
    bool perturbation(ParsedData &parsedData);
    bool checkoverlap(SoftModule &softmodule, FixedModule &currentmodule);
    bool checkoverlap_fix(FixedModule &softmodule, FixedModule &currentmodule);
    bool checkover_legal(FixedModule &softmodule, ParsedData &parsedData);
    bool swap_process(ParsedData &parsedData);
    void swap(FixedModule &target1, FixedModule &target2);
    bool rotate_process(ParsedData &parsedData);
    bool resize_process(ParsedData &parsedData);
    bool move_process(ParsedData &parsedData);
    void SA(ParsedData &parsedData);
    void SA_avg(ParsedData &parsedData);
};

#endif