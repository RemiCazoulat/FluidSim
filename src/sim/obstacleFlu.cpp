 //
// Created by remi.cazoulat on 30/08/2024.
//
// TODO : modifier project pour que ca colle avec le tableau is_b.
//  appeler correctement is_vel_bound.
//  On y est presque !

#include "../../include/sim/obstacleFlu.h"

#define SWAP(x0, x) {float* tmp = x0; x0 = x; x = tmp;}

static float force_x = 0.0f, force_y = 0.0f, mouse_x = 0.0f, mouse_y = 0.0f;
static int left_mouse_pressed = 0, right_mouse_pressed = 0, middle_mouse_pressed = 0;

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        left_mouse_pressed = 1;
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        left_mouse_pressed = 0;
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        right_mouse_pressed = 1;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        right_mouse_pressed = 0;
    }
    if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        middle_mouse_pressed = 1;
    } else if(button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
        middle_mouse_pressed = 0;
    }
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    mouse_x = static_cast<float>(xpos);
    mouse_y = static_cast<float>(ypos);
}


obstacleFlu::obstacleFlu(GLFWwindow* window, const int width, const int height, const int cell_size, const float diff, const float visc, const int sub_step) {
    this->window = window;
    this->width = width;
    this->height = height;
    this->cell_size = cell_size;
    this->grid_spacing = 1.f / static_cast<float>(height);
    this->diff = diff;
    this->visc = visc;
    this->sub_step = sub_step;
    const int gridSize = width * height;
    is_b = new float[gridSize]();
    u_permanent = new float[gridSize]();
    v_permanent = new float[gridSize]();
    u = new float[gridSize]();
    v = new float[gridSize]();
    u_prev = new float[gridSize]();
    v_prev = new float[gridSize]();
    dens = new float[gridSize]();
    dens_prev = new float[gridSize]();
    dens_permanent = new float[gridSize]();
    pressure = new float[gridSize]();
    color = new float[gridSize * 3]();

    for (int j = 0; j < height; j ++) {
        const int jw = j * width;
        for(int i = 0; i < width; i ++) {
            const int index = i + jw;
            if (i == 0 || j == 0 || i == width - 1 || j == height - 1) is_b[index] = 0.0;
            else is_b[index] = 1.0;
        }
    }
    /*
    for (int i = width / 4; i < width - 1 - width / 4; i++) {
        for(int j = -3; j < 3; j ++) {
            add_permanent_vel(i, height - height / 4 + j, 0.f, 5.0f);
        }
    }
    */
    //add_permanent_dens(width / 2, height / 2, 1.f);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

}

obstacleFlu::~obstacleFlu() {
    delete[] is_b;
    delete[] u;
    delete[] v;
    delete[] u_permanent;
    delete[] v_permanent;
    delete[] u_prev;
    delete[] v_prev;
    delete[] dens;
    delete[] dens_prev;
    delete[] dens_permanent;
    delete[] pressure;
    delete[] color;
}

void obstacleFlu::add_source(float* x, const float* s, const float dt) const {
    for (int i = 0; i < width * height; i++ ) x[i] += dt * s[i];
}

void obstacleFlu::diffuse(const int b, float* x, const float* x0, const float diff, const float dt) const {
    const float a = dt * diff * static_cast<float>(width) * static_cast<float>(height);
    for (int k = 0 ; k < sub_step ; k++ ) {
        for ( int j = 1 ; j < height - 1; j++ ) {
            const int jw = j * width;
            const int jw0 = (j - 1) * width;
            const int jw1 = (j + 1) * width;
            for (int i = 1 ; i < width - 1; i++ ) {
                if(is_b[i + jw] == 0.f) continue;
                const int i0 = i - 1;
                const int i1 = i + 1;
                const float s0n = is_b[i0 + jw];
                const float s1n = is_b[i1 + jw];
                const float sn0 = is_b[i + jw0];
                const float sn1 = is_b[i + jw1];
                const float s = s0n + s1n + sn0 + sn1;
                const float d = x[i0 + jw] * s0n + x[i1 + jw] * s1n + x[i + jw0] * sn0 + x[i + jw1] * sn1;
                x[i + jw] = (x0[i + jw] + a * d) / (1 + s * a);
            }
        }
    }
}

float avg_u(const int i,const int j, const float* u,const int width) {
    const int jw = j * width;
    const int j0w = (j - 1) * width;
    return (u[i + j0w] + u[i + jw] + u[i+1 + j0w] + u[i+1 + j]) * 0.25f;
}

float avg_v(const int i,const int j, const float* v,const int width) {
    const int jw = j * width;
    const int j1w = (j + 1) * width;
    return (v[i - 1 + jw] + v[i + jw] + v[i - 1 + j1w] + v[i + j1w]) * 0.25f;
}


