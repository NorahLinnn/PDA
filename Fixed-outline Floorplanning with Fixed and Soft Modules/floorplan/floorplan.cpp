#include "floorplan.h"
#include <time.h>
#include <string>
#include <iostream>
#include <math.h>
#include <chrono>
#include <vector>

void Floorplan::swap(FixedModule &target1, FixedModule &target2)
{
    auto xcoord_temp = target1.xCoordinate;
    auto ycoord_temp = target1.yCoordinate;
    target1.xCoordinate = target2.xCoordinate;
    target1.yCoordinate = target2.yCoordinate;
    target2.xCoordinate = xcoord_temp;
    target2.yCoordinate = ycoord_temp;
}
bool Floorplan::swap_process(ParsedData &parsedData)
{
    if(parsedData.tmp_fix.size()<=parsedData.fixedModules.size()+2) return false;
    int mode_counter = 0;
    bool overlap_flag = true;
    bool success_flag = false;
    while (mode_counter <= 10 && overlap_flag)
    {
        mode_counter += 1;
        int target1_id = rand()%(parsedData.tmp_fix.size()-parsedData.fixedModules.size())+parsedData.fixedModules.size();
        int target2_id = rand()%(parsedData.tmp_fix.size()-parsedData.fixedModules.size())+parsedData.fixedModules.size();
        // std::cout << " -----" << std::endl;
        // std::cout << parsedData.tmp_fix[target1_id].name<<std::endl;
        // std::cout << parsedData.tmp_fix[target2_id].name<<std::endl;
        while (target1_id == target2_id)
        {
            target2_id = rand()%(parsedData.tmp_fix.size()-parsedData.fixedModules.size())+parsedData.fixedModules.size();
        }
        
        // std::cout << parsedData.tmp_fix.size() << " " << parsedData.fixedModules.size() << std::endl;
        // std::cout << "target1_id: " << target1_id << " ,target2_id: " << target2_id << std::endl;
        swap(parsedData.tmp_fix[target1_id], parsedData.tmp_fix[target2_id]);
        if(!checkover_legal(parsedData.tmp_fix[target1_id], parsedData))
        {
            swap(parsedData.tmp_fix[target1_id], parsedData.tmp_fix[target2_id]);
            continue;
        }
        if(!checkover_legal(parsedData.tmp_fix[target2_id], parsedData))
        {
            swap(parsedData.tmp_fix[target1_id], parsedData.tmp_fix[target2_id]);
            continue;
        }
        for(auto &temp_fix: parsedData.tmp_fix)
        {
            if (temp_fix.name != parsedData.tmp_fix[target1_id].name)
            {
                if(checkoverlap_fix(temp_fix, parsedData.tmp_fix[target1_id]))
                {
                    overlap_flag = false;
                    success_flag = true;
                }else
                {
                    mode_counter += 1;
                    swap(parsedData.tmp_fix[target1_id], parsedData.tmp_fix[target2_id]);
                    overlap_flag = true;
                    break;
                }
            }
            if (temp_fix.name != parsedData.tmp_fix[target2_id].name)
            {
                if(checkoverlap_fix(temp_fix, parsedData.tmp_fix[target2_id]))
                {
                    overlap_flag = false;
                    success_flag = true;
                }else
                {
                    mode_counter += 1;
                    swap(parsedData.tmp_fix[target1_id], parsedData.tmp_fix[target2_id]);
                    overlap_flag = true;
                    break;
                }
            }
        }
        if(!overlap_flag)
        {
            parsedData.softModules[parsedData.tmp_fix[target1_id].fix_id].xCoordinate = parsedData.tmp_fix[target1_id].xCoordinate;
            parsedData.softModules[parsedData.tmp_fix[target1_id].fix_id].yCoordinate = parsedData.tmp_fix[target1_id].yCoordinate;
            parsedData.softModules[parsedData.tmp_fix[target2_id].fix_id].xCoordinate = parsedData.tmp_fix[target2_id].xCoordinate;
            parsedData.softModules[parsedData.tmp_fix[target2_id].fix_id].yCoordinate = parsedData.tmp_fix[target2_id].yCoordinate;
        }
    }
    return success_flag;
}
bool Floorplan::rotate_process(ParsedData &parsedData)
{
    if(parsedData.tmp_fix.size()-parsedData.fixedModules.size()==0) return false;
    int mode_counter = 0;
    bool overlap_flag = true;
    bool success_flag = false;
    while (mode_counter <= 10 && overlap_flag)
    {
        mode_counter += 1;
        // std::cout <<  ((parsedData.tmp_fix.size()-parsedData.fixedModules.size()))<< std::endl;
        int target1_id = (rand()%(parsedData.tmp_fix.size()-parsedData.fixedModules.size()))+parsedData.fixedModules.size();
        // std::cout << " -----" << std::endl;
        // std::cout << parsedData.tmp_fix[target1_id].name<<std::endl;
        int temp = parsedData.tmp_fix[target1_id].width;
        parsedData.tmp_fix[target1_id].width = parsedData.tmp_fix[target1_id].height;
        parsedData.tmp_fix[target1_id].height = temp;
        if(!checkover_legal(parsedData.tmp_fix[target1_id], parsedData))
        {
            temp = parsedData.tmp_fix[target1_id].width;
            parsedData.tmp_fix[target1_id].width = parsedData.tmp_fix[target1_id].height;
            parsedData.tmp_fix[target1_id].height = temp;
            continue;
        }
        for(auto &temp_fix: parsedData.tmp_fix)
        {
            if (temp_fix.name != parsedData.tmp_fix[target1_id].name)
            {
                if(checkoverlap_fix(temp_fix, parsedData.tmp_fix[target1_id]))
                {
                    // std::cout << "hi" << std::endl;
                    overlap_flag = false;
                    success_flag = true;
                }else
                {
                    
                    mode_counter += 1;
                    temp = parsedData.tmp_fix[target1_id].width;
                    parsedData.tmp_fix[target1_id].width = parsedData.tmp_fix[target1_id].height;
                    parsedData.tmp_fix[target1_id].height = temp;
                    overlap_flag = true;
                    break;
                }
            }
        }
        if(!overlap_flag)
        {
            parsedData.softModules[parsedData.tmp_fix[target1_id].fix_id].width = parsedData.tmp_fix[target1_id].width;
            parsedData.softModules[parsedData.tmp_fix[target1_id].fix_id].height = parsedData.tmp_fix[target1_id].height;
        }
    }
    return success_flag;
}

