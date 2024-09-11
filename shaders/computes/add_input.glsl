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
/*
for(int x = -r; x <= r; x++) {
    for(int y = -r; y <= r; y++) {
        if (i + x >= 1 && i + x < width - 1 && j + y >= 1 && j + y < height - 1) {
            if (std::sqrt(static_cast<float>(x * x + y * y)) < static_cast<float>(r)) {
                if(middle_mouse_pressed) {
                    add(i + x, j + y, u_permanent, 0);
                    add(i + x, j + y, v_permanent, intensity);
                    new_permanent = true;
                }
                if (left_mouse_pressed){
                    //add_vel(i + x, j + y, (mouse_x - force_x) , (mouse_y - force_y));
                    add(i + x, j + y, u_temp, (mouse_x - force_x));
                    add(i + x, j + y, v_temp, -(mouse_y - force_y));
                    new_temp = true;
                }
                if(right_mouse_pressed) {
                    add(i + x, j + y, dens_temp, intensity);
                    new_temp = true;
                }
            }
        }
    }
}
*/