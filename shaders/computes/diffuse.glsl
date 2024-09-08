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
    const float s0n = imageLoad(grid, i0j).x;
    const float s1n = imageLoad(grid, i1j).x;
    const float sn0 = imageLoad(grid, ij0).x;
    const float sn1 = imageLoad(grid, ij1).x;
    s = s0n + s1n + sn0 + sn1;
    const float x0n = imageLoad(x, i0j).x;
    const float x1n = imageLoad(x, i1j).x;
    const float xn0 = imageLoad(x, ij0).x;
    const float xn1 = imageLoad(x, ij1).x;
    const float divergence = x0n * s0n + x1n * s1n + xn0 * sn0 + xn1 * sn1;

    float x0_val =  imageLoad(x0, ij).x;

    float new_x = (x0_val + a * d) / (1 + s * a);

    imageStore(x, ij, vec4(new_x, 0.0, 0.0, 0.0));
}

/*
void obstacleFlu::diffuse(float* x, const float* x0, const float diff, const float dt) const {
const float a = dt * diff * static_cast<float>(width) * static_cast<float>(height);
for (int k = 0 ; k < sub_step ; k++ ) {
    for ( int j = 1 ; j < height - 1; j++ ) {
        const int jw = j * width;
        const int jw0 = (j - 1) * width;
        const int jw1 = (j + 1) * width;
        for (int i = 1 ; i < width - 1; i++ ) {
            if(grid[i + jw] == 0.f) continue;
            const int i0 = i - 1;
            const int i1 = i + 1;
            const float s0n = grid[i0 + jw];
            const float s1n = grid[i1 + jw];
            const float sn0 = grid[i + jw0];
            const float sn1 = grid[i + jw1];
            const float s = s0n + s1n + sn0 + sn1;
            const float d = x[i0 + jw] * s0n + x[i1 + jw] * s1n + x[i + jw0] * sn0 + x[i + jw1] * sn1;
            x[i + jw] = (x0[i + jw] + a * d) / (1 + s * a);
        }
    }
}
}
*/