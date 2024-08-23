//
// Created by Rémi on 21/08/2024.
//
#include <iostream>
#include "../../include/sim/fluid2DCpu.h"

fluid2DCpu::fluid2DCpu(const int width, const int height, const int pixelsPerCell, const float fluidDensity){
    this->width = width;
    this->height = height;
    this->pixelsPerCell = pixelsPerCell;
    this->fluid_density = fluidDensity;
    const int gridSize = width * height;
    const int gridSize2 = gridSize * 2;
    velocity = new GLfloat[gridSize2]();
    pressure = new GLfloat[gridSize]();
    is_border = new GLfloat[gridSize]();
    for (int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = i + jw;
            if (i == 0 || j == 0 || i == width - 1 || j == width - 1) {
                is_border[index] = 0.0;
            }
            else {
                is_border[index] = 1.0;
            }
        }
    }
}
fluid2DCpu::~fluid2DCpu() {
    delete[] velocity;
    delete[] pressure;
    delete[] is_border;
}

void fluid2DCpu::compute_gravity(const float timeStep) const {
    for (int j = 1; j < height - 1; j ++) {
        const int jw = j * width;
        for(int i = 1; i < width - 1; i ++) {
            const int index = (i + jw) * 2 + 1;
            velocity[index] += 9.81f * timeStep;
        }
    }
}

GLfloat fluid2DCpu::find_max_pressure() const {
    GLfloat max = pressure[0];
    for (int j = 1; j < height - 1; j++) {
        const int jw = j * width;
        for (int i = 1; i < width - 1; i++) {
            const int index = (i + jw) * 2 + 1;
            if (pressure[index] > max) max = pressure[index];
        }
    }
    std::cout<<"max in pressure : "<<max<<std::endl;
    return max;
}
GLfloat fluid2DCpu::find_min_pressure() const {
    GLfloat min = pressure[0];
    for (int j = 1; j < height - 1; j++) {
        const int jw = j * width;
        for (int i = 1; i < width - 1; i++) {
            const int index = (i + jw) * 2 + 1;
            if (pressure[index] < min) min = pressure[index];
        }
    }
    std::cout<<"min in pressure : "<<min<<std::endl;
    return min;
}

void fluid2DCpu::projection(const int subStep, const float timeStep, const float o = 1.9) const {
    for(int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = jw + i;
            pressure[index] = 0.0;
        }
    }
    for (int n = 0; n < subStep; n ++) {
        for (int j = 1; j < height - 1; j ++) {
            const int jw = j * width;
            const int jwp = (j + 1) * width;
            const int jwm = (j - 1) * width;
            for(int i = 1; i < width - 1; i ++) {
                const int ij = i + jw;
                const int ipj = ij + 1;
                const int ijp = i + jwp;
                const int imj = ij - 1;
                const int ijm = i + jwm;
                GLfloat Uij = velocity[ij * 2];
                GLfloat Vij = velocity[ij * 2 + 1];
                GLfloat Uipj = velocity[ipj * 2];
                GLfloat Vijp = velocity[ijp * 2 + 1];
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
                p += d / s * fluid_density / timeStep;
                pressure[ij] = p;
            }
        }
    }
}

void fluid2DCpu::advection(float timeStep) {

}






