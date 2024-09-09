#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D u;
layout (r32f, binding = 1) uniform image2D v;
layout (r32f, binding = 2) uniform image2D p;
layout (r32f, binding = 3) uniform image2D div;
layout (r32f, binding = 4) uniform image2D grid;

// Variables à passer en uniform
uniform float h;
uniform int step;

void step_1(ivec2 ij) {
    imageStore(p, ij, vec4(0.0, 0.0, 0.0, 0.0));
    float s =  imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }
    ivec2 i0j = ivec2(ij.x - 1, ij.y);
    ivec2 i1j = ivec2(ij.x + 1, ij.y);
    ivec2 ij0 = ivec2(ij.x, ij.y - 1);
    ivec2 ij1 = ivec2(ij.x, ij.y + 1);
    const float x0n = imageLoad(u, i0j).x;
    const float x1n = imageLoad(u, i1j).x;
    const float xn0 = imageLoad(v, ij0).x;
    const float xn1 = imageLoad(v, ij1).x;
    const float divergence = -0.5 * h * (x0n + x1n + xn0 + xn1);
    imageStore(div, ij, vec4(divergence, 0.0, 0.0, 0.0));
}

void step_2(ivec2 ij) {
    float s =  imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }
    ivec2 i0j = ivec2(ij.x - 1, ij.y);
    ivec2 i1j = ivec2(ij.x + 1, ij.y);
    ivec2 ij0 = ivec2(ij.x, ij.y - 1);
    ivec2 ij1 = ivec2(ij.x, ij.y + 1);
    const float s0n = imageLoad(grid, i0j).x;
    const float s1n = imageLoad(grid, i1j).x;
    const float sn0 = imageLoad(grid, ij0).x;
    const float sn1 = imageLoad(grid, ij1).x;
    s = s0n + s1n + sn0 + sn1;
    const float p0n = imageLoad(p, i0j).x;
    const float p1n = imageLoad(p, i1j).x;
    const float pn0 = imageLoad(p, ij0).x;
    const float pn1 = imageLoad(p, ij1).x;
    const float divergence = imageLoad(div, ij).x;
    const float new_p = (divergence + p0n * s0n + p1n * s1n + pn0 * sn0 + pn1 * sn1) / s;
    imageStore(p, ij, vec4(new_p, 0.0, 0.0, 0.0));
}

void step_3(ivec2 ij) {
    float s =  imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }
    ivec2 i0j = ivec2(ij.x - 1, ij.y);
    ivec2 i1j = ivec2(ij.x + 1, ij.y);
    ivec2 ij0 = ivec2(ij.x, ij.y - 1);
    ivec2 ij1 = ivec2(ij.x, ij.y + 1);
    const float s0n = imageLoad(grid, i0j).x;
    const float s1n = imageLoad(grid, i1j).x;
    const float sn0 = imageLoad(grid, ij0).x;
    const float sn1 = imageLoad(grid, ij1).x;
    s = s0n + s1n + sn0 + sn1;
    const float p0n = imageLoad(p, i0j).x;
    const float p1n = imageLoad(p, i1j).x;
    const float pn0 = imageLoad(p, ij0).x;
    const float pn1 = imageLoad(p, ij1).x;

    const float old_u = imageLoad(u, ij).x;
    const float old_v = imageLoad(v, ij).x;
    const float new_u = old_u - (p1n * s1n - p0n * s0n) / (h * 2);
    const float new_v = old_v - (pn1 * sn1 - pn0 * sn0) / (h * 2);
    imageStore(u, ij, vec4(new_u, 0.0, 0.0, 0.0));
    imageStore(v, ij, vec4(new_v, 0.0, 0.0, 0.0));
}

void main() {
    ivec2 ij = ivec2(gl_GlobalInvocationID.xy);
    if(step == 1) step_1(ij);
    if(step == 2) step_2(ij);
    if(step == 3) step_3(ij);
}
