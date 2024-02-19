#include <iostream>
#include <chrono>
#include "Parser/Parser.h"
#include "check/check.h"
#include "floorplan/floorplan.h"
#include "writer/writer.h"
auto start = std::chrono::high_resolution_clock::now();
// int main() 
int main(int argc, char *argv[]) 
{
    
    Parser parser;
    std::cout << "hi" << std::endl;
    // std::cout << argv[1] << std::endl;
    ParsedData data = parser.parseInputFile(argv[1]);
    // ParsedData data = parser.parseInputFile("/users/course/2023F/cs613500110003/u111065519/hw3/HW3/testcase/public2.txt");
    auto end_parsed = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end_parsed - start;
    std::cout << "parsed runtime: " << elapsed.count() << " second" << std::endl;
    srand(70);
    Floorplan floorplan;
    floorplan.createInitial(data);
    auto end_init = std::chrono::high_resolution_clock::now();

    elapsed = end_init - end_parsed;
    std::cout << "Initial runtime: " << elapsed.count() << " second" << std::endl;
    floorplan.countWirelength(data);
    std::cout << "wirelength: " << data.wirelength << std::endl;
    auto end_count = std::chrono::high_resolution_clock::now();

    elapsed = end_count - end_init;
    std::cout << "count wirelength runtime: " << elapsed.count() << " second" << std::endl;
    // floorplan.SA(data);
    floorplan.SA_avg(data);
    floorplan.SA(data);
    auto end_sa = std::chrono::high_resolution_clock::now();

    elapsed = end_sa - end_count;
    std::cout << "sa runtime: " << elapsed.count() << " second" << std::endl;
    ResultWriter resultwriter;
    resultwriter.writeResult(data, argv[2]);
    // resultwriter.writeResult(data, "/users/course/2023F/cs613500110003/u111065519/hw3/HW3/output/public2.floorplan");
    // checkAndPrint(data);
    auto end_write = std::chrono::high_resolution_clock::now();

    elapsed = end_write - end_sa;
    std::cout << "write file runtime: " << elapsed.count() << " second" << std::endl;

    std::cout << "end" << std::endl;
}