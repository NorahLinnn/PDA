#include "ResultWriter.h"
#include <fstream>
#include <iostream>

void ResultWriter::writeResult(const ParsedData &data, const std::string &outputFileName) {
    std::ofstream outputFile(outputFileName);

    if (!outputFile.is_open()) {
        std::cerr << "Error opening file: " << outputFileName << std::endl;
        return;
    }

    outputFile << "CutSize " << data.loop_mincut << std::endl;

    outputFile << "DieA " << data.loop_min_cut_diea.size() << std::endl;
    for (const auto cell : data.loop_min_cut_diea) {
        outputFile << cell << std::endl;
    }

    outputFile << "DieB " << data.loop_min_cut_dieb.size() << std::endl;
    for (const auto cell : data.loop_min_cut_dieb) {
        outputFile << cell << std::endl;
    }

    outputFile.close();
}
