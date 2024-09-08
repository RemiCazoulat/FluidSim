#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D z;
layout (r32f, binding = 1) uniform image2D z0;
layout (r32f, binding = 2) uniform image2D u;
layout (r32f, binding = 3) uniform image2D v;
layout (r32f, binding = 4) uniform image2D grid;

// Variables à passer en uniform
uniform float dtw;
uniform float dth;
uniform int width;
uniform int height;

void main() {
    ivec2 ij = ivec2(gl_GlobalInvocationID.xy);
    float s =  imageLoad(grid, ij).x;
    if (s == 0.0) {
        return;
    }
    float u_val = imageLoad(u, ij).x;
    float v_val = imageLoad(v, ij).x;
    const float delta_x = dtw * u_val;
    const float delta_y = dth * v_val;

    float x = float(ij.x) - delta_x;
    float y = float(ij.y) - delta_y;

    if(x < 0.5) x = 0.5;
    if(x > float(width) - 1.5) x = float(width) - 1.5;
    const int i0 = int(x);
    const int i1 = i0 + 1;

    if(y < 0.5) y = 0.5;
    if(y > float(width) - 1.5) y = float(width) - 1.5;
    const int j0 = int(x);
    const int j1 = j0 + 1;

    const float s1 = x - float(i0);
    const float s0 = 1 - s1;
    const float t1 = y - float(j0);
    const float t0 = 1 - t1;

    ivec2 i0j0 = ivec2(i0, j0);
    ivec2 i1j0 = ivec2(i1, j0);
    ivec2 i0j1 = ivec2(i0, j1);
    ivec2 i1j1 = ivec2(i1, j1);
    const float z00 = imageLoad(z0, i0j0).x;
    const float z10 = imageLoad(z0, i1j0).x;
    const float z01 = imageLoad(z0, i0j1).x;
    const float z11 = imageLoad(z0, i1j1).x;
    const float new_z = s0 * (t0 * z00 + t1 * z01) + s1 * (t0 * z10 + t1 * z11);
    imageStore(z, ij, vec4(new_z, 0.0, 0.0, 0.0));
}

/*
void obstacleFlu::advect(float * z, const float * z0, const float * u_vel, const float * v_vel, const float dt) const {
    const float dt0w = dt * static_cast<float>(width);
    const float dt0h = dt * static_cast<float>(height);
    for (int j = 1; j < height - 1; j++ ) {
        const int jw = j * width;
        for (int i = 1; i < width - 1; i++ ) {
            if(grid[i + jw] == 0.f) continue;

            const float delta_x = dt0w * u_vel[i + jw];
            const float delta_y = dt0h * v_vel[i + jw];

            float x = static_cast<float>(i) - delta_x;
            float y = static_cast<float>(j) - delta_y;

            if (x < 0.5) x = 0.5;
            if (x > static_cast<float>(width) - 1.5) x = static_cast<float>(width) - 1.5f;
            const int i0 = static_cast<int>(x);
            const int i1 = i0 + 1;

            if (y < 0.5) y = 0.5;
            if (y > static_cast<float>(height) - 1.5) y = static_cast<float>(height) - 1.5f;
            const int j0 = static_cast<int>(y);
            const int j1 = j0 + 1;

            const float s1 = x - static_cast<float>(i0);
            const float s0 = 1 - s1;
            const float t1 = y - static_cast<float>(j0);
            const float t0 = 1 - t1;
            const float new_z = s0 * (t0 * z0[i0 + j0 * width] + t1 * z0[i0 + j1 * width]) +
                                s1 * (t0 * z0[i1 + j0 * width] + t1 * z0[i1 + j1 * width]);
            z[i + jw] = new_z;
        }
    }
}
*/