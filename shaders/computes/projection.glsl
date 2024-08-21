#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (rg32f, binding = 0) uniform image2D vel;
layout (r32f, binding = 1) uniform image2D grid;
layout (r32f, binding = 2) uniform image2D results;



// Variables à passer en uniform
float h = 1;
float density = 1.0;
float timeStep = 0.1;

float o = 1.9;


void main() {

    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 size = imageSize(grid);
    int i = coord[0];
    int j = coord[1];
    int width = size[0];
    int height = size[1];
    if (i == 0 || j == 0 || i == width - 1 || j == height - 1) return;

    float uij = imageLoad(vel, coord).x;
    float vij = imageLoad(vel, coord).y;
    float ui1j = imageLoad(vel, ivec2(i + 1, j)).x;
    float vij1 = imageLoad(vel, ivec2(i, j + 1)).y;
    d = o * (ui1j - uij + vij1 - vij);

    float sip = imageLoad(grid, ivec2(i + 1, j)).x;
    float sim = imageLoad(grid, ivec2(i - 1, j)).x;
    float sjp = imageLoad(grid, ivec2(i, j + 1)).x;
    float sjm = imageLoad(grid, ivec2(i, j - 1)).x;

    float s = sip + sim + sjp + sjm;
    /*
    uij = uij + d * sim / s;
    vij = vij + d * sjm / s;
    ui1j = ui1j - d * sip / s;
    vij1 = vij1 - d * sjp / s;
    */
    if (s == 0.0) return;
    float result = d / s;


    imageStore(results, coord, result);
}
