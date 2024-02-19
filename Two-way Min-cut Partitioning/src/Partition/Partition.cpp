#include "Partition.h"
#include <iostream>
#include <ctime>
#include <set>
#include <random>
#include <tuple>
#include <chrono>

void FMalgo::calculateCutSize(ParsedData &parsedData) {
    int cutSize = 0;
    for (auto &net : parsedData.nets_vector) {
        if(net.numCellInGroup[0] != 0 && net.numCellInGroup[1] != 0)
            cutSize++;
    }
    parsedData.cutSize = cutSize;
}
void InitialPartition::compute_gain(ParsedData &parsedData){
    for (auto& net : parsedData.nets_vector){
        net.die_cell_num(parsedData, net);
    }
    for (auto& cell : parsedData.cells_vector){
        cell.lock = false;
        cell.gain = 0;
        for(auto& net : cell.netlist)
        {

            if (parsedData.nets_vector[net].numCellInGroup[cell.die_id] == 1){
                ++cell.gain;
            }
            if (parsedData.nets_vector[net].numCellInGroup[!cell.die_id] == 0){
                --cell.gain;
            }
        }
    }
}
void InitialPartition::createInitialPartition(ParsedData &parsedData) {
    
    long long dieWidth = parsedData.dieWidth, dieHeigh = parsedData.dieHeight;
    long long dieAUtil = dieWidth*dieHeigh*(static_cast<double>(parsedData.dieAUtilization)/100);
    long long dieBUtil = dieWidth*dieHeigh*(static_cast<double>(parsedData.dieBUtilization)/100);
    parsedData.dieAarea = dieAUtil;
    parsedData.dieBarea = dieBUtil;
    long long cellWidth_A,cellHeight_A,newArea_A = 0;
    long long cellWidth_B,cellHeight_B,newArea_B = 0;
    parsedData.dies[0].initBucklist(0, parsedData);
    parsedData.dies[1].initBucklist(1, parsedData);
    // std::srand(std::time(nullptr));
    long long currentArea_A = parsedData.dies[0].current_area;
    long long currentArea_B = parsedData.dies[1].current_area;
    bool reinit_flag = false;


    
    // for cells_vector
    for (auto &cell : parsedData.cells_vector){
        // auto cell_name = std::stoi(cell.libCellName.substr(2))-1;
        auto cell_name = cell.lib_id;
        
        cellWidth_A = parsedData.techs_vector[parsedData.dies[0].tech_id].libCells_vector[cell_name].width;
        cellHeight_A = parsedData.techs_vector[parsedData.dies[0].tech_id].libCells_vector[cell_name].height;
        newArea_A = cellWidth_A * cellHeight_A;
        cellWidth_B = parsedData.techs_vector[parsedData.dies[1].tech_id].libCells_vector[cell_name].width;
        cellHeight_B = parsedData.techs_vector[parsedData.dies[1].tech_id].libCells_vector[cell_name].height;
        newArea_B = cellWidth_B * cellHeight_B;
        if (newArea_A+currentArea_A <= dieAUtil*0.95)
        {
            currentArea_A += newArea_A;
            cell.die_id = 0;
        }else
        {
            if (newArea_B+currentArea_B <= dieBUtil*0.95)
            {
                currentArea_B += newArea_B;
                cell.die_id = 1;
                
            }else if(newArea_A+currentArea_A <= dieAUtil)
            {
                currentArea_A += newArea_A;
                cell.die_id = 0;
            }else if(newArea_B+currentArea_B <= dieBUtil)
            {
                currentArea_B += newArea_B;
                cell.die_id = 1;
            }else
            {
                std::cout << "no space: " << cell.name << std::endl;
                reinit_flag = true;
                break;
            }
        }
    }
    if(reinit_flag)
    {
        for (auto &cell : parsedData.cells_vector){
            auto cell_name = cell.lib_id;
            cellWidth_A = parsedData.techs_vector[parsedData.dies[0].tech_id].libCells_vector[cell_name].width;
            cellHeight_A = parsedData.techs_vector[parsedData.dies[0].tech_id].libCells_vector[cell_name].height;
            newArea_A = cellWidth_A * cellHeight_A;
            cellWidth_B = parsedData.techs_vector[parsedData.dies[1].tech_id].libCells_vector[cell_name].width;
            cellHeight_B = parsedData.techs_vector[parsedData.dies[1].tech_id].libCells_vector[cell_name].height;
            newArea_B = cellWidth_B * cellHeight_B;
            if (newArea_B+currentArea_B <= dieBUtil*0.95)
            {
                // std::cout << newArea_A << " " << cell_name << std::endl;
                currentArea_B += newArea_B;
                cell.die_id = 1;
            }else
            {
                if (newArea_A+currentArea_A <= dieAUtil*0.95)
                {
                    // std::cout << newArea_B << " " << cell_name << std::endl;
                    currentArea_A += newArea_A;
                    cell.die_id = 0;
                    
                }else if(newArea_B+currentArea_B <= dieBUtil)
                {
                    currentArea_B += newArea_B;
                    cell.die_id = 1;
                }else if(newArea_A+currentArea_A <= dieAUtil)
                {
                    currentArea_A += newArea_A;
                    cell.die_id = 0;
                }else
                {
                    std::cout << "no space: " << cell.name << std::endl;
                    reinit_flag = false;
                    break;
                }
            }
        }
    }
    parsedData.dies[0].current_area = currentArea_A;
    parsedData.dies[1].current_area = currentArea_B;
    parsedData.dies[0].spare_area = dieAUtil - currentArea_A;
    parsedData.dies[1].spare_area = dieBUtil - currentArea_B;
    // std::cout << "total area: " << parsedData.dieAarea << " " << parsedData.dieBarea << std::endl;
    // std::cout << parsedData.dies[0].current_area << " " << parsedData.dies[1].current_area << std::endl;
    // std::cout << parsedData.dies[0].spare_area << " " << parsedData.dies[1].spare_area << std::endl;
    compute_gain(parsedData);
    for (auto &cell : parsedData.cells_vector){
        // std::cout << cell.die_id << " " << cell.name << " " << cell.gain << " " << parsedData.pmax <<std::endl;
        parsedData.dies[cell.die_id].insertNode(&cell);
    }   
}
std::tuple<int, int, Node *> FMalgo::getBaseCell(ParsedData &data){

    //vector bucket
    int pMax = data.pmax;
    auto spare_a = data.dies[0].spare_area, spare_b = data.dies[1].spare_area;
    for(int i = pMax; i >= -pMax; i--)
    {
        auto die_a_bucket = static_cast<Cell *>(data.dies[0].bucketList_vector[i+pMax].next);
        auto die_b_bucket = static_cast<Cell *>(data.dies[1].bucketList_vector[i+pMax].next);
        bool die_a_bucket_flag = false, die_b_bucket_flag = false;
        if (die_a_bucket)
        {
            // std::cout << "cell: " <<  die_a_bucket->name << std::endl;
            auto lib_name = (static_cast<Cell *>(die_a_bucket)->lib_id);
            auto target_cell_area = data.techs_vector[data.dies[1].tech_id].libCells_vector[lib_name].width* data.techs_vector[data.dies[1].tech_id].libCells_vector[lib_name].height;
            die_a_bucket_flag = spare_b <= target_cell_area;
            while (die_a_bucket->lock && die_a_bucket->next && die_a_bucket_flag )
            {   
                die_a_bucket = static_cast<Cell *>(die_a_bucket->next);
            }
        }
        if (die_b_bucket)
        {
            auto lib_name = (static_cast<Cell *>(die_b_bucket)->lib_id);
            auto target_cell_area = data.techs_vector[data.dies[0].tech_id].libCells_vector[lib_name].width* data.techs_vector[data.dies[0].tech_id].libCells_vector[lib_name].height;
            die_b_bucket_flag = spare_a <= target_cell_area;
            while (die_b_bucket->lock && die_b_bucket_flag && die_b_bucket->next)
            {   
                die_b_bucket = static_cast<Cell *>(die_b_bucket->next);
            }

        }
        if (die_a_bucket || die_b_bucket)
        {
            if(die_a_bucket && !die_b_bucket) 
            {
                if(!die_a_bucket->lock && !die_a_bucket_flag) return std::make_tuple(0, die_a_bucket->id, die_a_bucket);
            }
            else if (die_b_bucket && !die_a_bucket) 
            {
                if(!die_b_bucket->lock && !die_b_bucket_flag) return std::make_tuple(1, die_b_bucket->id, die_b_bucket);
            }
            else 
            {
                // std::cout << "cell: " <<  die_a_bucket->name << std::endl;
                // std::cout << "hi_3" << std::endl;
                if(!(spare_b >= spare_a  ? die_a_bucket->lock:die_b_bucket->lock) && !(spare_b >= spare_a ? die_a_bucket_flag:die_b_bucket_flag)) return (spare_b >= spare_a ? std::make_tuple(0, die_a_bucket->id, die_a_bucket):std::make_tuple(1, die_b_bucket->id, die_b_bucket));
            }
        }
    }
    
    return std::make_tuple(-1, -1, nullptr);
}
void FMalgo::update_gain(ParsedData &data, Cell base_cell)
{
    for (auto& net:base_cell.netlist)
    {
        auto net_name = net;
        auto num_cell_target = data.nets_vector[net_name].numCellInGroup[base_cell.die_id];
        data.nets_vector[net_name].numCellInGroup[(base_cell.die_id+1)%2]-=1;
        data.nets_vector[net_name].numCellInGroup[base_cell.die_id]+=1;
        auto num_cell_after =data.nets_vector[net_name].numCellInGroup[(base_cell.die_id+1)%2];
        
        for(auto& cellname: data.nets_vector[net_name].cell_list)
        {
            
            
            auto &cell = data.cells_vector[cellname];
            auto temp = cell.gain;
            if(cell.lock == 0 && num_cell_target == 0)
            {
                
                ++cell.gain;
            } else if (cell.lock==0 && num_cell_target == 1)
            {
                
                if(cell.die_id == base_cell.die_id)
                {
                    --cell.gain;
                }
            }  
            
            if(cell.lock == 0 && num_cell_after == 0)
            {
                --cell.gain;
            } else if (cell.lock==0 && num_cell_after == 1)
            {
                if(cell.die_id == (base_cell.die_id+1)%2)
                {
                    ++cell.gain;
                }
            }  
            if (temp != cell.gain)
            {
                data.dies[cell.die_id].removeNode(&cell);
                data.dies[cell.die_id].insertNode(&cell);
            }
        }
    }
}
void FMalgo::fm_init(ParsedData &data)
{
    long long dieWidth = data.dieWidth, dieHeigh = data.dieHeight;
    long long dieAUtil = dieWidth*dieHeigh*(static_cast<double>(data.dieAUtilization)/100);
    long long dieBUtil = dieWidth*dieHeigh*(static_cast<double>(data.dieBUtilization)/100);
    data.dieAarea = dieAUtil;
    data.dieBarea = dieBUtil;
    long long cellWidth_A,cellHeight_A,newArea_A = 0;
    long long cellWidth_B,cellHeight_B,newArea_B = 0;
    data.dies[0].bucketList_vector.clear();
    data.dies[1].bucketList_vector.clear();
    data.dies[0].initBucklist(0, data);
    data.dies[1].initBucklist(1, data);
    // std::srand(std::time(nullptr));
    long long currentArea_A = data.dies[0].current_area;
    currentArea_A = 0;
    long long currentArea_B = data.dies[1].current_area;
    currentArea_B = 0;
    
    // for cells_vector
    for(auto& cell_name: data.min_cut_diea_idx)
    {
        
        auto lib_name = data.cells_vector[cell_name].lib_id;
        cellWidth_A = data.techs_vector[data.dies[0].tech_id].libCells_vector[lib_name].width;
        cellHeight_A = data.techs_vector[data.dies[0].tech_id].libCells_vector[lib_name].height;
        newArea_A = cellWidth_A * cellHeight_A;
        if (newArea_A+currentArea_A <= dieAUtil)
        {
            currentArea_A += newArea_A;
            data.cells_vector[cell_name].die_id = 0;
        }else{
            std::cout << "no space in: " << data.cells_vector[cell_name].name << std::endl;
        }
    }
    for(auto& cell_name: data.min_cut_dieb_idx)
    {
        auto lib_name = data.cells_vector[cell_name].lib_id;
        cellWidth_B = data.techs_vector[data.dies[1].tech_id].libCells_vector[lib_name].width;
        cellHeight_B = data.techs_vector[data.dies[1].tech_id].libCells_vector[lib_name].height;
        newArea_B = cellWidth_B * cellHeight_B;
        if (newArea_B+currentArea_B <= dieBUtil)
        {
            currentArea_B += newArea_B;
            data.cells_vector[cell_name].die_id = 1;
        }else{
            std::cout << "no space in: " << data.cells_vector[cell_name].name << std::endl;
        }
    }
    data.dies[0].current_area = currentArea_A;
    data.dies[1].current_area = currentArea_B;
    data.dies[0].spare_area = dieAUtil - currentArea_A;
    data.dies[1].spare_area = dieBUtil - currentArea_B;
    InitialPartition init;
    init.compute_gain(data);
    for (auto &cell : data.cells_vector){
        data.dies[cell.die_id].insertNode(&cell);
    }   
}
extern std::chrono::_V2::system_clock::time_point start;
void FMalgo::fm(ParsedData &data){
    data.mincut = data.cutSize;
    data.temp_cutsize = data.cutSize;
    data.loop_mincut = data.cutSize;
    int count = 0, j = 0;
    while (1)
    {
        data.cutSize = data.loop_mincut;
        data.gain_change = 0;
        for(auto i = 0; i < static_cast<int>(data.cells_vector.size()); i ++)
        {
            // std::cout << "start get" << std::endl;
            auto basecell_info = getBaseCell(data);
            
            // std::cout << std::get<0>(basecell_info) << " " << std::get<1>(basecell_info) << " " << std::get<2>(basecell_info) << std::endl;
            if(std::get<0>(basecell_info) != -1)
            {
                
                // auto basecell_die = std::get<0>(basecell_info);
                auto basecell_name = std::get<1>(basecell_info);
                auto basecell_idx = basecell_name;
                auto current_cell = data.cells_vector[basecell_idx];
                auto current_cell_lib = current_cell.lib_id;
               
                auto current_die = current_cell.die_id;
                // std::cout << "current cell: "<< current_cell.name << " " <<current_cell.die_id << std::endl;
                auto current_cell_area = data.techs_vector[data.dies[current_die].tech_id].libCells_vector[current_cell_lib].width * data.techs_vector[data.dies[current_die].tech_id].libCells_vector[current_cell_lib].height;
                auto target_cell_area = data.techs_vector[data.dies[(current_die+1)%2].tech_id].libCells_vector[current_cell_lib].width * data.techs_vector[data.dies[(current_die+1)%2].tech_id].libCells_vector[current_cell_lib].height;
                auto target_spare_area = (current_die == 0 ? data.dies[1].spare_area : data.dies[0].spare_area);
                // std::cout << current_cell_area << " " << target_cell_area << " " << target_spare_area << std::endl;
                // std::cout<<i<<std::endl;
                // std::cout<<"switch "<<  i << " " << target_spare_area << " " << target_cell_area<<std::endl;
                if (data.cells_vector[basecell_idx].lock){
                    std::cout << " lock " << std::endl;
                }
                if (target_spare_area >= target_cell_area)
                {
                    data.dies[current_die].removeNode(&data.cells_vector[basecell_idx]);
                    
                    data.dies[current_die].spare_area += current_cell_area;
                    data.dies[(current_die+1)%2].spare_area -= target_cell_area;
                    data.cells_vector[basecell_idx].die_id = (current_die+1)%2;
                    data.cells_vector[basecell_idx].lock = true;
                    data.num_of_lock += 1;
                    // std::cout << " gain " << data.cells_vector[basecell_idx].gain << std::endl;
                    data.gain_change = data.cells_vector[basecell_idx].gain;
                    data.cutSize = data.cutSize-data.gain_change;
                    
                    update_gain(data, data.cells_vector[basecell_idx]);
                    if(data.cells_vector[basecell_idx].gain <= 0) count += 1;
                    else if (data.cells_vector[basecell_idx].gain > 0) count = 0;
                    
                    
                }
            }else if((std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start)).count()>=290)
            {
                break;
            }else
            {
                std::cout << "break " << std::endl;
                break;
            }
            if (data.mincut>=data.cutSize)
            {
                
                data.mincut = data.cutSize;
                // std::cout<<"update cut size: " << data.cutSize << " " << data.mincut<< std::endl;
                data.min_cut_diea.clear();
                data.min_cut_dieb.clear();
                data.min_cut_diea_idx.clear();
                data.min_cut_dieb_idx.clear();
                for(auto& cell :data.cells_vector){
                    // std::cout << "cell info: " << cell.second.name << " " << cell.second.die_id << " " << cell.second.gain << std::endl;
                    if(cell.die_id == 0)
                    {
                        data.min_cut_diea.push_back(cell.name);
                        data.min_cut_diea_idx.push_back(cell.id);
                    }
                    else
                    {
                        data.min_cut_dieb.push_back(cell.name);
                        data.min_cut_dieb_idx.push_back(cell.id);
                    }
                }
            }
            
            if((std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start)).count()>=290)
            {
                break;
            }
            
        }
        std::cout << "finished iter: " << j << " " << data.mincut << std::endl;
        j++;
        std::cout << (std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start)).count() << std::endl;
        if(data.loop_mincut > data.mincut)
        {
            data.loop_mincut = data.mincut;
            // data.cutSize = data.mincut;
            data.loop_min_cut_diea.clear();
            data.loop_min_cut_dieb.clear();
            data.loop_min_cut_diea_idx.clear();
            data.loop_min_cut_dieb_idx.clear();
            for(auto& cell_die_a: data.min_cut_diea)
            {
                data.loop_min_cut_diea.push_back(cell_die_a);
            }
            for(auto& cell_die_a: data.min_cut_diea_idx)
            {
                data.loop_min_cut_diea_idx.push_back(cell_die_a);
            }
            for(auto& cell_die_b: data.min_cut_dieb)
            {
                data.loop_min_cut_dieb.push_back(cell_die_b);
            }
            for(auto& cell_die_b: data.min_cut_dieb_idx)
            {
                data.loop_min_cut_dieb_idx.push_back(cell_die_b);
            }
        }else{
            // fm_init(data);
            break;
        }
        if((std::chrono::duration<double>(std::chrono::high_resolution_clock::now()-start)).count()>=290)
        {    // fm_init(data);
            break;
        }
        for(auto &cell: data.cells_vector)
        {
            cell.lock = false;
        }
        fm_init(data);
        
    }
}