float obstacleFlu::interpolate(float x, float y, const float* t, const float dx, const float dy) const {
    const float h = grid_spacing;
    const float h1 = 1.f / h;

    x = std::max(std::min(x, static_cast<float>(width) * h), h);
    y = std::max(std::min(y, static_cast<float>(height) * h), h);

    const float x0 = std::min(std::floor((x - dx) * h1), static_cast<float>(width) - 1.f);
    const float x1 = std::min(x0 + 1, static_cast<float>(width) - 1.f);
    const float tx = (x - dx - x0 * h) * h1;

    const float y0 = std::min(std::floor((y - dy) * h1), static_cast<float>(height) - 1.f);
    const float y1 = std::min(y0 + 1, static_cast<float>(height) - 1.f);
    const float ty = (y - dy - y0 * h) * h1;

    const float sx = 1.0f - tx;
    const float sy = 1.0f - ty;

    const auto widthf = static_cast<float>(width);
    const float val = sx * sy * t[static_cast<int>(x0 + y0 * widthf)] +
                tx * sy * t[static_cast<int>(x1 + y0 * widthf)] +
                sx * ty * t[static_cast<int>(x0 + y1 * widthf)] +
                tx * ty * t[static_cast<int>(x1 + y1 * widthf)];
    return val;
}



void obstacleFlu::advect_vel(const float dt) const {
    const float h = grid_spacing;
    const float h2 = 0.5f * h;
    for (int j = 1; j < height - 1; j++ ) {
        const int jw = j * width;
        const int j0w = (j - 1) * width;
        for (int i = 1; i < width - 1; i++ ) {
            // u
            if(is_b[i + jw] != 0.f && is_b[i - 1 + jw] != 0.f) {
                float x = static_cast<float>(i) * h;
                float y = static_cast<float>(j) * h + h2;
                const float u_f = u_prev[i + jw];
                const float v_f = avg_v(i, j, v_prev, width);
                x -= dt * u_f;
                y -= dt * v_f;
                const float u_val = interpolate(x, y, u, 0.f, h2);
                u[i + jw] = u_val;
            }
            // v
            if(is_b[i + jw] != 0.f && is_b[i + j0w] != 0.f) {
                float x = static_cast<float>(i) * h + h2;
                float y = static_cast<float>(j) * h;
                const float u_f = avg_u(i, j, u_prev, width);
                const float v_f = v[i + jw];
                x -= dt * u_f;
                y -= dt * v_f;
                const float v_val = interpolate(x, y, v_prev, h2, 0.f);
                v[i + jw] = v_val;
            }
        }
    }
}


void obstacleFlu::advect(const int b, float * z, const float * z0, const float * u_vel, const float * v_vel, const float dt) const {
    const float dt0w = dt * static_cast<float>(width);
    const float dt0h = dt * static_cast<float>(height);
    for (int j = 1; j < height - 1; j++ ) {
        const int jw = j * width;
        for (int i = 1; i < width - 1; i++ ) {
            const float delta_x = dt0w * u_vel[i + jw];
            const float delta_y = dt0h * v_vel[i + jw];

            float x = static_cast<float>(i) - delta_x;
            float y = static_cast<float>(j) - delta_y;

            if (x < 0.5) x = 0.5;
            if (x > static_cast<float>(width) - 1.5) x = static_cast<float>(width) - 1.5f;
            const int i0 = static_cast<int>(x);
            const int i1 = i0 + 1;

            if (y < 1.5) y = 1.5;
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
    set_bound(b, z);
}

void obstacleFlu::project_simple() const {
    for(int k = 0; k < sub_step; k ++) {
        for(int j = 1; j < height - 1; j ++) {
            const int jw = j * width;
            const int j1w = (j + 1) * width;
            const int j0w = (j - 1) * width;
            for(int i = 1; i < width - 1; i ++) {
                const int i0 = i - 1;
                const int i1 = i + 1;
                if(is_b[i + jw] == 0.f) continue;
                const float sx0 = is_b[i0 + jw];
                const float sx1 = is_b[i1 + jw];
                const float sy0 = is_b[i + j0w];
                const float sy1 = is_b[i + j1w];
                const float s = sx0 + sx1 + sy0 + sy1;
                if (s == 0.f) continue;
                const float divergence = u[i1 + jw] - u[i + jw] + v[i + j1w] - v[i + jw];
                const float p = -divergence / s * 1.9f;
                u[i + jw] -= sx0 * p;
                u[i1 + jw] += sx1 * p;
                v[i + jw] -= sy0 * p;
                v[i + j1w] += sy1 * p;
            }
        }
    }
}
void obstacleFlu::project(float * u, float * v, float * p, float * div) const {
    const float h = grid_spacing;
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        const int j1w = (j + 1) * width;
        const int j0w = (j - 1) * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            const int i0 = i - 1;
            const int i1 = i + 1;
            const float d = u[i1 + jw] - u[i0 + jw] + v[i + j1w] - v[i + j0w];
            div[i + jw] = -0.5f * h * d;
            p[i + jw] = 0;
        }
    }
    set_bound (0, div);
    set_bound (0, p);
    for (int k = 0; k < sub_step ;k++ ) {
        for (int j = 1 ; j < height - 1 ; j++ ) {
            const int jw = j * width;
            const int j1w = (j + 1) * width;
            const int j0w = (j - 1) * width;
            for (int i = 1 ; i < width - 1 ; i++ ) {
                const int i0 = i - 1;
                const int i1 = i + 1;
                p[i + jw] = (
                    div[i + jw] + p[i0 + jw]  + p[i1 + jw]  + p[i + j0w]  + p[i + j1w]) / 4;
            }
        }
        set_bound (0, p );
    }
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        const int j1w = (j + 1) * width;
        const int j0w = (j - 1) * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            const int i0 = i - 1;
            const int i1 = i + 1;
            u[i + jw] -= (p[i1 + jw] - p [i0 + jw]) / (h * 2);
            v[i + jw] -= (p[i + j1w] - p [i + j0w]) / (h * 2);
        }
    }
    set_bound (1, u );
    set_bound (2, v );
}

