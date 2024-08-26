//
// Created by remi.cazoulat on 26/08/2024.
//

#include "../../include/sim/fluid_density.h"


fluid_density::fluid_density(const int width,const int height, float density) {
    this->width = width;
    this->height = height;
    this->density = density;
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
}

fluid_density::~fluid_density() {
    delete[] velocity;
    delete[] pressure;
    delete[] is_border;
    delete[] pressure_color;
}

void fluid_density::compute_gravity(float time_step) const {
}

void fluid_density::diffusion(const int sub_step, const float dt, const float diff) const {
    float a = dt * diff * static_cast<float>(width) * static_cast<float>(height);

    auto pres_t = new GLfloat[width * height]();
    for (int k = 0 ; k < sub_step ; k++ ) {
        for ( int j = 1 ; j < height - 1; j++ ) {
            const int jw = j * width;
            const int j0w = (j - 1) * width;
            const int j1w = (j + 1) * width;
            for (int i = 1 ; i < width - 1; i++ ) {
                // IX(i,j) = i + N * j
                const int ij = jw + i;
                const int i0j = jw + i - 1;
                const int i1j = jw + i + 1;
                const int ij0 = j0w + i;
                const int ij1 = j1w + i;
                pres_t[ij] = pressure[ij] + a * (pres_t[i0j] + pres_t[i1j] + pres_t[ij0] + pres_t[ij1]) / ( 1 + 4 * a);

            }
        }
    }
    delete[] pres_t;
}

void fluid_density::advection(float time_step) {
}

void fluid_density::clearing_divergence() {
}

void fluid_density::calculate_pressure_color() const {
}

GLfloat fluid_density::find_max_pressure() const {
}

GLfloat fluid_density::find_min_pressure() const {
}

void fluid_density::set_bnd(int b) {
    int bound = width > height ? width : height;
    for (int i = 1 ; i < bound - 1; i++ ) {
        if(i < width) {
            int i0 = i;
            int iw = i + (height - 1) * width;
            pressure[i0] = b==2 ? –pressure[IX(i,1)] : pressure[IX(i,1)];
            pressure[iw] = b==2 ? –pressure[IX(i,N)] : pressure[IX(i,N)];
        }
        if(i < height) {
            pressure[IX(0 ,i)] = b==1 ? –x[IX(1,i)] : x[IX(1,i)];
            pressure[IX(N+1,i)] = b==1 ? –x[IX(N,i)] : x[IX(N,i)];
        }



    }
    pressure[IX(0 ,0 )] = 0.5*(x[IX(1,0 )]+x[IX(0 ,1)]);
    pressure[IX(0 ,N+1)] = 0.5*(x[IX(1,N+1)]+x[IX(0 ,N )]);
    pressure[IX(N+1,0 )] = 0.5*(x[IX(N,0 )]+x[IX(N+1,1)]);
    pressure[IX(N+1,N+1)] = 0.5*(x[IX(N,N+1)]+x[IX(N+1,N )]);
}
