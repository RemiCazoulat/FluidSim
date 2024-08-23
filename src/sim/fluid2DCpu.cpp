//
// Created by Rémi on 21/08/2024.
//

#include "../../include/sim/fluid2DCpu.h"

fluid2DCpu::fluid2DCpu(const int width, const int height, const int pixelsPerCell, const float fluidDensity){
    this->width = width;
    this->height = height;
    this->pixelsPerCell = pixelsPerCell;
    this->fluid_density = fluidDensity;
    const int gridSize = width * height;
    const int gridSizex2 = gridSize * 2;
    this->velocity = new GLfloat[gridSizex2]();
    this->pressure = new GLfloat[gridSize]();
    this->is_border = new GLfloat[gridSize]();
    for (int j = 0; j < this->height; j ++) {
        const int jw = j * this->width;
        for(int i = 0; i < this->width; i ++) {
            const int index = i + jw;
            if (i == 0 || j == 0 || i == this->width - 1 || j == this->width - 1) {
                this->is_border[index] = 0.0;
            }
            else {
                this->is_border[index] = 1.0;
            }
        }
    }
}
fluid2DCpu::~fluid2DCpu() {
    delete[] this->velocity;
    delete[] this->pressure;
    delete[] this->is_border;
}

void fluid2DCpu::compute_gravity(const float timeStep) const {
    for (int j = 1; j < this->height - 1; j ++) {
        const int jw = j * this->width;
        for(int i = 1; i < this->width - 1; i ++) {
            const int index = (i + jw) * 2 + 1;
            this->velocity[index] += 9.81f * timeStep;
        }
    }
}

void fluid2DCpu::projection(const int subStep, const float timeStep, const float o = 1.9) const {
    for(int j = 0; j < this->height; j ++) {
        const int jw = j * this->width;
        for(int i = 0; i < this->width; i ++) {
            const int index = jw + i;
            this->pressure[index] = 0.0;
        }
    }
    for (int n = 0; n < subStep; n ++) {
        for (int j = 1; j < this->height - 1; j ++) {
            const int jw = j * this->width;
            const int jwp = (j + 1) * this->width;
            const int jwm = (j - 1) * this->width;
            for(int i = 1; i < this->width - 1; i ++) {
                const int ij = i + jw;
                const int ipj = ij + 1;
                const int ijp = i + jwp;
                const int imj = ij - 1;
                const int ijm = i + jwm;
                GLfloat Uij = this->velocity[ij * 2];
                GLfloat Vij = this->velocity[ij * 2 + 1];
                GLfloat Uipj = this->velocity[ipj * 2];
                GLfloat Vijp = this->velocity[ijp * 2 + 1];
                const float d = o * (- Uij - Vij + Uipj + Vijp);
                const float Sipj = this->is_border[ipj];
                const float Simj = this->is_border[imj];
                const float Sijp = this->is_border[ijp];
                const float Sijm = this->is_border[ijm];
                const float s = Sipj + Simj + Sijp + Sijm;
                Uij += d * Simj / s;
                Uipj += -d * Sipj / s;
                Vij += d * Sijm / s;
                Vijp += -d * Sijp / s;
                this->velocity[ij * 2] = Uij;
                this->velocity[ij * 2 + 1] = Uipj;
                this->velocity[ipj * 2] = Vij;
                this->velocity[ijp * 2 + 1] = Vijp;

                float p = this->pressure[ij];
                p += d / s * this->fluid_density / timeStep;
                this->pressure[ij] = p;
            }
        }
    }
}

void fluid2DCpu::advection(float timeStep) {

}






