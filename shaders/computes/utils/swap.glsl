#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D x;
layout (r32f, binding = 1) uniform image2D y;

void main(){
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    float x_val =  imageLoad(x, coord).x;
    float y_val = imageLoad(y, coord).x;
    imageStore(x, coord, vec4(y_val, 0.0, 0.0, 0.0));
    imageStore(y, coord, vec4(x_val, 0.0, 0.0, 0.0));

}