void obstacleFlu::calculate_pressure(const float dt) const {
    for(int j = 1; j < height - 1; j++) {
        const int jw = j * width;
        const int j0w = (j - 1) * width;
        const int j1w = (j + 1) * width;
        for(int i = 1; i < width - 1; i++) {
            const int i0 = i - 1;
            const int i1 = i + 1;
            const float d = u[i1 + jw] - u[i0 + jw] + v[i + j1w] - v[i + j0w];
            pressure[ i + jw] = d / 4 * dens[i + jw] * grid_spacing / dt;
        }
    }
}

void obstacleFlu::density_step(const float dt) {
    add_source(dens, dens_prev, dt);
    SWAP(dens_prev, dens); diffuse(0, dens, dens_prev, diff, dt);
    SWAP(dens_prev, dens); advect( 0, dens, dens_prev, u, v, dt);
}




void obstacleFlu::velocity_step(float dt) {
    add_source (u, u_prev, dt);
    add_source (v, v_prev, dt);
    SWAP(u_prev, u); diffuse (1, u, u_prev, visc, dt);
    SWAP(v_prev, v); diffuse (2, v, v_prev, visc, dt);
    project (u, v, u_prev, v_prev);
    //project_simple();
    SWAP( u_prev, u );
    SWAP( v_prev, v);
    advect (1, u, u_prev, u_prev, v_prev, dt );
    advect (2, v, v_prev, u_prev, v_prev, dt );
    project (u, v, u_prev, v_prev);
    //advect_vel(dt);
    //project_simple();
}

void obstacleFlu::set_vel_bound() const {
    for(int j = 1; j < height - 1; j ++) {
        const int jw = j * width;
        const int j0w = (j - 1) * width;
        const int j1w = (j + 1) * width;
        for(int i = 1; i < width - 1; i ++) {
            const int i0 = i - 1;
            const int i1 = i + 1;
            float x = u[i + jw];
            float y = v[i + jw];
            const float s0x = is_b[i0 + jw];
            const float s1x = is_b[i1 + jw];
            const float s0y = is_b[i + j0w];
            const float s1y = is_b[i + j1w];
            if(x > 0.f && s1x == 0.f || x < 0.f && s0x == 0.f) x = -x;
            if(y > 0.f && s1y == 0.f || y < 0.f && s0y == 0.f) y = -y;
            u[i + jw] = x;
            v[i + jw] = y;

        }
    }
}

void obstacleFlu::set_bound(const int b, float* x) const {
    constexpr int i0 = 0;
    constexpr int i1 = 1;
    const int iN1 = width - 1;
    const int iN2 = width - 2;
    const int j0 = 0 * width;
    const int j1 = 1 * width;
    const int jN1 = (height - 1) * width;
    const int jN2 = (height - 2) * width;

    // vertical borders (up and down)
    for (int i = 1; i < width - 1; i++) {
        x[i + j0]  = b == 2 ? -x[i + j1] : x[i + j1];   // down
        x[i + jN1] = b == 2 ? -x[i + jN2] : x[i + jN2]; // up
    }
    // horizontal borders(right and left)
    for (int j = 1; j < height - 1; j++) {
        const int ji = j * width;
        x[i0 + ji]  = b == 1 ? -x[i1 + ji] : x[i1 + ji];   // Left
        x[iN1 + ji] = b == 1 ? -x[iN2 + ji] : x[iN2 + ji]; // Right
    }
    // corners (mean of the 2 nearest)
    x[i0  + j0]  = 0.5f * (x[i1  + j0] + x[i0  + j1]);  // corner down left
    x[i0  + jN1] = 0.5f * (x[i1  + jN1] + x[i0  + jN2]); // corner up left
    x[iN1 + j0]  = 0.5f * (x[iN2 + j0] + x[iN1 + j1]);   // corner down right
    x[iN1 + jN1] = 0.5f * (x[iN2 + jN1] + x[iN1 + jN2]); // corner up right
}

