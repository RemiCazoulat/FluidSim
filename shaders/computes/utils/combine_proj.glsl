#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (rg32f, binding = 0) uniform image2D vel;
layout (r32f, binding = 1) uniform image2D grid;
layout (rgba32f, binding = 2) uniform readonly image2D results;

void main() {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(results);
    int i = coord[0];
    int j = coord[1];
    int width = size[0];
    int height = size[1];
    if (i == 0 || j == 0 || i == width - 1 || j == height - 1)
    {
        return;
    }

    float sip = imageLoad(grid, ivec2(i + 1, j)).x;
    float sim = imageLoad(grid, ivec2(i - 1, j)).x;
    float sjp = imageLoad(grid, ivec2(i, j + 1)).x;
    float sjm = imageLoad(grid, ivec2(i, j - 1)).x;

    vec2 new_vel = imageLoad(vel, coord).xy;
    if(sim == 1) new_vel.x += imageLoad(results, ivec2(i - 1, j)).x;
    if(sip == 1) new_vel.x += imageLoad(results, ivec2(i + 1, j)).x;
    if(sjm == 1) new_vel.y += imageLoad(results, ivec2(i, j - 1)).x;
    if(sjp == 1) new_vel.y += imageLoad(results, ivec2(i, j + 1)).x;

    imageStore(vel, coord, vec4(new_vel, 0.0, 0.0));

}
