#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D x;
layout (r32f, binding = 1) uniform image2D s;

uniform float dt;

void main(){
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    float x_val =  imageLoad(x, coord).x;
    float s_val = imageLoad(s, coord).x;
    x_val += s_val * dt;
    imageStore(x, coord, vec4(x_val, 0.0, 0.0, 0.0));
}