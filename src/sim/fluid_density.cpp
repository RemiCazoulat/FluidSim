//
// Created by remi.cazoulat on 26/08/2024.
//

#include "../../include/sim/fluid_density.h"

#define SWAP(x0,x) {float* tmp = x0; x0 = x; x = tmp;}


fluid_density::fluid_density(const int width,const int height, const float diff, const float visc) {
    this->width = width;
    this->height = height;
    this->grid_spacing = 1.f / static_cast<float>(height);
    this->diff = diff;
    this->visc = visc;
    const int gridSize = width * height;
    is_b = new GLfloat[gridSize]();
    u = new GLfloat[gridSize]();
    v = new GLfloat[gridSize]();
    u_prev = new GLfloat[gridSize]();
    v_prev = new GLfloat[gridSize]();
    dens = new GLfloat[gridSize]();
    dens_prev = new GLfloat[gridSize]();
    color = new GLfloat[gridSize]();
    for (int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = i + jw;
            if (i == 0 || j == 0 || i == width - 1 || j == height - 1) {
                is_b[index] = 0.0;
            }
            else {
                is_b[index] = 1.0;
            }
        }
    }
}

fluid_density::~fluid_density() {
    delete[] is_b;
    delete[] u;
    delete[] v;
    delete[] u_prev;
    delete[] v_prev;
    delete[] dens;
    delete[] dens_prev;
    delete[] color;
}

void fluid_density::add_source(GLfloat * x, const GLfloat* s,const float dt) const {
    for (int i = 0; i < width * height; i++ ) x[i] += dt*s[i];
}

void fluid_density::diffuse(const int b, GLfloat* x, GLfloat* x0, const float diff, const float dt) const {
    const float a = dt * diff * static_cast<float>(width) * static_cast<float>(height);
    for (int k = 0 ; k < 20 ; k++ ) {
        for ( int j = 1 ; j < height - 1; j++ ) {
            const int jw = j * width;
            const int j0w = (j - 1) * width;
            const int j1w = (j + 1) * width;
            for (int i = 1 ; i < width - 1; i++ ) {
                const int ij = jw + i;
                const int i0j = jw + i - 1;
                const int i1j = jw + i + 1;
                const int ij0 = j0w + i;
                const int ij1 = j1w + i;
                x0[ij] = x[ij] + a * (x0[i0j] + x0[i1j] + x0[ij0] + x0[ij1]) / ( 1 + 4 * a);
                //x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/(1+4*a);
            }
        }
        set_bound(b, x);
    }
}

void fluid_density::advect(const int b, GLfloat * z, const GLfloat * z0, const GLfloat * u, const GLfloat * v, const float dt) const {
    const float dt0w = dt * static_cast<float>(width);
    const float dt0h = dt * static_cast<float>(height);
    for (int j = 1; j < height - 1; j++ ) {
        const int jw = j * width;
        for (int i = 1; i<= width - 1; i++ ) {
            float y = static_cast<float>(j) - dt0h * v[i + jw];
            float x = static_cast<float>(i) - dt0w * u[i + jw];

            if (x < 0.5) x = 0.5;
            if (x > width - 1 + 0.5) x = 0.5f + static_cast<float>(width);
            const int i0 = static_cast<int>(x);
            const int i1 = i0 + 1;

            if (y < 0.5) y = 0.5;
            if (y > height - 1 + 0.5) y = 0.5f + static_cast<float>(height);
            const int j0 = static_cast<int>(y);
            const int j1 = j0 + 1;

            const float s1 = x - static_cast<float>(i0);
            const float s0 = 1 - s1;
            const float t1 = y - static_cast<float>(j0);
            const float t0 = 1 - t1;
            z[i + jw] =
               s0 * (t0 * z0[i0 + j0 * width] + t1 * z0[i0 + j1 * width]) +
               s1 * (t0 * z0[i1 + j0 * width] + t1 * z0[i1 + j1 * width]);
            /*
            d[i + jw] =
                s0 * (t0 * d0[IX(i0,j0)] + t1 * d0[IX(i0,j1)]) +
                s1 * (t0 * d0[IX(i1,j0)] + t1 * d0[IX(i1,j1)]);
            */
        }
    }
    set_bound(b, z);
}