bool Floorplan::move_process(ParsedData &parsedData)
{
    if(parsedData.tmp_fix.size()-parsedData.fixedModules.size()==0) return false;
    int mode_counter = 0;
    bool overlap_flag = true;
    bool success_flag = false;
    while (mode_counter <= 10 && overlap_flag)
    {
        mode_counter += 1;

        int target1_id = rand()%(parsedData.tmp_fix.size()-parsedData.fixedModules.size())+parsedData.fixedModules.size();
        // std::cout << " -----" << std::endl;
        // std::cout << parsedData.tmp_fix[target1_id].name<<std::endl;
        int target_soft_id = parsedData.tmp_fix[target1_id].fix_id;
        int old_xcoord = parsedData.tmp_fix[target1_id].xCoordinate;
        int old_ycoord = parsedData.tmp_fix[target1_id].yCoordinate;
        int target_xcoord = rand()%(parsedData.chipSize.width-parsedData.tmp_fix[target1_id].width+1);
        int target_ycoord = rand()%(parsedData.chipSize.height-parsedData.tmp_fix[target1_id].height+1);
        if(target_xcoord+parsedData.tmp_fix[target1_id].width>parsedData.chipSize.width || target_ycoord+parsedData.tmp_fix[target1_id].height>parsedData.chipSize.height)
        {
            parsedData.tmp_fix[target1_id].xCoordinate = old_xcoord;
            parsedData.tmp_fix[target1_id].yCoordinate = old_ycoord;
            continue;
        }
        parsedData.tmp_fix[target1_id].xCoordinate = target_xcoord;
        parsedData.tmp_fix[target1_id].yCoordinate = target_ycoord;
        if(!checkover_legal(parsedData.tmp_fix[target1_id], parsedData))
        {
            parsedData.tmp_fix[target1_id].xCoordinate = old_xcoord;
            parsedData.tmp_fix[target1_id].yCoordinate = old_ycoord;
            continue;
        }
        for(auto &temp_fix: parsedData.tmp_fix)
        {
            if (temp_fix.name != parsedData.tmp_fix[target1_id].name)
            {
                if(checkoverlap_fix(temp_fix, parsedData.tmp_fix[target1_id]))
                {
                    overlap_flag = false;
                    success_flag = true;
                }else
                {
                    
                    mode_counter += 1;
                    parsedData.tmp_fix[target1_id].xCoordinate = old_xcoord;
                    parsedData.tmp_fix[target1_id].yCoordinate = old_ycoord;
                    overlap_flag = true;
                    break;
                }
            }
        }
        if(!overlap_flag)
        {
            parsedData.softModules[parsedData.tmp_fix[target1_id].fix_id].xCoordinate = parsedData.tmp_fix[target1_id].xCoordinate;
            parsedData.softModules[parsedData.tmp_fix[target1_id].fix_id].yCoordinate = parsedData.tmp_fix[target1_id].yCoordinate;
        }
    }
    return success_flag;
}
bool Floorplan::resize_process(ParsedData &parsedData)
{
    if(parsedData.tmp_fix.size()-parsedData.fixedModules.size()==0) return false;
    int mode_counter = 0;
    bool overlap_flag = true;
    bool success_flag = false;
    while (mode_counter <= 10 && overlap_flag)
    {
        mode_counter += 1;
        int target1_id = rand()%(parsedData.tmp_fix.size()-parsedData.fixedModules.size())+parsedData.fixedModules.size();
        // std::cout << " -----" << std::endl;
        // std::cout << parsedData.tmp_fix[target1_id].name<<std::endl;
        int target_soft_id = parsedData.tmp_fix[target1_id].fix_id;
        int old_width = parsedData.tmp_fix[target1_id].width;
        int old_height = parsedData.tmp_fix[target1_id].height;
        int target_width = rand()%(parsedData.softModules[target_soft_id].max_width-parsedData.softModules[target_soft_id].min_width+1)+parsedData.softModules[target_soft_id].min_width;
        int target_height = ceil(parsedData.softModules[target_soft_id].minArea/(double)target_width);
        if((double)target_height/target_width>2 || (double)target_height/target_width<0.5)
        {
            parsedData.tmp_fix[target1_id].width = old_width;
            parsedData.tmp_fix[target1_id].height = old_height;
            continue;
        }
        parsedData.tmp_fix[target1_id].width = target_width;
        parsedData.tmp_fix[target1_id].height = target_height;
        if(!checkover_legal(parsedData.tmp_fix[target1_id], parsedData))
        {
            parsedData.tmp_fix[target1_id].width = old_width;
            parsedData.tmp_fix[target1_id].height = old_height;
            continue;
        }
        for(auto &temp_fix: parsedData.tmp_fix)
        {
            if (temp_fix.name != parsedData.tmp_fix[target1_id].name)
            {
                if(checkoverlap_fix(temp_fix, parsedData.tmp_fix[target1_id]))
                {
                    overlap_flag = false;
                    success_flag = true;
                }else
                {
                    mode_counter += 1;
                    parsedData.tmp_fix[target1_id].width = old_width;
                    parsedData.tmp_fix[target1_id].height = old_height;
                    overlap_flag = true;
                    break;
                }
            }
        }
        if(!overlap_flag)
        {
            parsedData.softModules[parsedData.tmp_fix[target1_id].fix_id].width = parsedData.tmp_fix[target1_id].width;
            parsedData.softModules[parsedData.tmp_fix[target1_id].fix_id].height = parsedData.tmp_fix[target1_id].height;
        }
    }
    return success_flag;
}
bool Floorplan::perturbation(ParsedData &parsedData)
{
    // for(auto &temp_fix: parsedData.tmp_fix)
    // {
    int mode_id = rand()%(3+1);
    bool perturbation_result= false;
    
    // int mode_id = 2;
    if(mode_id == 0)
    {
        // std::cout << "swap" << std::endl;
        perturbation_result = swap_process(parsedData);
    }
    if(mode_id == 1)
    {
        // std::cout << "rotate" << std::endl;
        perturbation_result = rotate_process(parsedData);
    }
    if(mode_id == 2)
    {
        // std::cout << "resize" << std::endl;
        perturbation_result = resize_process(parsedData);
    }
    if(mode_id == 3)
    {
        // std::cout << "move" << std::endl;
        perturbation_result = move_process(parsedData);
    }
    // std::cout << perturbation_result << std::endl;
    return perturbation_result;
}
bool Floorplan::checkoverlap_fix(FixedModule &softmodule, FixedModule &currentmodule){
    return ((currentmodule.xCoordinate+currentmodule.width <= softmodule.xCoordinate) || 
                (currentmodule.yCoordinate+currentmodule.height <=softmodule.yCoordinate) || 
                (softmodule.xCoordinate+softmodule.width <= currentmodule.xCoordinate) || 
                (softmodule.yCoordinate+softmodule.height <= currentmodule.yCoordinate));
}
bool Floorplan::checkover_legal(FixedModule &softmodule, ParsedData &parsedData){
    return (softmodule.xCoordinate+softmodule.width <= parsedData.chipSize.width &&  softmodule.yCoordinate+softmodule.height <= parsedData.chipSize.height);
}
bool Floorplan::checkoverlap(SoftModule &softmodule, FixedModule &currentmodule){
    // std::cout << "1: " << (currentmodule.xCoordinate+currentmodule.width <= softmodule.xCoordinate) << std::endl;
    // std::cout <<  "2: " << (currentmodule.yCoordinate+currentmodule.height <=softmodule.yCoordinate) << std::endl;
    // std::cout <<  "3: " << (softmodule.xCoordinate+softmodule.width <= currentmodule.xCoordinate) << std::endl;
    // std::cout <<  "4: " << (softmodule.yCoordinate+softmodule.height <= currentmodule.yCoordinate) << std::endl;
    // std::cout << "soft x: " << softmodule.xCoordinate << ", soft y: "<<softmodule.yCoordinate << ", soft w: " << softmodule.width << ", soft height: " << softmodule.height << std::endl;
    // std::cout << "currentmodule x: " << currentmodule.xCoordinate << ", soft y: "<<currentmodule.yCoordinate << ", currentmodule w: " << currentmodule.width << ", currentmodule height: " << currentmodule.height << std::endl;
    // std::cout << "current: " << currentmodule.name << " , soft:" << softmodule.name << std::endl;
    return ((currentmodule.xCoordinate+currentmodule.width <= softmodule.xCoordinate) || 
                (currentmodule.yCoordinate+currentmodule.height <=softmodule.yCoordinate) || 
                (softmodule.xCoordinate+softmodule.width <= currentmodule.xCoordinate) || 
                (softmodule.yCoordinate+softmodule.height <= currentmodule.yCoordinate));
}
void Floorplan::createInitial(ParsedData &parsedData)
{
    auto global_width = parsedData.chipSize.width;
    auto global_height = parsedData.chipSize.height;
    
    bool restart = true;
    std::vector<FixedModule> old_tmp_fix = parsedData.tmp_fix;
    while (restart)
    {    
        parsedData.tmp_fix = old_tmp_fix;
        int soft_num = 0;
        for(auto &softmodule:parsedData.softModules)
        {
            bool current_softmodule_loop = true;
            bool perturbation_success = false;
            int current_softmodeul_counter = 0;
            // std::cout << softmodule.name << " " << softmodule.minArea << std::endl;
            while (current_softmodule_loop && current_softmodeul_counter<=100)
            {    
                current_softmodeul_counter += 1;
                bool perturbation_success = false;
                // std::cout << softmodule.name << " " << softmodule.minArea << std::endl;
                
                FixedModule tmp_soft2fix;
                
                int counter=0;
                
                bool flag = true;
                while (flag && counter <= 1000)
                {
                    counter += 1;
                    softmodule.width = rand()%(softmodule.max_width-softmodule.min_width+1)+softmodule.min_width;
                    softmodule.height = ceil(softmodule.minArea/(double)softmodule.width);
                    // std::cout << "width height: " << softmodule.width << " " << softmodule.height << std::endl;
                    if((double)softmodule.height/softmodule.width>2 || (double)softmodule.height/softmodule.width<0.5)
                    {
                        continue;
                    }
                    // softmodule.height = softmodule.slope*softmodule.width+softmodule.ci;
                    softmodule.xCoordinate = rand()%(global_width-softmodule.width+1);
                    softmodule.yCoordinate = rand()%(global_height-softmodule.height+1);
                    if(softmodule.xCoordinate+softmodule.width>global_width || softmodule.yCoordinate+softmodule.height>global_height)
                    {
                        continue;
                    }
                    // std::cout << counter << std::endl;
                    for(auto &currentmodule: parsedData.tmp_fix)
                    {
                        
                        if(checkoverlap(softmodule, currentmodule))
                        {
                            // std::cout << "no overlap" << std::endl;
                            flag = false;
                        }else
                        {
                            flag=true;
                            break;
                        }
                    }
                    if (!flag)
                    {
                        // std::cout << "no overlap" << std::endl;
                        // std::cout << softmodule.name << std::endl;
                        parsedData.tmp_fix.push_back(tmp_soft2fix);
                        auto &tmp_soft_2_fix = parsedData.tmp_fix[soft_num+parsedData.fixedModules.size()];
                        
                        tmp_soft_2_fix.name = softmodule.name;
                        soft_num += 1;
                        tmp_soft_2_fix.fix_id = softmodule.soft_id;
                        tmp_soft_2_fix.width = softmodule.width;
                        tmp_soft_2_fix.height = softmodule.height;
                        tmp_soft_2_fix.xCoordinate = softmodule.xCoordinate;
                        tmp_soft_2_fix.yCoordinate = softmodule.yCoordinate;
                        current_softmodule_loop= false;
                        // std::cout << tmp_soft_2_fix.xCoordinate << " " << tmp_soft_2_fix.yCoordinate << std::endl;
                    }
                }
                // std::cout << " " << counter << std::endl;
                if(counter >1000 && flag)
                {
                    // while (!perturbation(parsedData, softmodule.soft_id))
                    // {
                    //     continue;
                    // }
                    perturbation_success = perturbation(parsedData);
                    
                    // if(perturbation(parsedData, softmodule.soft_id))
                    // {
                    // }

                    // std::cout << " stop " << current_softmodeul_counter  << " " << perturbation_success << std::endl;
                }
            }
            if(!current_softmodule_loop)
            {
                restart = false;
            }else{
                restart = true;
                break;
            }
        }
    }
}


