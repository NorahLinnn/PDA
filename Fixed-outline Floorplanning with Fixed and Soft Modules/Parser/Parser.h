#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <vector>
struct ParsedData;

struct ChipSize {
    int width;
    int height;
};

class modules
{
    public:
    int xCoordinate;
    int yCoordinate;
    long long minArea;
    int width;
    int height;
};


class SoftModule : public modules{
    public:
    std::string name;
    int soft_id;
    
    // int width; 
    int min_width;
    int max_width;
    int min_height;
    int max_height;
    double slope;
    int ci;
    // int height; 
    SoftModule(): modules(){};
    // int xCoordinate;
    // int yCoordinate;
};

class FixedModule : public modules{
    public:
    std::string name;
    int fix_id;
    // int xCoordinate;
    // int yCoordinate;
    // int width;
    // int height;
};


struct Net {
    std::string module1Name;
    std::string module2Name;
    int module1id;
    int module2id;
    int weight;
};
struct ParsedData {
    ChipSize chipSize;
    int num_softModules;
    int num_fixedModules;
    int num_net;
    double delta_avg;
    std::vector<SoftModule> softModules;
    std::vector<FixedModule> fixedModules;
    std::vector<FixedModule> tmp_fix;
    std::vector<Net> nets;
    int wirelength;
    int wirelength_2;
};
class Parser {
public:
    ParsedData parseInputFile(const std::string &filename);
    bool sortarea(const SoftModule softmodule1, const SoftModule softmodule2);
    
private:
    ParsedData parsedData;
};
#endif