void fluid_density::project(GLfloat * u, GLfloat * v, GLfloat * p, GLfloat * div) const {
    const float h = 1.0f / static_cast<float>(height);
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        const int j1w = (j + 1) * width;
        const int j0w = (j - 1) * width;
        for (int i = 1 ; i <= width - 1 ; i++ ) {
            div[i + jw] = -0.5f * h * (u[i + 1 + jw] - u[i - 1 + jw] + v[i + j1w] - v[i + j0w]);
            p[i + jw] = 0;
            /*
            div[IX(i,j)] = -0.5*h*(u[IX(i+1,j)]-u[IX(i-1,j)]+
            v[IX(i,j+1)]-v[IX(i,j-1)]);
            p[IX(i,j)] = 0;
            */
        }
    }
    set_bound (0, div);
    set_bound (0, p);
    for (int k = 0; k < 20 ;k++ ) {
        for (int j = 1 ; j < height - 1 ; j++ ) {
            const int jw = j * width;
            const int j1w = (j + 1) * width;
            const int j0w = (j - 1) * width;
            for (int i = 1 ; i <= width - 1 ; i++ ) {

                //p[IX(i,j)] = (div[IX(i,j)]+p[IX(i-1,j)]+p[IX(i+1,j)]+ p[IX(i,j-1)]+p[IX(i,j+1)])/4;
            }
        }
        set_bound (0, p );
    }
    for (int j = 1 ; j < height - 1 ; j++ ) {
        for (int i = 1 ; i <= width - 1 ; i++ ) {
            u[IX(i,j)] -= 0.5*(p[IX(i+1,j)]-p[IX(i-1,j)])/h;
            v[IX(i,j)] -= 0.5*(p[IX(i,j+1)]-p[IX(i,j-1)])/h;
        }
    }
    set_bound (1, u );
    set_bound (2, v );
}

void fluid_density::density_step(const float dt) {
    // x : dens
    // x0 : dens_prev
    add_source(dens, dens_prev, dt);
    SWAP(dens_prev, dens); diffuse(0, dens, dens_prev, diff, dt);
    SWAP(dens_prev, dens); advect( 0, dens, dens_prev, u, v, dt);

}

void fluid_density::velocity_step(const float dt) {
    // u : u
    // u0 : u_prev
    // v : v
    // v0 : v_prev
    add_source (u, u_prev, dt); add_source ( v, v_prev, dt);
    SWAP (u_prev, u); diffuse (1, u, u_prev, visc, dt);
    SWAP (v_prev, v); diffuse (2, v, v_prev, visc, dt);

    project (u, v, u_prev, v_prev);
    SWAP ( u_prev, u ); SWAP ( v_prev, v);
    advect (1, u, u_prev, u_prev, v_prev, dt ); advect (2, v, v_prev, u_prev, v_prev, dt );
    project (u, v, u_prev, v_prev);
}

void fluid_density::set_color() const {
}

GLfloat fluid_density::find_max_dens() const {
    return 0;
}

GLfloat fluid_density::find_min_dens() const {
    return 0;
}

void fluid_density::set_bound(const int b, GLfloat * x) const {
    constexpr int i0 = 0;
    constexpr int i1 = 1;
    const int iN1 = width - 1;
    const int iN2 = width - 2;
    const int j0 = 0 * width;
    const int j1 = 1 * width;
    const int jN1 = (height - 1) * width;
    const int jN2 = (height - 2) * width;

    const int bound = width > height ? width : height;
    for (int i = 1 ; i < bound - 1; i++ ) {
        if(i < width) {
            const int ii = i;
            x[ii + j0 ] = b == 2 ? - x[ii + j1 ] : x[ii + j1 ];
            x[ii + jN1] = b == 2 ? - x[ii + jN2] : x[ii + jN2];
            // x[IX(i,0 )] = b==2 ? –x[IX(i,1)] : x[IX(i,1)];
            // x[IX(i,N+1)] = b==2 ? –x[IX(i,N)] : x[IX(i,N)];
        }
        if(i < height) {
            const int ji = i * width;
            x[i0 + ji ] = b == 1 ? - x[i1 + ji ] : x[i1 + ji ];
            x[iN1 + j1] = b == 1 ? - x[iN2 + ji] : x[iN2 + ji];
            //pressure[IX(0 ,i)] = b==1 ? –x[IX(1,i)] : x[IX(1,i)];
            //pressure[IX(N+1,i)] = b==1 ? –x[IX(N,i)] : x[IX(N,i)];
        }
    }
    x[i0  + i0 ] = 0.5f * (x[i1  + j0 ] + x[i0  + j1 ]);
    x[i0  + jN1] = 0.5f * (x[i1  + jN1] + x[i0  + jN2]);
    x[iN1 + j0 ] = 0.5f * (x[iN2 + j0 ] + x[iN1 + j1 ]);
    x[iN1 + jN1] = 0.5f * (x[iN2 + jN1] + x[iN1 + jN2]);
    /*
    x[IX(0 ,0 )] = 0.5*(x[IX(1,0 )]+x[IX(0 ,1)]);
    x[IX(0 ,N+1)] = 0.5*(x[IX(1,N+1)]+x[IX(0 ,N )]);
    x[IX(N+1,0 )] = 0.5*(x[IX(N,0 )]+x[IX(N+1,1)]);
    x[IX(N+1,N+1)] = 0.5*(x[IX(N,N+1)]+x[IX(N+1,N )]);
    */
}
