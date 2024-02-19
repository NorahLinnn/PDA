#include <iostream>
#include <chrono>
#include "Parser/Parser.h"
#include "checkParser/check.h"
#include "Partition/Partition.h"
#include "ResultWriter/ResultWriter.h"
auto start = std::chrono::high_resolution_clock::now();

int main(int argc, char *argv[]) 
// int main() 
{
    
    Parser parser;
    std::cout << "hi" << std::endl;
    // ParsedData data = parser.parseInputFile("/users/course/2023F/cs613500110003/u111065519/hw2/HW2/testcase/public6.txt");
    // std::cout << argv[1] << std::endl;
    ParsedData data = parser.parseInputFile(argv[1]);
    
    std::cout << "end parse" << std::endl;
    InitialPartition initial;
    initial.createInitialPartition(data);
    // printParsedData(data);
    auto end_init = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed_init = end_init - start;
    std::cout << "_init_runtime: " << elapsed_init.count() << " second" << std::endl;
    std::cout << "end init" << std::endl;
    FMalgo FM;
    FM.calculateCutSize(data);
    std::cout << "cut before: " << data.cutSize << std::endl;
    
    FM.fm(data);
    // printParsedData(data);
    // printmincut(data);
    FM.calculateCutSize(data);
    std::cout << "cut after: " << data.cutSize << std::endl;
    ResultWriter rw;
    rw.writeResult(data,argv[2]);
    // rw.writeResult(data,"/users/course/2023F/cs613500110003/u111065519/hw2/HW2/output/public6.out");
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "runtime: " << elapsed.count() << " second" << std::endl;
    return 0;
}
