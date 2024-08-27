//
// Created by remi.cazoulat on 26/08/2024.
//
#include "../../include/sim/stable_fluid.h"



#define SWAP(x0,x) {float* tmp = x0; x0 = x; x = tmp;}

stable_fluid::stable_fluid(const int width,const int height, const int cell_size, const float diff, const float visc) {
    this->width = width;
    this->height = height;
    this->cell_size = cell_size;
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
    color = new GLfloat[gridSize * 3]();
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

stable_fluid::~stable_fluid() {
    delete[] is_b;
    delete[] u;
    delete[] v;
    delete[] u_prev;
    delete[] v_prev;
    delete[] dens;
    delete[] dens_prev;
    delete[] color;
}

void stable_fluid::add_source(GLfloat * x, const GLfloat* s,const float dt) {
    for (int i = 0; i < width * height; i++ ) x[i] += dt * s[i];
}

void stable_fluid::diffuse(const int b, GLfloat* x, const GLfloat* x0, const float diff, const float dt) {
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
                x[ij] = x0[ij] + a * (x[i0j] + x[i1j] + x[ij0] + x[ij1]) / ( 1 + 4 * a);
                //x[IX(i,j)] = (x0[IX(i,j)] + a*(x[IX(i-1,j)]+x[IX(i+1,j)]+x[IX(i,j-1)]+x[IX(i,j+1)]))/(1+4*a);
            }
        }
        set_bound(b, x);
    }
}

void stable_fluid::advect(const int b, GLfloat * z, const GLfloat * z0, const GLfloat * u, const GLfloat * v, const float dt) {
    const float dt0w = dt * static_cast<float>(width);
    const float dt0h = dt * static_cast<float>(height);
    for (int j = 1; j < height - 1; j++ ) {
        const int jw = j * width;
        for (int i = 1; i < width - 1; i++ ) {
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


void stable_fluid::project(GLfloat * u, GLfloat * v, GLfloat * p, GLfloat * div) {
    const float h = 1.0f / static_cast<float>(height);
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        const int j1w = (j + 1) * width;
        const int j0w = (j - 1) * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            div[i + jw] = -0.5f * h * (u[i + 1 + jw] - u[i - 1 + jw] + v[i + j1w] - v[i + j0w]);
            p[i + jw] = 0;
            //div[IX(i,j)] = -0.5*h*(u[IX(i+1,j)]-u[IX(i-1,j)]+v[IX(i,j+1)]-v[IX(i,j-1)]);
            //p[IX(i,j)] = 0;
        }
    }
    set_bound (0, div);
    set_bound (0, p);
    for (int k = 0; k < 20 ;k++ ) {
        for (int j = 1 ; j < height - 1 ; j++ ) {
            const int jw = j * width;
            const int j1w = (j + 1) * width;
            const int j0w = (j - 1) * width;
            for (int i = 1 ; i < width - 1 ; i++ ) {
                p[i + jw] = (
                    div[i + jw] +
                    p[i - 1 + jw] +
                    p[i + 1 + jw] +
                    p[i + j0w] +
                    p[i + j1w]) / 4;
                //p[IX(i,j)] = (div[IX(i,j)]+p[IX(i-1,j)]+p[IX(i+1,j)]+ p[IX(i,j-1)]+p[IX(i,j+1)])/4;
            }
        }
        set_bound (0, p );
    }
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        const int j1w = (j + 1) * width;
        const int j0w = (j - 1) * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            u[i + jw] -= 0.5f * (p[i + 1 + jw] - p [i - 1 + jw]) / h;
            v[i + jw] -= 0.5f * (p[i + j1w] - p [i + j0w]) / h;
            //u[IX(i,j)] -= 0.5*(p[IX(i+1,j)]-p[IX(i-1,j)])/h;
            //v[IX(i,j)] -= 0.5*(p[IX(i,j+1)]-p[IX(i,j-1)])/h;
        }
    }
    set_bound (1, u );
    set_bound (2, v );
}
void stable_fluid::watch_inputs(const int mouse_pressed,const float mouse_x,const float mouse_y, float &force_x, float &force_y) {

    //u_prev[width / 2 + height / 2 * width ] = 100.0f;

    if (mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / cell_size;  // Suppose que la fenêtre est de 800x800
        const int j = static_cast<int>((static_cast<float>(cell_size * height) - mouse_y)) / cell_size;  // Inversion de l'axe Y

        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            // Ajoute de la densité à la position de la souris
            dens_prev[i + j * width] += 1.0f;

            // Ajoute une force dans les directions X et Y (par exemple, un simple mouvement de la souris)
            u_prev[i + j * width] += (mouse_x - force_x) * 0.1f;  // La force dépend du déplacement de la souris
            v_prev[i + j * width] += (mouse_y - force_y) * 0.1f;

            // Met à jour les forces pour la prochaine itération
            force_x = mouse_x;
            force_y = mouse_y;
        }
    }
}

