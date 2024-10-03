#version 460 core

layout (local_size_x = 8, local_size_y = 8) in;

layout (r32f, binding = 0) uniform image2D u;
layout (r32f, binding = 1) uniform image2D v;
layout (rgba32f, binding = 2) uniform image2D color;

const float PI = 3.14159265358979323846;

uniform int draw_mode;


vec3 xy2hsv2rgb(float x, float y, float r_max){
    float r,g,b;
    float h = atan(y, x) * 180 / PI + 180;
    float s = 1.0;
    float v = sqrt(x * x + y * y) / r_max;
    int segment = int(h / 60) % 6;  // Determine dans quel segment H tombe
    float f = h / 60 - float(segment);  // Facteur fractionnaire de H
    float p = v * (1 - s);
    float q = v * (1 - s * f);
    float t = v * (1 - s * (1 - f));
    switch (segment) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
        default :r = 1, g = 1, b = 1;
    }
    return vec3(r, g, b);
}

void main(){
    ivec2 ij = ivec2(gl_GlobalInvocationID.xy);
    float x =  imageLoad(u, ij).x;
    float y =  imageLoad(v, ij).x;
    vec3 rgb = vec3(1.0);
    if(draw_mode == 0) {
        rgb = xy2hsv2rgb(x, y, 0.5);

    }
    /*
    if(draw_mode == 1) {
        float d = imageLoad(color, ij).r;
        float delta_d = 1.0 - 0.0;
        d = (d + delta_d - 1.0) / delta_d;
        r = d; g = d; b = d;
    }
    */
    /*
    if (draw_mode == 2) {
        float p = imageLoad(color, ij).r;
        float min_p = 0.0;
        float max_p = 1.0;
        float delta_p = max_p - min_p;
        r = (p - min_p) / delta_p;
        g = (p - min_p) / delta_p;
        b = (p - min_p) / delta_p;
    }
    */
    imageStore(color, ij, vec4(rgb, 1.0));

}
