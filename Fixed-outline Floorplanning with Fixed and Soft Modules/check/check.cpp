#include "check.h"
#include <iostream>

void checkAndPrint(const ParsedData& data) {
    // Access and print data here
    std::cout << "Chip Size: " << data.chipSize.width << " x " << data.chipSize.height << std::endl;
    std::cout << "Number of Soft Modules: " << data.softModules.size() << std::endl;
    // Example: Print Soft Modules
    // for (const auto& softModule : data.softModules) {
    //     std::cout << "Soft Module: " << softModule.name << ", ID: " << softModule.soft_id << ", Min Area: " << softModule.minArea << std::endl;
    //     std::cout << "min width: " << softModule.min_width << ", max width: " << softModule.max_width << ", min height: " << softModule.min_height << ", max height: " << softModule.max_height << ", slope: " << softModule.slope << ", ci: " << softModule.ci << std::endl;
    //     std::cout << "height: " << softModule.height << ",width: " << softModule.width << std::endl;
    //     std::cout << "coordinates: " << softModule.xCoordinate << ", " << softModule.yCoordinate << std::endl;
    // }

    // // Example: Print Fixed Modules
    // for (const auto& fixedModule : data.fixedModules) {
    //     std::cout << "Fixed Module: " << fixedModule.name << ", ID: " << fixedModule.fix_id << ", Coordinates: (" << fixedModule.xCoordinate << ", " << fixedModule.yCoordinate << "), Size: " << fixedModule.width << " x " << fixedModule.height << std::endl;
    // }

    // Example: Print Nets
    for (const auto& net : data.nets) {
        std::cout << "Net: " << net.module1Name << " (ID: " << net.module1id << ") <-> " << net.module2Name << " (ID: " << net.module2id << "), Weight: " << net.weight << std::endl;
    }
}