void stable_fluid::density_step(const float dt) {
    // x : dens
    // x0 : dens_prev
    add_source(dens, dens_prev, dt);
    SWAP(dens_prev, dens); diffuse(0, dens, dens_prev, diff, dt);
    SWAP(dens_prev, dens); advect( 0, dens, dens_prev, u, v, dt);

}

void stable_fluid::velocity_step(const float dt) {
    // u : u
    // u0 : u_prev
    // v : v
    // v0 : v_prev
    add_source (u, u_prev, dt);
    add_source ( v, v_prev, dt);
    SWAP (u_prev, u); diffuse (1, u, u_prev, visc, dt);
    SWAP (v_prev, v); diffuse (2, v, v_prev, visc, dt);
    project (u, v, u_prev, v_prev);
    SWAP ( u_prev, u );
    SWAP ( v_prev, v);
    advect (1, u, u_prev, u_prev, v_prev, dt );
    advect (2, v, v_prev, u_prev, v_prev, dt );
    project (u, v, u_prev, v_prev);
}

void stable_fluid::draw(const DRAW_MODE mode=VELOCITY) const {
    if(mode == VELOCITY) {
        const float max_u = find_max(u);
        const float max_v = find_max(v);
        const float r_max = std::sqrt(max_u * max_u + max_v * max_v);
        for (int j = 1 ; j < height - 1 ; j++ ) {
            const int jw = j * width;
            for (int i = 1 ; i < width - 1 ; i++ ) {
                const int ij = i + jw;
                const float x = u[ij];
                const float y = v[ij];
                const float     h = std::atan2(y, x) * 180 / static_cast<float>(3.14159265358979323846);
                constexpr float s = 1.f;
                const float     v = std::sqrt(x * x + y * y) / r_max;
                const int segment = static_cast<int>(h / 60) % 6;  // Determine dans quel segment H tombe
                const float f = h / 60 - static_cast<float>(segment);  // Facteur fractionnaire de H
                const float p = v * (1 - s);
                const float q = v * (1 - s * f);
                const float t = v * (1 - s * (1 - f));
                float r, g, b;
                switch (segment) {
                    case 0: r = v; g = t; b = p; break;
                    case 1: r = q; g = v; b = p; break;
                    case 2: r = p; g = v; b = t; break;
                    case 3: r = p; g = q; b = v; break;
                    case 4: r = t; g = p; b = v; break;
                    case 5: r = v; g = p; b = q; break;
                    default: r = 0; g = 0; b = 0;
                }
                color[ij * 3 + 0] = r;
                color[ij * 3 + 1] = g;
                color[ij * 3 + 2] = b;
            }
        }
    }
    if(mode == DENSITY) {
        const float max = find_max(dens);
        const float min = find_min(dens);
        const float delta = max - min;
        for (int j = 1 ; j < height - 1 ; j++ ) {
            const int jw = j * width;
            for (int i = 1 ; i < width - 1 ; i++ ) {
                const int ij = i + jw;
                float x = dens[ij];
                x = (x + delta - max) / delta;
                color[ij * 3 + 0] = x;
                color[ij * 3 + 1] = x;
                color[ij * 3 + 2] = x;
            }
        }
    }
}

GLfloat stable_fluid::find_max(const GLfloat* x) const {
    GLfloat max = x[0];
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            if(const int ij = i + jw; x[ij] > max) max = x[ij];
        }
    }
    return max;
}

GLfloat stable_fluid::find_min(const GLfloat* x) const {
    GLfloat min = x[0];
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            if(const int ij = i + jw; x[ij] < min) min = x[ij];
        }
    }
    return min;
}

void stable_fluid::set_bound(const int b, GLfloat * x) {
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
        }
        if(i < height) {
            const int ji = i * width;
            x[i0 + ji ] = b == 1 ? - x[i1 + ji ] : x[i1 + ji ];
            x[iN1 + j1] = b == 1 ? - x[iN2 + ji] : x[iN2 + ji];
        }
    }
    x[i0  + i0 ] = 0.5f * (x[i1  + j0 ] + x[i0  + j1 ]);
    x[i0  + jN1] = 0.5f * (x[i1  + jN1] + x[i0  + jN2]);
    x[iN1 + j0 ] = 0.5f * (x[iN2 + j0 ] + x[iN1 + j1 ]);
    x[iN1 + jN1] = 0.5f * (x[iN2 + jN1] + x[iN1 + jN2]);
}
