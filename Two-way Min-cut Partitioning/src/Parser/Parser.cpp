#include "Parser.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <unordered_map>

ParsedData Parser::parseInputFile(const std::string &filename) {
    ParsedData input;
    input.pmax = 0;
    input.dies.resize(2);
    std::unordered_map<std::string, int> name_list;
    std::unordered_map<std::string, int> lib_name_list;
    std::unordered_map<std::string, int> tech_name_list;
    
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string word;
        iss >> word;
        // std::cout << "Processing line: " << line << std::endl;
        if (line.find("NumTechs") != std::string::npos) {
            iss >> input.numTechs;
        } else if (line.find("DieSize") != std::string::npos) {
            iss >> input.dieWidth >> input.dieHeight;
        } else if (line.find("NumCells") != std::string::npos) {
            iss >> input.numCells;
        } else if (line.find("NumNets") != std::string::npos) {
            iss >> input.numNets;
        } else if (line.find("Tech") != std::string::npos&& line.find("Num") == std::string::npos) {
            // std::cout << line << std::endl;
            Tech tech;
            int numLibCells;
            iss >> tech.name >> numLibCells;
            // std::cout << "tech: " << tech.name << numLibCells << std::endl;
            for (int i = 0; i < numLibCells; i++) {
                std::getline(file, line);
                std::istringstream libCellStream(line);
                std::string libCellWord;
                libCellStream >> libCellWord;
                if (libCellWord == "LibCell") {
                    LibCell libCell;
                    libCellStream >> libCell.name >> libCell.width >> libCell.height;
                    // std::cout << "libcell: " << libCell.name<< std::endl;
                    // tech.libCells.emplace(libCell.name, libCell);
                    lib_name_list[libCell.name] = i;
                    tech.libCells_vector.push_back(libCell);
                }
            }
            // input.techs.emplace(tech.name, tech);
            tech.tech_id = input.techs_vector.size();
            input.techs_vector.push_back(tech);
            // std::cout << "tech: " << tech.name << tech.libCells.size() << std::endl;

        }else if (line.find("DieA") != std::string::npos) {
            iss >> input.dieATech >> input.dieAUtilization;
            input.dies[0].tech = input.dieATech;
            input.dies[0].maxArea = input.dieAUtilization;
        } else if (line.find("DieB") != std::string::npos) {
            iss >> input.dieBTech >> input.dieBUtilization;
            input.dies[1].tech = input.dieBTech;
            input.dies[1].maxArea = input.dieBUtilization;
        } else if (line.find("Cell") != std::string::npos && line.find("LibCell") == std::string::npos) {
            Cell cell;
            cell.pin_num = 0;
            iss >> cell.name >> cell.libCellName;
            cell.id = input.cells_vector.size();
            cell.lib_id = lib_name_list[cell.libCellName];
            name_list[cell.name] = cell.id;
            // input.cells.emplace(cell.name, cell);
            // std::cout << cell.name.substr(1) << std::endl;
            // std::cout << std::stoi(cell.name.substr(1))-1 << std::endl;
            input.cells_vector.push_back(cell);
            // input.cells[cell.name] = cell;
        } else if (line.find("Net") != std::string::npos) {
            // std::cout << "hi" << std::endl;
            Net temp_net;
            int numConnectedCells;
            iss >> temp_net.name >> numConnectedCells;
            input.nets_vector.push_back(temp_net);
            
            Net &net = input.nets_vector.back();
            net.net_id = std::stoi(temp_net.name.substr(1))-1;
            for (int i = 0; i < numConnectedCells; i++) {
                // std::cout << "hi_2" << std::endl;
                // std::cout << net.name << std::endl;
                std::getline(file, line);
                std::istringstream netCellStream(line);
                std::string netCellWord, cellName;
                netCellStream >> netCellWord >> cellName;
                // net.connectedCells.push_back(cellName);
                auto cell_name = name_list[cellName];
                // auto cell_name = cellName.substr(1)-1;
                // std::cout << cellName << std::endl;
                input.cells_vector[cell_name].pin_num += 1;
                if( input.cells_vector[cell_name].pin_num > input.pmax){
                    input.pmax = input.cells_vector[cell_name].pin_num;
                }
                // input.cells[cellName].netlist.push_back(net.name);
                // input.cells_vector[cell_name].netlist.push_back(net.name);
                
                net.cell_list.push_back(input.cells_vector[cell_name].id);
                
                input.cells_vector[cell_name].netlist.push_back(net.net_id);
            }
            net.numCellInGroup.resize(2);
            // input.nets.emplace(net.name, net);
            
        }
        int i = 0;
        for(auto& tech:input.techs_vector)
        {
            if (tech.name == input.dies[0].tech)
            {
                input.dies[0].tech_id = i;
            }
            if (tech.name == input.dies[1].tech)
            {
                input.dies[1].tech_id = i;
            }
            i ++ ;
        }
        
    }
    return input;
}
void Die::initBucklist(int die_id, ParsedData &data){
    // for(int i = -data.pmax; i<=data.pmax;i++){
    //     data.dies[die_id].bucketList[i];
    // }
    data.dies[die_id].bucketList_vector.resize(data.pmax*2+1);
}
void Die::insertNode(Cell *cell){
// void Die::insertNode(ParsedData &data, std::string cell_name){
    ++bucket_length;
    // std::cout << cell->name << " and " << cell->gain << std::endl;
    // auto head = &bucketList[cell->gain];
    
    // cell->prev = head;
    
    // cell->next = head->next;
    // if (head->next){
    //     head->next->prev = cell;
    // }

    // head->next = cell;
    auto pmx = (bucketList_vector.size()-1)/2;
    auto head = &bucketList_vector[(cell->gain)+pmx];
    
    cell->prev = head;
    cell->next = head->next;
    if (head->next){
        head->next->prev = cell;
    }
    head->next = cell;
    
    // ++bucket_length;
    // auto idx = data.cells[cell_name].die_id;
    // auto cell_gain = data.cells[cell_name].gain;
    // std::cout << data.cells[cell_name].name << " and " << data.cells[cell_name].gain << std::endl;
    // auto head = &data.dies[idx].bucketList[cell_gain];
    // data.cells[cell_name].prev = head;
    // data.cells[cell_name].next = head->next;
    // if (head->next)
    // {
    //     head->next->prev = &data.cells[cell_name];
    // }
    // head->next = &data.cells[cell_name];
    // std::cout << &data.dies[data.cells[cell_name].die_id].bucketList[data.cells[cell_name].gain].next << std::endl;
    // std::cout << data.dies[data.cells[cell_name].die_id].bucketList[data.cells[cell_name].gain]->next << std::endl;
}
void Die::removeNode(Cell *cell){
    --bucket_length;
    if (cell->next)
        cell->next->prev = cell->prev;
    cell->prev->next = cell->next;
    cell->prev = cell->next = nullptr;
}

// char lastChar = str.back(); // 取出最後一個字元
//         if (isdigit(lastChar)) {
//             int lastDigit = lastChar - '0'; // 轉換為整數
void Net::die_cell_num(ParsedData &data, Net &net_name) {
    net_name.numCellInGroup[0] = net_name.numCellInGroup[1] = 0;
    // std::cout << " check " << std::endl;
    for (auto& cell: data.nets_vector[net_name.net_id].cell_list){
        // std::cout << " check_2 " << std::endl;
        // auto cell_name = std::stoi(cell.substr(1))-1;
        // std::cout << data.cells_vector[cell_name].die_id << " " << data.cells_vector[cell_name].name << std::endl;
        // ++numCellInGroup[data.cells[cell].die_id];
        // std::cout << i << std::endl;
        ++net_name.numCellInGroup[data.cells_vector[cell].die_id];
    }
}

