//
// Created by Rémi on 21/08/2024.
//
#include <iostream>
#include "../../include/sim/fluid2DCpu.h"

#include "../../thirdparty/glfw/src/internal.h"

fluid2DCpu::fluid2DCpu(const int width, const int height, const float fluid_density){
    this->width = width;
    this->height = height;
    this->fluid_density = fluid_density;
    this->grid_spacing = 1.f / static_cast<float>(height);
    const int gridSize = width * height;
    velocity = new GLfloat[gridSize * 2]();
    pressure = new GLfloat[gridSize]();
    pressure_color = new GLfloat[gridSize]();
    is_border = new GLfloat[gridSize]();
    for (int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = i + jw;
            if (i == 0 || j == 0 || i == width - 1 || j == height - 1) {
                is_border[index] = 0.0;
            }
            else {
                is_border[index] = 1.0;
            }
        }
    }
    const int jmin = height / 2 - 2;
    const int jmax = jmin + 4;

    for(int j = jmin; j < jmax; j ++) {
        velocity[(j * width + 1) * 2 ] = 2;
    }
}
fluid2DCpu::~fluid2DCpu() {
    delete[] velocity;
    delete[] pressure;
    delete[] is_border;
    delete[] pressure_color;
}

void fluid2DCpu::compute_gravity(const float time_step) const {
    for (int j = 1; j < height; j ++) {
        const int jw = j * width;
        const int jwm = (j - 1) * width;
        for(int i = 0; i < width; i ++) {
            const int ij = i + jw;
            const int ijm = i + jwm;
            if (is_border[ij] == 0 || is_border[ijm] == 0) continue;
            velocity[ij * 2 + 1] += 9.81f * time_step;
        }
    }
}

void fluid2DCpu::projection(const int sub_step, const float time_step, const float o = 1.9) const {
    for(int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            const int ij = jw + i;
            pressure[ij] = 0.0;
        }
    }
    for (int n = 0; n < sub_step; n ++) {
        for (int j = 0; j < height ; j ++) {
            const int jw = j * width;
            const int jwp = (j + 1) * width;
            const int jwm = (j - 1) * width;
            for(int i = 0; i < width; i ++) {
                const int ij = i + jw;
                if (is_border[ij] == 0) continue;
                const int ipj = ij + 1;
                const int ijp = i + jwp;
                const int imj = ij - 1;
                const int ijm = i + jwm;
                GLfloat Uij = velocity[ij * 2]; //velocity[i][j].x
                GLfloat Vij = velocity[ij * 2 + 1]; // velocity[i][j].y
                GLfloat Uipj = velocity[ipj * 2]; //velocity[i + 1][j].x
                GLfloat Vijp = velocity[ijp * 2 + 1]; //velocity[i][j + 1].y
                const float d = o * (- Uij - Vij + Uipj + Vijp);
                const float Sipj = is_border[ipj];
                const float Simj = is_border[imj];
                const float Sijp = is_border[ijp];
                const float Sijm = is_border[ijm];
                const float s = Sipj + Simj + Sijp + Sijm;
                Uij += d * Simj / s;
                Uipj += -d * Sipj / s;
                Vij += d * Sijm / s;
                Vijp += -d * Sijp / s;
                velocity[ij * 2] = Uij;
                velocity[ij * 2 + 1] = Vij;
                velocity[ipj * 2] = Uipj;
                velocity[ijp * 2 + 1] = Vijp;

                float p = pressure[ij];
                p += (d / s) * (fluid_density * grid_spacing / time_step);
                pressure[ij] = p;
            }
        }
    }
}



void fluid2DCpu::advection(float time_step) {

}


void fluid2DCpu::calculate_pressure_color() const {
    const float p_min = find_min_pressure();
    const float p_max = find_max_pressure();
    const float delta = p_max - p_min;

    std::cout<<"[DEBUG] : min : "<<p_min<<", max :"<<p_max<<", delta : "<<delta<<std::endl;
    for(int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            int ij = jw + i;
            float p = pressure[ij];
            if (delta == 0)
                p = 0.5;
            else
                p = (p - p_min) / delta;
            pressure_color[ij] = p;
        }
    }
}

void fluid2DCpu::print_pressure() const {
    std::string r = "Pressure : \n\n";
    for(int j = 0; j < height; j ++) {
        for(int i = 0; i < width; i ++) {
            r += std::to_string(pressure[j * width + i ]) +", ";
        }
        r += "\n";
    }
    std::cout<<r<<std::endl;
}


GLfloat fluid2DCpu::find_max_pressure() const {
    GLfloat max = 0;
    for (int j = 1; j < height - 1; j++) {
        const int jw = j * width;
        for (int i = 1; i < width - 1; i++) {
            const int index = (i + jw);
            if (pressure[index] > max) max = pressure[index];
        }
    }
    return max;
}
GLfloat fluid2DCpu::find_min_pressure() const {
    GLfloat min = 1;
    for (int j = 1; j < height - 1; j++) {
        const int jw = j * width;
        for (int i = 1; i < width - 1; i++) {
            const int index = (i + jw);
            if (pressure[index] < min) min = pressure[index];
        }
    }
    return min;
}