void obstacleFlu::add_dens(const int x, const int y) const {
    dens_prev[x + y * width] += 0.5f;
}

void obstacleFlu::add_permanent_dens(const int x, const int y, const float radius) const {
    for(int j = 0; j < height; j++) {
        const int jw = j * width;
        for(int i = 0; i < width; i++) {
            const int ij = i + jw;
            const auto dx = static_cast<float>(i - x);
            const auto dy = static_cast<float>(j - y);
            if (std::sqrt(dx * dx + dy * dy) < radius) {
                dens_permanent[ij] = 1.0f;
            }
        }
    }
}

void obstacleFlu::add_vel(const int x, const int y, const float u_intensity, const float v_intensity) const {
    u_prev[x + y * width] = u_intensity;
    u_prev[x + y * width] = v_intensity;
}

void obstacleFlu::add_permanent_vel(const int x, const int y, const float u_intensity, const float v_intensity) const {
    u_permanent[x + y * width] = u_intensity;
    v_permanent[x + y * width] = v_intensity;
}

void obstacleFlu::add_all_perm_step() const {
    for(int i = 0; i < width * height; i ++) {
        if(dens_permanent[i] > 0.0f) dens_prev[i] += 0.01f;
        if(u_permanent[i] > 0.0f) u_prev[i] += u_permanent[i];
        if(v_permanent[i] > 0.0f) v_prev[i] += v_permanent[i];
    }
}

void obstacleFlu::inputs_step(const int r, const float intensity) const  {

    if (left_mouse_pressed || right_mouse_pressed || middle_mouse_pressed) {
        const int i = static_cast<int>(mouse_x) / cell_size;
        const int j = static_cast<int>((static_cast<float>(cell_size * height) - mouse_y)) / cell_size;

        if (i >= 1 && i < width - 1 && j >= 1 && j < height - 1) {
            if(left_mouse_pressed || middle_mouse_pressed) {
                for(int x = -r; x <= r; x++) {
                    for(int y = -r; y <= r; y++) {
                        if (i + x >= 1 && i + x < width - 1 && j + y >= 1 && j + y < height - 1) {
                            if (std::sqrt(static_cast<float>(x * x + y * y)) < static_cast<float>(r)) {
                                if(middle_mouse_pressed) {
                                    add_permanent_vel(i + x, j + y, 0 , intensity);

                                } else {
                                    add_vel(i + x, j + y, (mouse_x - force_x) , (mouse_y - force_y));
                                }
                            }
                        }
                    }
                }
                force_x = mouse_x;
                force_y = mouse_y;
            }
            if(right_mouse_pressed) {
                add_dens(i, j);
            }
        }
    }
    add_all_perm_step();
}

static void xy2hsv2rgb(const float x, const float y, float &r, float &g, float &b, const float r_max) {
    const float h = std::atan2(y, x) * 180 / static_cast<float>(3.14159265358979323846) + 180;
    constexpr float s = 1.f;
    const float v = std::sqrt(x * x + y * y) / r_max;
    const int segment = static_cast<int>(h / 60) % 6;  // Determine dans quel segment H tombe
    const float f = h / 60 - static_cast<float>(segment);  // Facteur fractionnaire de H
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

float *obstacleFlu::draw(const DRAW_MODE mode) const {
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
            color[ij * 3 + 0] = r;
            color[ij * 3 + 1] = g;
            color[ij * 3 + 2] = b;
        }
    }
    return color;
}

float obstacleFlu::find_max(const float* x) const {
    float max = x[0];
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            if(const int ij = i + jw; x[ij] > max) max = x[ij];
        }
    }
    return max;
}

float obstacleFlu::find_min(const float* x) const {
    float min = x[0];
    for (int j = 1 ; j < height - 1 ; j++ ) {
        const int jw = j * width;
        for (int i = 1 ; i < width - 1 ; i++ ) {
            if(const int ij = i + jw; x[ij] < min) min = x[ij];
        }
    }
    return min;
}

