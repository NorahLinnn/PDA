#ifndef RESULTWRITER_H
#define RESULTWRITER_H
#include "../Parser/Parser.h"
#include <memory>
#include <string>
#include <vector>

class ResultWriter {
public:
    void writeResult(const ParsedData &data, const std::string &outputFileName);
};

#endif