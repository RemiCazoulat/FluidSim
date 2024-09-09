#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

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
        rgb = xy2hsv2rgb(x, y, 0.1);

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

/*
GLuint obstacleFlu::draw(const DRAW_MODE mode) const {
    const float max_u = find_max(u);
    const float max_v = find_max(v);
    const float r_max = std::sqrt(max_u * max_u + max_v * max_v);
    const float max_d = find_max(dens);
    const float min_d = find_min(dens);
    const float delta_d = max_d - min_d;

    float r, g, b;

    for (int j = 0 ; j < height ; j++ ) {
        const int jw = j * width;
        for (int i = 0 ; i < width ; i++ ) {
            const int ij = i + jw;
            if(mode == VELOCITY) {
                const float x = u[ij];
                const float y = v[ij];
                xy2hsv2rgb(x, y, r, g, b, r_max);
            }
            if(mode == DENSITY) {
                float x = dens[ij];
                x = (x + delta_d - max_d) / delta_d;
                r = x; g = x; b = x;
            }
            if (mode == PRESSURE) {
                const float max_p = find_max(pressure);
                const float min_p = find_min(pressure);
                getSciColor(pressure[ij], min_p, max_p, r, g, b);
            }
            color[ij * 3 + 0] = r;
            color[ij * 3 + 1] = g;
            color[ij * 3 + 2] = b;
        }
    }
    GLuint colorTex = createTextureVec3(color, width, height);
    return colorTex;
}
*/