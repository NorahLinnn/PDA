#include "writer.h"
#include <fstream>
#include <iostream>

void ResultWriter::writeResult(const ParsedData &data, const std::string &outputFileName) {
    std::ofstream outputFile(outputFileName);

    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << outputFileName << std::endl;
        return;
    }
    std::cout << "start write" << std::endl;
    outputFile << "Wirelength " << data.wirelength << std::endl;
    outputFile << "NumSoftModules " << data.softModules.size() << std::endl;

    
    for (const auto module : data.softModules) {
        // bool is_soft = false;
        // for(const auto fix_module: data.fixedModules)
        // {
        //     if(fix_module.name == module.name)
        //     {
        //         is_soft = true;
        //         break;
        //     }
        //     else{
        //         is_soft = false;
        //     }
        // }
        // if(!is_soft)
        // {
        outputFile << module.name << " " << module.xCoordinate << " " << module.yCoordinate << " " << module.width << " " << module.height << std::endl;
        // }
        
    }


    outputFile.close();
}
