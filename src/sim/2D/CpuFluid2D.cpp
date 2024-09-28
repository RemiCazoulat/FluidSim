 //
// Created by remi.cazoulat on 30/08/2024.
//

#include "../../../include/sim/2D/CpuFluid2D.h"
#define SWAP(x0, x) {float* tmp = x0; (x0) = x; x = tmp;}


CpuFluid2D::CpuFluid2D(GLFWwindow* window, SimData* simData)
: Fluid2D(window, simData)
{

    const int gridSize = width * height;
    grid = new float[gridSize]();
    u_permanent = new float[gridSize]();
    v_perm = new float[gridSize]();
    u = new float[gridSize]();
    v = new float[gridSize]();
    u_prev = new float[gridSize]();
    v_prev = new float[gridSize]();
    dens = new float[gridSize]();
    dens_prev = new float[gridSize]();
    dens_perm = new float[gridSize]();
    pressure = new float[gridSize]();
    color = new float[gridSize * 4]();

    constexpr float r = 10.f;
    const int circle_x = width / 2;
    const int circle_y = height / 2;
    for (int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = i + jw;
            if (i == 0 || j == 0 || i == width - 1 || j == height - 1) grid[index] = 0.0;
            else grid[index] = 1.0;
            const int dist_x = circle_x - i;
            const int dist_y = circle_y - j;
            if(std::sqrt(dist_x * dist_x + dist_y * dist_y) < r) grid[index] = 0.0;
        }
    }
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    printf("Finishing CpuFluid2D constructor. \n");
        }

CpuFluid2D::~CpuFluid2D() {
    delete[] grid;
    delete[] u;
    delete[] v;
    delete[] u_permanent;
    delete[] v_perm;
    delete[] u_prev;
    delete[] v_prev;
    delete[] dens;
    delete[] dens_prev;
    delete[] dens_perm;
    delete[] pressure;
    delete[] color;
}

void CpuFluid2D::add_source(float* x, const float* s, const float dt) const {
    for (int i = 0; i < width * height; i++ ) x[i] += dt * s[i];
}

void CpuFluid2D::diffuse(float* x, const float* x0, const float diffusion_rate, const float dt) const {
    const float a = dt * diffusion_rate * static_cast<float>(width) * static_cast<float>(height);
    for (int k = 0 ; k < simData->sub_step ; k++ ) {
        for ( int j = 1 ; j < height - 1; j++ ) {
            const int jw = j * width;
            const int jw0 = (j - 1) * width;
            const int jw1 = (j + 1) * width;
            for (int i = 1 ; i < width - 1; i++ ) {
                if(grid[i + jw] == 0.f) continue;
                const int i0 = i - 1;
                const int i1 = i + 1;
                const float s0n = grid[i0 + jw];
                const float s1n = grid[i1 + jw];
                const float sn0 = grid[i + jw0];
                const float sn1 = grid[i + jw1];
                const float s = s0n + s1n + sn0 + sn1;
                const float d = x[i0 + jw] * s0n + x[i1 + jw] * s1n + x[i + jw0] * sn0 + x[i + jw1] * sn1;
                x[i + jw] = (x0[i + jw] + a * d) / (1 + s * a);
            }
        }
    }
}

void CpuFluid2D::advect(float * z, const float * z0, const float * u_vel, const float * v_vel, const float dt) const {
    const float dt0w = dt * static_cast<float>(width);
    const float dt0h = dt * static_cast<float>(height);
    for (int j = 1; j < height - 1; j++ ) {
        const int jw = j * width;
        for (int i = 1; i < width - 1; i++ ) {
            if(grid[i + jw] == 0.f) continue;

            const float delta_x = dt0w * u_vel[i + jw];
            const float delta_y = dt0h * v_vel[i + jw];

            float x = static_cast<float>(i) - delta_x;
            float y = static_cast<float>(j) - delta_y;

            if (x < 0.5) x = 0.5;
            if (x > static_cast<float>(width) - 1.5) x = static_cast<float>(width) - 1.5f;
            const int i0 = static_cast<int>(x);
            const int i1 = i0 + 1;

            if (y < 0.5) y = 0.5;
            if (y > static_cast<float>(height) - 1.5) y = static_cast<float>(height) - 1.5f;
            const int j0 = static_cast<int>(y);
            const int j1 = j0 + 1;

            const float s1 = x - static_cast<float>(i0);
            const float s0 = 1 - s1;
            const float t1 = y - static_cast<float>(j0);
            const float t0 = 1 - t1;
            const float new_z = s0 * (t0 * z0[i0 + j0 * width] + t1 * z0[i0 + j1 * width]) +
                                s1 * (t0 * z0[i1 + j0 * width] + t1 * z0[i1 + j1 * width]);
            z[i + jw] = new_z;
        }
    }
}

