#include "Parser.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
bool sortarea(const SoftModule& softmodule1, const SoftModule& softmodule2)
{
    return softmodule1.minArea > softmodule2.minArea;
}
ParsedData Parser::parseInputFile(const std::string &filename) 
{
    ParsedData input;
    std::ifstream file(filename);
    std::unordered_map<std::string, int> namelist;
    std::unordered_map<std::string, int> soft_namelist;
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }
    std::string line;
    while (std::getline(file, line)) 
    {
        std::istringstream iss(line);
        std::string word;
        iss >> word;
        if (line.find("ChipSize") != std::string::npos) 
        {
            // std::cout << line << std::endl;
            iss >> input.chipSize.width >> input.chipSize.height;
        }else if (line.find("NumSoftModules")!= std::string::npos) 
        {
            iss >> input.num_softModules;
        }else if(line.find("SoftModule")!= std::string::npos && line.find("NumSoftModules") == std::string::npos)
        // }else if(word == "SoftModule")
        {
            SoftModule tmp_soft;
            // std::cout << line << std::endl;
            iss >> tmp_soft.name >> tmp_soft.minArea;
            tmp_soft.soft_id = input.softModules.size();
            namelist[tmp_soft.name] = tmp_soft.soft_id;
            // std::cout << tmp_soft.minArea << std::endl;
            tmp_soft.min_width = ceil(sqrt(tmp_soft.minArea*0.5));
            tmp_soft.max_width = ceil(sqrt(tmp_soft.minArea*2));
            tmp_soft.min_height = ceil(sqrt(tmp_soft.minArea/2));
            tmp_soft.max_height = ceil(sqrt(tmp_soft.minArea/0.5));
            if(tmp_soft.max_width*tmp_soft.min_width >= tmp_soft.minArea && 0.5<=tmp_soft.max_height/tmp_soft.min_width<=2 && 0.5<=tmp_soft.min_height/tmp_soft.max_width<=2)
            {
                tmp_soft.max_height -= 1;
                tmp_soft.max_width -= 1;
            }
            tmp_soft.slope = (tmp_soft.max_height-tmp_soft.min_height)/(tmp_soft.min_width-tmp_soft.max_width);
            tmp_soft.ci = tmp_soft.max_height-(tmp_soft.slope*tmp_soft.min_width);
            input.softModules.push_back(tmp_soft);
        }else if (line.find("NumFixedModules")!= std::string::npos) 
        {
            iss >> input.num_fixedModules;
        }else if(line.find("FixedModule")!= std::string::npos && line.find("NumFixedModules") == std::string::npos)
        // }else if(word == "FixedModules")
        {
            FixedModule tmp_fix;
            iss >> tmp_fix.name >> tmp_fix.xCoordinate >> tmp_fix.yCoordinate >> tmp_fix.width >> tmp_fix.height;
            tmp_fix.fix_id = input.fixedModules.size()+input.softModules.size();
            namelist[tmp_fix.name] = tmp_fix.fix_id;
            input.fixedModules.push_back(tmp_fix);
            tmp_fix.fix_id -= input.softModules.size();
            input.tmp_fix.push_back(tmp_fix);
        }else if (line.find("NumNet")!= std::string::npos)
        {
            iss >> input.num_net;
        }else if(line.find("Net")!= std::string::npos && line.find("NumNet") == std::string::npos)
        {
            Net tmp_net;
            iss >> tmp_net.module1Name >> tmp_net.module2Name >> tmp_net.weight;
            tmp_net.module1id = namelist[tmp_net.module1Name];
            tmp_net.module2id = namelist[tmp_net.module2Name];
            input.nets.push_back(tmp_net);
        }
        std::sort(input.softModules.begin(), input.softModules.end(),[](const SoftModule& a, const SoftModule& b) {
            return a.minArea > b.minArea;
        });
        int counter = 0;
        for(auto &softm:input.softModules)
        {
            softm.soft_id = counter;
            namelist[softm.name] = softm.soft_id;
            counter += 1;
        }
        for(auto &net:input.nets)
        {
            net.module1id = namelist[net.module1Name];
            net.module2id = namelist[net.module2Name];
        }
    }
    return input;
}

