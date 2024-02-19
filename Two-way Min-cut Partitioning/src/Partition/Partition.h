#ifndef PARTITION_H
#define PARTITION_H

#include "../Parser/Parser.h"

class InitialPartition {
public:
    std::vector<Die> group_a;
    std::vector<Die> group_b;
    void createInitialPartition(ParsedData &parsedData);
    void compute_gain(ParsedData &parsedData);
    
    // , std::vector<Cell> &partitionA, std::vector<Cell> &partitionB
};
class FMalgo{
public:
    std::tuple<int, int, Node *> getBaseCell(ParsedData &data);
    void fm(ParsedData &data);
    void calculateCutSize(ParsedData &parsedData);
    void update_gain(ParsedData &data, Cell base_cell);
    void fm_init(ParsedData &data);
};
#endif