void CpuFluid2D::project(float * p, float * div) const {
    const float h =  simData->h_w;
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        const int j1w = (j + 1) * width;
        const int j0w = (j - 1) * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            p[i + jw] = 0;
            if(grid[i + jw] == 0.f) continue;
            const int i0 = i - 1;
            const int i1 = i + 1;
            const float d = u[i1 + jw] - u[i0 + jw] + v[i + j1w] - v[i + j0w];
            div[i + jw] = -0.5f * h * d;
        }
    }
    for (int k = 0; k <  simData->sub_step ;k++ ) {
        for (int j = 1 ; j < height - 1 ; j++ ) {
            const int jw = j * width;
            const int j1w = (j + 1) * width;
            const int j0w = (j - 1) * width;
            for (int i = 1 ; i < width - 1 ; i++ ) {
                if(grid[i + jw] == 0.f) continue;
                const int i0 = i - 1;
                const int i1 = i + 1;
                const float s0x = grid[i0 + jw];
                const float s1x = grid[i1 + jw];
                const float s0y = grid[i + j0w];
                const float s1y = grid[i + j1w];
                const float s = s0x + s1x + s0y + s1y;
                p[i + jw] = (
                    div[i + jw] +
                    p[i0 + jw] * s0x +
                    p[i1 + jw] * s1x +
                    p[i + j0w] * s0y +
                    p[i + j1w] * s1y  ) / s;
            }
        }
    }
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        const int j1w = (j + 1) * width;
        const int j0w = (j - 1) * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            if(grid[i + jw] == 0.f) continue;
            const int i0 = i - 1;
            const int i1 = i + 1;
            const float s0x = grid[i0 + jw];
            const float s1x = grid[i1 + jw];
            const float s0y = grid[i + j0w];
            const float s1y = grid[i + j1w];
            u[i + jw] -= (p[i1 + jw] * s1x - p [i0 + jw] * s0x) / (h * 2);
            v[i + jw] -= (p[i + j1w] * s1y - p [i + j0w] * s0y) / (h * 2);
        }
    }
}

void CpuFluid2D::pressure_step(const float dt) {
    for(int j = 1; j < height - 1; j++) {
        const int jw = j * width;
        const int j0w = (j - 1) * width;
        const int j1w = (j + 1) * width;
        for(int i = 1; i < width - 1; i++) {
            const int i0 = i - 1;
            const int i1 = i + 1;
            const float d = u[i1 + jw] - u[i0 + jw] + v[i + j1w] - v[i + j0w];
            pressure[ i + jw] = d / 4 * dens[i + jw] *  simData->h_w / dt;
        }
    }
}

void CpuFluid2D::density_step(const float dt) {
    add_source(dens, dens_prev, dt);
    SWAP(dens_prev, dens) diffuse(dens, dens_prev,  simData->diffusion, dt);
    SWAP(dens_prev, dens) advect(dens, dens_prev, u, v, dt);
}

void CpuFluid2D::velocity_step(const float dt) {
    add_source (u, u_prev, dt);
    add_source (v, v_prev, dt);
    SWAP(u_prev, u) diffuse (u, u_prev,  simData->viscosity, dt);
    SWAP(v_prev, v) diffuse (v, v_prev,  simData->viscosity, dt);
    project (u_prev, v_prev);
    set_vel_bound();
    SWAP( u_prev, u)
    SWAP( v_prev, v)
    advect (u, u_prev, u_prev, v_prev, dt);
    advect (v, v_prev, u_prev, v_prev, dt);
    set_vel_bound();
    project (u_prev, v_prev);
    set_vel_bound();
}

void CpuFluid2D::set_vel_bound() const {
    for(int j = 1; j < height - 1; j ++) {
        const int jw = j * width;
        const int j0w = (j - 1) * width;
        const int j1w = (j + 1) * width;
        for(int i = 1; i < width - 1; i ++) {
            if(grid[i + jw] == 0.f) continue;
            const int i0 = i - 1;
            const int i1 = i + 1;
            float x = u[i + jw];
            float y = v[i + jw];
            const float s0x = grid[i0 + jw];
            const float s1x = grid[i1 + jw];
            const float s0y = grid[i + j0w];
            const float s1y = grid[i + j1w];
            if(x > 0.f && s1x == 0.f || x < 0.f && s0x == 0.f) x = -x;
            if(y > 0.f && s1y == 0.f || y < 0.f && s0y == 0.f) y = -y;
            u[i + jw] = x;
            v[i + jw] = y;

        }
    }
}

void CpuFluid2D::add(const int x, const int y, float* t, const float intensity) const {
    if(grid[x + y * width] == 0.f) return;
    t[x + y * width] += intensity;
}

