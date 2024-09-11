#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D u;
layout (r32f, binding = 1) uniform image2D v;
layout (r32f, binding = 2) uniform image2D grid;

void main(){
    ivec2 ij = ivec2(gl_GlobalInvocationID.xy);
    float s =  imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }
    float x = imageLoad(u, ij).x;
    float y = imageLoad(v, ij).x;
    ivec2 i0j = ivec2(ij.x - 1, ij.y);
    ivec2 i1j = ivec2(ij.x + 1, ij.y);
    ivec2 ij0 = ivec2(ij.x, ij.y - 1);
    ivec2 ij1 = ivec2(ij.x, ij.y + 1);
    float s0x = imageLoad(grid, i0j).x;
    float s1x = imageLoad(grid, i1j).x;
    float s0y = imageLoad(grid, ij0).x;
    float s1y = imageLoad(grid, ij1).x;
    s = s0x + s1x + s0y + s1y;
    //if(s < 2.5) return;
    if(x > 0.0 && s1x == 0.0 || x < 0.0 && s0x == 0.0) x = -x;
    if(y > 0.0 && s1y == 0.0 || y < 0.0 && s0y == 0.0) y = -y;
    imageStore(u, ij, vec4(x, 0.0, 0.0, 0.0));
    imageStore(v, ij, vec4(y, 0.0, 0.0, 0.0));
}

