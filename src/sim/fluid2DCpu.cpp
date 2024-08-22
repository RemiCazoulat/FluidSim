//
// Created by Rémi on 21/08/2024.
//

#include "../../include/sim/fluid2DCpu.h"

fluid2DCpu::fluid2DCpu(const int width, const int height){
    this->width = width;
    this->height = height;

    const int gridSize = width * height;
    const int gridSizex2 = gridSize * 2;
    this->vel = new GLfloat[gridSizex2]();
    this->dens = new GLfloat[gridSize]();
    this->is_border = new GLfloat[gridSize]();
    for (int j = 0; j < this->height; j ++) {
        int jw = j * this->width;
        for(int i = 0; i < this->width; i ++) {
            int index = i + jw;
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
    delete[] this->vel;
    delete[] this->dens;
    delete[] this->is_border;
}

void fluid2DCpu::projection(const int subStep, const float o) const {
    for (int n = 0; n < subStep; n ++) {
        for (int j = 1; j < this->height - 1; j ++) {
            const int jw = j * this->width;
            const int jw1 = (j + 1) * this->width;
            for(int i = 1; i < this->width - 1; i ++) {
                const int uv = i + jw;
                const int u1v = uv + 1;
                const int uv1 = i + jw1;
                GLfloat Uuv = this->vel[uv * 2];
                GLfloat Vuv = this->vel[uv * 2 + 1];
                GLfloat Uu1v = this->vel[u1v * 2];
                GLfloat Vuv1 = this->vel[uv1 * 2 + 1];
                // s = o * ()
            }
        }
    }
}