void CpuFluid2D::input_step(const float dt) {
    if (left_mouse_pressed || right_mouse_pressed || middle_mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / simData->cell_size;
        const int j = static_cast<int>((static_cast<float>(simData->cell_size * height) - mouse_y)) / simData->cell_size;

        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            if(left_mouse_pressed ) {
                int r_int = 0;
                int r_float = 0.f;
                if(simData->smoke) r_int = (int)(simData->smoke_radius);
                if(simData->obstacles) r_int = (int)(simData->obstacles_radius);
                if(simData->velocity) r_int = (int)(simData->vel_radius);

                if(simData->smoke) r_float = simData->smoke_radius;
                if(simData->obstacles) r_float = simData->obstacles_radius;
                if(simData->velocity) r_float = simData->vel_radius;
                for(int x = -r_int; x <= r_int; x++) {
                    for(int y = -r_int; y <= r_int; y++) {
                        if (i + x >= 1 && i + x < width - 1 && j + y >= 1 && j + y < height - 1) {
                            if (std::sqrt(static_cast<float>(x * x + y * y)) < static_cast<float>(r_float)) {
                                if(simData->smoke) {
                                    if(simData->smoke_add) {
                                        add(i + x, j + y, dens,simData->smoke_intensity);
                                    }
                                    if(simData->smoke_perm) {
                                        add(i + x, j + y, dens_perm, simData->smoke_intensity);
                                    }
                                    if(simData->smoke_remove) {
                                        //TODO: make remove smoke
                                    }
                                }
                                if(simData->obstacles) {
                                   //TODO: make obstacles
                                }
                                if(simData->velocity) {
                                    if(simData->vel_add) {
                                        add(i + x, j + y, u,simData->vel_intensity[0]);
                                        add(i + x, j + y, v,simData->vel_intensity[1]);
                                    }
                                    if(simData->vel_perm) {
                                        add(i + x, j + y, v_perm, simData->vel_intensity[0]);
                                        add(i + x, j + y, v_perm, simData->vel_intensity[1]);
                                    }
                                    if(simData->vel_remove) {
                                        // TODO: make vel remove
                                    }

                                }
                            }
                        }
                    }
                }
                force_x = mouse_x;
                force_y = mouse_y;
            }
        }
    }
    add_source(dens, dens_perm, dt);
    add_source(u, u_permanent, dt);
    add_source(v, v_perm, dt);
}

static void xy2hsv2rgb(const float x, const float y, float &r, float &g, float &b, const float r_max) {
    const float h = std::atan2(y, x) * 180 / static_cast<float>(3.14159265358979323846) + 180;
    constexpr float s = 1.f;
    const float v = std::sqrt(x * x + y * y) / r_max;
    const int segment = static_cast<int>(h / 60) % 6;
    const float f = h / 60 - static_cast<float>(segment);
    const float p = v * (1 - s);
    const float q = v * (1 - s * f);
    const float t = v * (1 - s * (1 - f));
    switch (segment) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default:r = 1, g = 1, b = 1;
    }
}

static void getSciColor(float val, const float min, const float max, float &r, float &g, float &b) {
    val = static_cast<float>(
        std::min(
            std::max(
                static_cast<double>(val),
                static_cast<double>(min)),
            max - 0.0001));

    const float d = max - min;
    val = d == 0.0f ? 0.5f : (val - min) / d;
    constexpr float m = 0.25f;
    const int num = std::floor(val / m);
    const float s = (val - static_cast<float>(num) * m) / m;
    switch (num) {
        case 0 : r = 0.0; g = s       ; b = 1.0     ; break;
        case 1 : r = 0.0; g = 1.0     ; b = 1.0f - s; break;
        case 2 : r = s  ; g = 1.0     ; b = 0.0     ; break;
        case 3 : r = 1.0; g = 1.0f - s; b = 0.0     ; break;
        default: r = 0.0; g = 0.0     ; b = 0.0     ; break;
    }
}

GLuint CpuFluid2D::draw_step(const DRAW_MODE mode) {
    const float max_u = find_max(u);
    const float max_v = find_max(v);
    const float r_max = std::sqrt(max_u * max_u + max_v * max_v);
    const float max_d = find_max(dens);
    const float min_d = find_min(dens);
    const float delta_d = max_d - min_d;

    float r, g, b;

    for (int j = 0 ; j < height ; j++ ) {
        const int jw = j * width;
        for (int i = 0 ; i < width ; i++ ) {
            const int ij = i + jw;
            if(mode == VELOCITY) {
                const float x = u[ij];
                const float y = v[ij];
                xy2hsv2rgb(x, y, r, g, b, r_max);
            }
            if(mode == DENSITY) {
                float x = dens[ij];
                x = (x + delta_d - max_d) / delta_d;
                r = x; g = x; b = x;
            }
            if (mode == PRESSURE) {
                const float max_p = find_max(pressure);
                const float min_p = find_min(pressure);
                getSciColor(pressure[ij], min_p, max_p, r, g, b);
            }
            color[ij * 4 + 0] = r;
            color[ij * 4 + 1] = g;
            color[ij * 4 + 2] = b;
            color[ij * 4 + 3] = 1.0;
        }
    }
    const GLuint colorTex = createTextureVec4(color, width, height);
    renderer->rendering(colorTex);
    return colorTex;
}

 void CpuFluid2D::debug() {
 }

 float CpuFluid2D::find_max(const float* x) const {
    float max = x[0];
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            if(const int ij = i + jw; x[ij] > max) max = x[ij];
        }
    }
    return max;
}

float CpuFluid2D::find_min(const float* x) const {
    float min = x[0];
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            if(const int ij = i + jw; x[ij] < min) min = x[ij];
        }
    }
    return min;
}

