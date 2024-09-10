#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D z;
layout (r32f, binding = 1) uniform image2D z0;
layout (r32f, binding = 2) uniform image2D u;
layout (r32f, binding = 3) uniform image2D v;
layout (r32f, binding = 4) uniform image2D grid;

// Variables à passer en uniform
uniform float dtw;
uniform float dth;
uniform int width;
uniform int height;

void main() {
    ivec2 ij = ivec2(gl_GlobalInvocationID.xy);
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
    float z00 = imageLoad(z0, i0j0).x;
    float z01 = imageLoad(z0, i0j1).x;
    float z10 = imageLoad(z0, i1j0).x;
    float z11 = imageLoad(z0, i1j1).x;
    float new_z = s0 * (t0 * z00 + t1 * z01) + s1 * (t0 * z10 + t1 * z11);
    imageStore(z, ij, vec4(new_z, 0.0, 0.0, 0.0));
}

