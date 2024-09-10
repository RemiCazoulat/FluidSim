#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D x;
layout (r32f, binding = 1) uniform image2D x0;
layout (r32f, binding = 2) uniform image2D grid;

uniform float a;

void main(){
    ivec2 ij = ivec2(gl_GlobalInvocationID.xy);
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
    const float x0x = imageLoad(x, i0j).x;
    const float x1x = imageLoad(x, i1j).x;
    const float x0y = imageLoad(x, ij0).x;
    const float x1y = imageLoad(x, ij1).x;
    const float divergence = x0x * s0x + x1x * s1x + x0y * s0y + x1y * s1y;

    float x0_val = imageLoad(x0, ij).x;

    float new_x = (x0_val + a * divergence) / (1 + s * a);

    imageStore(x, ij, vec4(new_x, 0.0, 0.0, 0.0));
}
