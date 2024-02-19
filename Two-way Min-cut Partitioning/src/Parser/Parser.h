#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
struct ParsedData;
struct Net;
struct LibCell {
    std::string name;
    int lib_id;
    int width;
    int height;
};
struct Tech {
    std::string name;
    int tech_id;
    std::vector<LibCell> libCells_vector;
};
struct Node {
    Node *prev, *next;

    Node() : prev(nullptr), next(nullptr){}
};
struct Cell: Node{
    std::string name;
    std::string libCellName;
    // std::string diename;
    // std::vector<std::string> netlist;
    std::vector<int> netlist;
    int pin_num;
    int die_id, gain;
    bool lock;
    int id, lib_id;
};

struct Net {
    std::string name;

    std::vector<int> cell_list;
    std::vector<int> numCellInGroup;
    void die_cell_num(ParsedData &data, Net &net_name);
    int net_id;

    
};
struct Die {
    std::string name; 
    std::string tech; 
    int tech_id;
    int maxArea; 
    int pMax, bucket_length;
    long long current_area, spare_area;
    std::vector<Cell> bucketList_vector;
    void initBucklist(int die_id, ParsedData &data);
    void insertNode(Cell *cell);
    std::pair<int, int> getBaseCell(ParsedData &data);
    void removeNode(Cell *cell);
    void moveNode(Cell *cell);
    Die() :bucket_length(0), current_area(0), spare_area(0){}

};

struct ParsedData {
    int numTechs;
    int dieWidth;
    int dieHeight;
    std::string dieATech;
    long long dieAarea;
    long long dieBarea;
    int dieAUtilization;
    std::string dieBTech;
    int dieBUtilization;
    int numCells;
    int numNets;
    int cutSize, mincut, loop_mincut, temp_cutsize;
    int pmax, num_of_lock, gain_change;
    std::vector<std::string> min_cut_diea;
    std::vector<std::string> min_cut_dieb;
    std::vector<int> min_cut_diea_idx;
    std::vector<int> min_cut_dieb_idx;

    std::vector<std::string> loop_min_cut_diea;
    std::vector<std::string> loop_min_cut_dieb;
    std::vector<int> loop_min_cut_diea_idx;
    std::vector<int> loop_min_cut_dieb_idx;
    
    std::vector<Tech> techs_vector;
    std::vector<Cell> cells_vector;
    std::vector<Net> nets_vector;
    std::vector<Die> dies;
    ParsedData() : num_of_lock(0), gain_change(0){}
};

class Parser {
public:
    ParsedData parseInputFile(const std::string &filename);
    
private:
    ParsedData parsedData;
};

#endif 