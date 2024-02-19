#include "check.h"
#include <iostream>

void printLibraryCell(const LibCell& cell) {
    std::cout << "LibCell Name: " << cell.name << ", Width: " << cell.width << ", Height: " << cell.height << std::endl;
}

void printTechnology(const Tech& tech) {
    std::cout << "Tech Name: " << tech.name << ", Num of LibCells: " << tech.libCells_vector.size() << std::endl;
    for (const auto& cellPair : tech.libCells_vector) {
        printLibraryCell(cellPair);
    }
}
// std::pair<int, int> findLibraryCellSize(const std::string& libCellName, const std::vector<Tech>& technologies) {
//     for (const auto& tech : technologies) {
//         for (const auto& libCell : tech.libCells) {
//             if (libCell.name == libCellName) {
//                 return {libCell.width, libCell.height};
//             }
//         }
//     }
//     std::cout << "Could not find size for: " << libCellName << std::endl;  // Debug print statement
//     return {-1, -1}; // Return invalid size if not found
// }
void printCell(const Cell& cell){
    std::cout << "Cell Name: " << cell.name << ", LibCell Name: " << cell.libCellName << std::endl;
    for(const auto net:cell.netlist){
        std::cout << "Net Name: " << net << " ";
    }
    // auto [width, height] = findLibraryCellSize(cell.libCellName, technologies);
    // std::cout << "Processing Cell: " << cell.name << ", LibCell Name: " << cell.libCellName << std::endl;  // Debug print
    // std::cout << "Cell Name: " << cell.name << ", LibCell Name: " << cell.libCellName;
    // std::cout << ", Size: " << width << "x" << height << std::endl;
    std::cout << std::endl;
    std::cout << "Die id: " << cell.die_id <<std::endl;
    std::cout << "Cell gain: " << cell.gain << std::endl;
}

void printNet(const Net& net) {
    std::cout << "Net Name: " << net.name << ", Cells: ";
    for (const auto& cellName : net.cell_list) {
        std::cout << cellName << " ";
    }
    std::cout << std::endl;
    // for (const auto& cell: net.cell_list){
    //     std::cout<<"stest" <<std::endl;
    //     std::cout<<cell.name<<std::endl;
    // }
}
void printDie(const Die& die) {
    std::cout << "Die" << die.name << " (Tech: " << die.tech << ", Utilization: " << die.maxArea << "%)" << std::endl;
}

void printParsedData(const ParsedData& data) {
    std::cout << "NumTechs: " << data.numTechs << std::endl;
    // for (const auto& tech : data.techs) {
    //     std::cout << tech.first << std::endl;
    //     printTechnology(tech.second);
    // }
    for(auto cell:data.cells_vector){
        std::cout << data.cells_vector[std::stoi(cell.name.substr(1))-1].name << std::endl;
    }
    // std::cout << data.cells_vector[0].name << std::endl;
    // std::cout << "DieSize: " << data.dieWidth << "x" << data.dieHeight << std::endl;
    // std::cout << "DieA (Tech: " << data.dieATech << ", Utilization: " << data.dieAUtilization << "%)" << std::endl;
    // std::cout << "DieB (Tech: " << data.dieBTech << ", Utilization: " << data.dieBUtilization << "%)" << std::endl;
    // for (const auto &dieA : data.dieACells) {
    //     std::cout << "DieA Cell Key: " << dieA.first << std::endl;
    //     for (const auto &cell : dieA.second) {
    //         std::cout << "Cell Name: " << cell.name << std::endl;
    //         std::cout << "Library Cell Name: " << cell.libCellName << std::endl;
    //     }

    // }

    // for (const auto &dieB : data.dieBCells) {
    //     std::cout << "DieB Cell Key: " << dieB.first << std::endl;
    //     for (const auto &cell : dieB.second) {
    //         std::cout << "Cell Name: " << cell.name << std::endl;
    //         std::cout << "Library Cell Name: " << cell.libCellName << std::endl;
    //     }
    // }
    // std::cout << "Cutsize: " << data.cutSize << std::endl;
    // // for (const auto& die : data.dies){
    // //     printDie(die.second);
    // // }
    std::cout << "NumCells: " << data.numCells << std::endl;
    // for (const auto& cell : data.cells) {
    //     printCell(cell.second);
    // }
    // std::cout << "NumNets: " << data.numNets << std::endl;
    // for (const auto& net : data.nets) {
    //     std::cout << net.first << std::endl;
    //     printNet(net.second);
    // }
    // std::cout << "check bucklist: "<< data.dies[0].bucketList_vector.size() << std::endl;
    // for (auto &bucket : data.dies[0].bucketList_vector) {
    //     std::cout << "Bucket with gain " << bucket.name << std::endl;
    //     auto current = static_cast<Cell *>(bucket.next);
    //     while (current) {
    //         std::cout << "Cell Name: " << current->name << " " << current->lock << " " << current->gain << std::endl;
    //         current = static_cast<Cell *>(current->next);
    //     }
    //     std::cout << "------------------" << std::endl;
    // }
    // std::cout << "check bucklist: "<< data.dies[1].bucketList_vector.size() << std::endl;
    // for (auto &bucket : data.dies[1].bucketList_vector) {
    //     std::cout << "Bucket with gain " << bucket.name << std::endl;
    //     auto current = static_cast<Cell *>(bucket.next);
    //     while (current) {
    //         std::cout << "Cell Name: " << current->name <<  " " << current->lock << " " << current->gain << std::endl; // 這裡假設有一個名為 name 的屬性
    //         current = static_cast<Cell *>(current->next);
    //     }
    //     std::cout << "------------------" << std::endl;
    // }

}
void printmincut(const ParsedData& data) 
{
    std::cout << "in die a: "<< std::endl;
    for(auto cell: data.min_cut_diea)
    {
        std::cout << cell << " ";
    }
    std::cout << std::endl;
    std::cout << "in die b: "<< std::endl;
    for(auto cell: data.min_cut_dieb)
    {
        std::cout << cell << " ";
    }
}
