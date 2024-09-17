#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D tex;
layout (r32f, binding = 1) uniform image2D grid;

uniform int i;
uniform int j;
uniform float r;
uniform float intensity;
uniform float dt;

void main(){
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    float s = imageLoad(grid, coord).x;
    if(s == 0.0) return;
    if(sqrt(pow(coord.x - i, 2) + pow(coord.y - j, 2)) < r ) {
        float tex_val =  imageLoad(tex, coord).x;
        imageStore(tex, coord, vec4(tex_val + intensity * dt, 0.0, 0.0, 0.0));
    }
}