#version 460 core
// macros for textures
#define GRID_T      0
#define DENS_T      1
#define DENS_PREV_T 2
#define DENS_PERM_T 3
#define U_T         4
#define V_T         5
#define U_PREV_T    6
#define V_PREV_T    7
#define U_PERM_T    8
#define V_PERM_T    9
#define COLOR_T     10
// macros for modes
#define INPUT     0
#define SOURCE    1
#define SWAP      2
#define DIFFUSE   3
#define ADVECT    4
#define PROJECT   5
#define BOUND_D   6
#define BOUND_V   7

layout (local_size_x = 64, local_size_y = 1) in;
// all the textures needed
layout (r32f, binding = 0) uniform image2D grid;
layout (r32f, binding = 1) uniform image2D dens;
layout (r32f, binding = 2) uniform image2D dens_prev;
layout (r32f, binding = 3) uniform image2D dens_perm;
layout (r32f, binding = 4) uniform image2D u;
layout (r32f, binding = 5) uniform image2D u_prev;
layout (r32f, binding = 6) uniform image2D u_perm;
layout (r32f, binding = 7) uniform image2D v;
layout (r32f, binding = 8) uniform image2D v_prev;
layout (r32f, binding = 9) uniform image2D v_perm;
layout (rgba32f, binding = 10) uniform image2D color;
// time
uniform float dt;
// To know which textures use
uniform int x_tex;
uniform int y_tex;
// To know which function call
uniform int mode;
// For add function
uniform float r;
uniform float intensity;


vec4 read_tex(int tex, ivec2 coord) {
    vec4 vec;
    if(tex == GRID_T)      vec = imageLoad(grid, coord);
    if(tex == DENS_T)      vec = imageLoad(dens, coord);
    if(tex == DENS_PREV_T) vec = imageLoad(dens_prev, coord);
    if(tex == DENS_PERM_T) vec = imageLoad(dens_perm, coord);
    if(tex == U_T)         vec = imageLoad(u, coord);
    if(tex == U_PREV_T)    vec = imageLoad(u_prev, coord);
    if(tex == U_PERM_T)    vec = imageLoad(u_perm, coord);
    if(tex == V_T)         vec = imageLoad(v, coord);
    if(tex == V_PREV_T)    vec = imageLoad(v_prev, coord);
    if(tex == V_PERM_T)    vec = imageLoad(v_perm, coord);
    if(tex == COLOR_T)     vec = imageLoad(color, coord);
    return vec;
}

void write_tex(int tex, ivec2 coord, vec4 val) {
    if(tex == GRID_T)      imageStore(grid, coord, val);
    if(tex == DENS_T)      imageStore(dens, coord, val);
    if(tex == DENS_PREV_T) imageStore(dens_prev, coord, val);
    if(tex == DENS_PERM_T) imageStore(dens_perm, coord, val);
    if(tex == U_T)         imageStore(u, coord, val);
    if(tex == U_PREV_T)    imageStore(u_prev, coord, val);
    if(tex == U_PERM_T)    imageStore(u_perm, coord, val);
    if(tex == V_T)         imageStore(v, coord, val);
    if(tex == V_PREV_T)    imageStore(v_prev, coord, val);
    if(tex == V_PERM_T)    imageStore(v_perm, coord, val);
    if(tex == COLOR_T)     imageStore(color, coord, val);
}

void swap(ivec2 coord) {
    vec4 x_val = read_tex(x_tex, coord);
    vec4 y_val = read_tex(_tex, coord);
    write_tex(y_tex, coord, x_val);
    write_tex(x_tex, coord, y_val);
}

void add(ivec2 coord) {
    float s = imageLoad(grid, coord).x;
    if (s == 0.0) return;
    if (sqrt(pow(coord.x - i, 2) + pow(coord.y - j, 2)) < r ) {
        vec4 val = read_tex(x_tex, coord);
        write_tex(x_tex, coord, val + intensity * dt);
    }
}

void source(ivec2 coord) {
    vec4 x_val = read_tex(x_tex, coord);
    vec4 y_val = read_tex(y_tex, coord);
    x_val += y_val * dt;
    write_tex( x_tex, coord, x_val);
}

void diffuse(ivec2 ij) {
    float s =  imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }
    ivec2 i0j = ivec2(ij.x - 1, ij.y);
    ivec2 i1j = ivec2(ij.x + 1, ij.y);
    ivec2 ij0 = ivec2(ij.x, ij.y - 1);
    ivec2 ij1 = ivec2(ij.x, ij.y + 1);
    const float s0x = imageLoad(grid, i0j).x;
    const float s1x = imageLoad(grid, i1j).x;
    const float s0y = imageLoad(grid, ij0).x;
    const float s1y = imageLoad(grid, ij1).x;
    s = s0x + s1x + s0y + s1y;
    const float x0x = read_tex(x_tex, i0j).x; // in this case : x_tex = x
    const float x1x = read_tex(x_tex, i1j).x; // in this case : x_tex = x
    const float x0y = read_tex(x_tex, ij0).x; // in this case : x_tex = x
    const float x1y = read_tex(x_tex, ij1).x; // in this case : x_tex = x
    const float divergence = x0x * s0x + x1x * s1x + x0y * s0y + x1y * s1y;
    float x0_val = read_tex(y_tex, ij).x; // in this case : y_tex = x0
    float new_x = (x0_val + a * divergence) / (1.0 + s * a);
    write_tex(x_tex, ij, vec4(new_x, 0.0, 0.0, 0.0));
}

void advect(ivec2 ij) {
    float s =  imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }

    float delta_x = dtw * imageLoad(u, ij).x;
    float delta_y = dth * imageLoad(v, ij).x;

    float x = float(ij.x) - delta_x;
    float y = float(ij.y) - delta_y;

    if(x < 0.5) x = 0.5;
    if(x > float(width) - 1.5) x = float(width) - 1.5;
    int i0 = int(x);
    int i1 = i0 + 1;

    if(y < 0.5) y = 0.5;
    if(y > float(height) - 1.5) y = float(height) - 1.5;
    int j0 = int(y);
    int j1 = j0 + 1;

    float s1 = x - float(i0);
    float s0 = 1.0 - s1;
    float t1 = y - float(j0);
    float t0 = 1.0 - t1;

    ivec2 i0j0 = ivec2(i0, j0);
    ivec2 i0j1 = ivec2(i0, j1);
    ivec2 i1j0 = ivec2(i1, j0);
    ivec2 i1j1 = ivec2(i1, j1);
    float z00 = read_tex(y_tex, i0j0).x; // in this case : y_tex = z0
    float z01 = read_tex(y_tex, i0j1).x; // in this case : y_tex = z0
    float z10 = read_tex(y_tex, i1j0).x; // in this case : y_tex = z0
    float z11 = read_tex(y_tex, i1j1).x; // in this case : y_tex = z0
    float new_z = s0 * (t0 * z00 + t1 * z01) + s1 * (t0 * z10 + t1 * z11);
    write_tex(x_tex, ij, vec4(new_z, 0.0, 0.0, 0.0));
}

void main() {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    if( mode == INPUT )    add(coord);
    if( mode == SOURCE )   source(coord);
    if( mode == SWAP )     swap(coord);
    if( mode == DIFFUSE )  diffuse(coord);
    if( mode == ADVECT )   advect(coord);
    if( mode == PROJECT );
    if( mode == BOUND_D );
    if( mode == BOUND_V );
}