#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D texSrc;
layout (r32f, binding = 1) uniform image2D texDst;


void main() {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);

    float valueSrc = imageLoad(texSrc, coord).x;
    imageStore(texDst, coord, vec4(valueSrc, 0.0, 0.0, 0.0));
    imageStore(texSrc, coord, vec4(0.0, 0.0, 0.0, 0.0));
}