void Floorplan::countWirelength(ParsedData &parsedata)
{
    int total_length = 0;
    int e_length = 0;
    for(auto &net: parsedata.nets)
    {
        
        modules& module1 = ((static_cast<size_t>(net.module1id) >= parsedata.softModules.size()) ? 
                     static_cast<modules&>(parsedata.fixedModules[net.module1id - parsedata.softModules.size()]) :
                     static_cast<modules&>(parsedata.softModules[net.module1id]));

        modules& module2 = ((static_cast<size_t>(net.module2id) >= parsedata.softModules.size()) ? 
                     static_cast<modules&>(parsedata.fixedModules[net.module2id - parsedata.softModules.size()]) :
                     static_cast<modules&>(parsedata.softModules[net.module2id]));

        // std::cout << "id: " << net.module1id << ", " << net.module2id <<std::endl;
        // std::cout << "x1: " << module1.xCoordinate << "y1, " << module1.yCoordinate << "w1: " << module1.width << "h1, " << module1.height << std::endl;
        // std::cout << "x2: " << module2.xCoordinate << "y2, " << module2.yCoordinate << "w1: " << module2.width << "h1, " << module2.height <<std::endl;
        total_length += (
            abs(
                (module1.xCoordinate+(module1.width)/2)-(module2.xCoordinate+(module2.width)/2)
                )+
            abs(
                (module1.yCoordinate+(module1.height)/2)-(module2.yCoordinate+(module2.height)/2)
            ))*net.weight;
        e_length += (
            pow(
                (module1.xCoordinate+(module1.width)/2)-(module2.xCoordinate+(module2.width)/2)
                ,2)+
            pow(
                (module1.yCoordinate+(module1.height)/2)-(module2.yCoordinate+(module2.height)/2)
            ,2))*net.weight;
    }
    parsedata.wirelength = total_length;
    parsedata.wirelength_2 = e_length;
}
extern std::chrono::_V2::system_clock::time_point start;
void Floorplan::SA_avg(ParsedData &parsedData)
{
    auto best_wire_lenngth = parsedData.wirelength;
    auto pre_wire_length = parsedData.wirelength;
    // vector<int> wire_length_move = {};
    auto tmp_data = parsedData;
    auto original_data = parsedData;
    int delta_cost = 0;
    int k = 7;
    int N = k*parsedData.num_softModules;
    int counter = 0;
    while (N >= 0)
    {
        
        pre_wire_length = parsedData.wirelength;
        bool success = perturbation(parsedData);
        if(success)
        {
            countWirelength(parsedData);
            if(parsedData.wirelength > best_wire_lenngth)
            {
                delta_cost += parsedData.wirelength-best_wire_lenngth;
            }
            if ((parsedData.wirelength < best_wire_lenngth))
            {
                
                best_wire_lenngth = parsedData.wirelength;
                tmp_data = parsedData;
            }
            N--;
        }else{
            parsedData = tmp_data;
        }
        counter++;
    }
    std::cout << "avg: " << delta_cost/(k*parsedData.num_softModules) << std::endl;
    std::cout << "p: " << log(0.99) << std::endl;
    
    std::cout << "counter: " << counter << std::endl;    
    
    parsedData = original_data;
    parsedData.delta_avg = delta_cost/(7*parsedData.num_softModules);
    std::cout << "T: " << (-parsedData.delta_avg)/log(0.98) << std::endl;
    // parsedData.delta_avg = delta_cost/counter;

}
void Floorplan::SA(ParsedData &parsedData)
{
    auto best_wire_lenngth = parsedData.wirelength;
    auto pre_wire_length = parsedData.wirelength;
    auto pre_wire_length_2 = parsedData.wirelength_2;
    // vector<int> wire_length_move = {};
    auto tmp_data = parsedData;
    auto best =parsedData;
    double P = 0.98;
    double ratio = 0.85;
    int uphill = 0;
    bool SA_flag = true;
    int MT = 0;
    int k = 7;
    int N = k*parsedData.num_softModules;
    auto T = -(parsedData.delta_avg/(log(P)));
    int reject = 0;
    std::cout << "N: " <<N << ", T: " << T <<std::endl;
    while ((std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start)).count()<=590 && ((MT ==0 ? 1:(reject/MT <= 0.95)) || T > 0))
    {
        uphill = 0;
        MT = 0;
        reject = 0;
        while ((uphill <= N || MT <= 2*N)&& (std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start)).count()<=590)
        {
            // pre_wire_length = parsedData.wirelength;
            bool success = perturbation(parsedData);
            MT++;
            if(success)
            {
               
                countWirelength(parsedData);
                int threshold = exp(-(parsedData.wirelength-pre_wire_length)/T);
                // int threshold = exp(-((parsedData.wirelength-pre_wire_length)+(parsedData.wirelength_2-pre_wire_length_2))/T);
                // int threshold = exp(-(parsedData.wirelength-best_wire_lenngth)/T);
                auto random_num = ((double) rand() / (RAND_MAX));
                if ((parsedData.wirelength < pre_wire_length)||random_num<threshold)
                // if ((parsedData.wirelength-pre_wire_length)+(parsedData.wirelength_2-pre_wire_length_2)||random_num<threshold)
                // if ((parsedData.wirelength < best_wire_lenngth)||random_num<threshold)
                {
                    if((parsedData.wirelength < best_wire_lenngth)) 
                    {
                        best_wire_lenngth = parsedData.wirelength;
                        best = parsedData;
                    }
                    if((parsedData.wirelength-pre_wire_length)>0)
                    // if((parsedData.wirelength-best_wire_lenngth)>0)
                    {
                        uphill++;
                    }
                    tmp_data = parsedData;
                    // pre_wire_length = parsedData.wirelength;
                }else if((parsedData.wirelength-best_wire_lenngth)<0)
                {
                    best_wire_lenngth = parsedData.wirelength;
                    best = parsedData;
                }else{
                    parsedData = tmp_data;
                    reject++;
                }
            }else{
                parsedData = tmp_data;
            }
        }
        T = ratio*T;
    }
    std::cout << parsedData.wirelength << std::endl;
    std::cout << best_wire_lenngth << std::endl;
    parsedData = best;
    countWirelength(parsedData);
    std::cout << parsedData.wirelength << std::endl;

}