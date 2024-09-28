#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D u;
layout (r32f, binding = 1) uniform image2D v;
layout (r32f, binding = 2) uniform image2D p;
layout (r32f, binding = 3) uniform image2D div;
layout (r32f, binding = 4) uniform image2D grid;

// Variables à passer en uniform
uniform float h_w;
uniform float h_h;
uniform int step;

void step_1(ivec2 ij) {
    imageStore(p, ij, vec4(0.0, 0.0, 0.0, 0.0));
    imageStore(div, ij, vec4(0.0, 0.0, 0.0, 0.0));

    float s =  imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }
    ivec2 i0j = ivec2(ij.x - 1, ij.y);
    ivec2 i1j = ivec2(ij.x + 1, ij.y);
    ivec2 ij0 = ivec2(ij.x, ij.y - 1);
    ivec2 ij1 = ivec2(ij.x, ij.y + 1);
    float u0x = imageLoad(u, i0j).x;
    float u1x = imageLoad(u, i1j).x;
    float v0y = imageLoad(v, ij0).x;
    float v1y = imageLoad(v, ij1).x;
    float s0x = imageLoad(grid, i0j).x;
    float s1x = imageLoad(grid, i1j).x;
    float s0y = imageLoad(grid, ij0).x;
    float s1y = imageLoad(grid, ij1).x;
    s = s0x + s1x + s0y + s1y;
    //float divergence = -0.5 * h * (u1x - u0x + v1y - v0y);
    //float divergence = -1/s * h * (u1x - u0x + v1y - v0y);
    float divergence = -1/s * (h_w * (u1x - u0x) + h_h * (v1y -v0y));
    imageStore(div, ij, vec4(divergence, 0.0, 0.0, 0.0));
}

void step_2(ivec2 ij) {

    ivec2 i0j = ivec2(ij.x - 1, ij.y);
    ivec2 i1j = ivec2(ij.x + 1, ij.y);
    ivec2 ij0 = ivec2(ij.x, ij.y - 1);
    ivec2 ij1 = ivec2(ij.x, ij.y + 1);
    float s0x = imageLoad(grid, i0j).x;
    float s1x = imageLoad(grid, i1j).x;
    float s0y = imageLoad(grid, ij0).x;
    float s1y = imageLoad(grid, ij1).x;
    float s = s0x + s1x + s0y + s1y;
    float p0x = imageLoad(p, i0j).x;
    float p1x = imageLoad(p, i1j).x;
    float p0y = imageLoad(p, ij0).x;
    float p1y = imageLoad(p, ij1).x;
    float divergence = imageLoad(div, ij).x;
    float new_p = (divergence + p0x * s0x + p1x * s1x + p0y * s0y + p1y * s1y) / s;
    imageStore(p, ij, vec4(new_p, 0.0, 0.0, 0.0));
}

void step_3(ivec2 ij) {
    ivec2 i0j = ivec2(ij.x - 1, ij.y);
    ivec2 i1j = ivec2(ij.x + 1, ij.y);
    ivec2 ij0 = ivec2(ij.x, ij.y - 1);
    ivec2 ij1 = ivec2(ij.x, ij.y + 1);
    float s0x = imageLoad(grid, i0j).x;
    float s1x = imageLoad(grid, i1j).x;
    float s0y = imageLoad(grid, ij0).x;
    float s1y = imageLoad(grid, ij1).x;
    float s = s0x + s1x + s0y + s1y;
    float p0x = imageLoad(p, i0j).x;
    float p1x = imageLoad(p, i1j).x;
    float p0y = imageLoad(p, ij0).x;
    float p1y = imageLoad(p, ij1).x;
    float old_u = imageLoad(u, ij).x;
    float old_v = imageLoad(v, ij).x;
    float new_u = old_u - (p1x - p0x) / (h_w * 2);
    float new_v = old_v - (p1y - p0y) / (h_h * 2);
    //float new_u = old_u - (p1x - p0x) / (h * (s1x + s0x));
    //float new_v = old_v - (p1y - p0y) / (h * (s1y + s0y));

    imageStore(u, ij, vec4(new_u, 0.0, 0.0, 0.0));
    imageStore(v, ij, vec4(new_v, 0.0, 0.0, 0.0));
}

void main() {
    ivec2 ij = ivec2(gl_GlobalInvocationID.xy);
    float s = imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }
    if(step == 1) step_1(ij);
    if(step == 2) step_2(ij);
    if(step == 3) step_3(ij);
}
