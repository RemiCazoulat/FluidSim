//
// Created by Rémi on 21/08/2024.
//

include "../../fluid2DCpu.h"

fluid2DCpu::fluid2DCpu(const float width, const float height){
    this.width = width;
    this.height = height;

    const int gridSize = width * height;
    const int gridSizex2 = gridSize * 2;
    this.vel = new GLfloat[gridSizex2]();
    this.dens = new GLfloat[gridSize]();
    this.is_border = new GLfloat[gridSize]();
    for (j = 0; j < this.height; j ++) {
        for(i = 0; i < this.width; i ++) {
            if (i == 0 || j == 0 || i == this.width - 1 || j == this.width - 1) {
                this.is_border = 0.0;
            }
            else {
                this.is_border = 1.0;
            }
        }
    }
}
fluid2DCpu::~fluid2DCpu() {
    delete[] this.vel;
    delete[] this.dens;
    delete[] this.is_border;
}

fluid2DCpu::projection(const int subStep, const float o) {

    for (n = 0; n < subStep; n ++) {
        for (j = 1; j < this.height - 1; j ++) {
            jw = j * this.width;
            jw1 = (j + 1) * this.width;
            for(i = 1; i < this.width - 1; i ++) {
                uv = i + jw;
                u1v = uv + 1;
                uv1 = i + jw1;
                Uuv = this.vel[uv * 2];
                Vuv = this.vel[uv * 2 + 1];
                Uu1v = this.vel[u1v * 2];
                Vuv1 = this.vel[uv1 * 2 + 1];
                s = o * ()
            }
        }
    }
}


