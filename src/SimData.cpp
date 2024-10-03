//
// Created by Rémi on 29/09/2024.
//
#include "../include/simData.h"

void SimData::change_res(int new_res) {
    resolution = new_res;
    real_res = (int)(std::pow(2, resolution));
    width = gen_width * real_res;
    height = gen_height * real_res;
    cell_size = gen_cell_size / real_res;
}

void SimData::change_sim_mode(SIM_MODE new_sim_mode) {

}

void SimData::change_sim_dim(SIM_DIM new_sim_dim) {

}
