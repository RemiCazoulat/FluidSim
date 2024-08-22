#version 460 core

layout (local_size_x = 64, local_size_y = 1) in;

layout (rg32f, binding = 0) uniform image2D vel;
layout (r32f, binding = 1) uniform image2D density0;
layout (r32f, binding = 2) uniform image2D densityTransi;


// Variables à passer en uniform
float k = 20;
float timeStep = 0.1;





bool checkBoundaries(ivec2 coord) {
    ivec2 size = imageSize(density0);
    if (coord.x == 0 || coord.x == size.x || coord.y == 0 || coord.y == size.y) {
        return true;
    }
    return false;
}



float inversedDiffusion(ivec2 coord) {
    ivec2 size = imageSize(density0);

    float currentDens = imageLoad(density0, coord).x;
    ivec2 leftCoord  = clamp(ivec2(coord.x - 1, coord.y), ivec2(0), size - 1);
    ivec2 rightCoord = clamp(ivec2(coord.x + 1, coord.y), ivec2(0), size - 1);
    ivec2 upCoord    = clamp(ivec2(coord.x, coord.y - 1), ivec2(0), size - 1);
    ivec2 downCoord  = clamp(ivec2(coord.x, coord.y + 1), ivec2(0), size - 1);
    float nextLeftDens = imageLoad(densityTransi, leftCoord).x;
    float nextRightDens = imageLoad(densityTransi, rightCoord).x;
    float nextUpDens = imageLoad(densityTransi, upCoord).x;
    float nextDownDens = imageLoad(densityTransi, downCoord).x;
    float nextDens = (currentDens + k * (nextLeftDens + nextRightDens + nextUpDens + nextDownDens) / 4.0) / ( 1.0 +  k);
    return nextDens;
}

float diffusion(ivec2 coord) {
    ivec2 size = imageSize(density0);
    float currentDens = imageLoad(density0, coord).x;
    float densLeftValue  = 0.0;
    float densRightValue = 0.0;
    float densUpValue    = 0.0;
    float densDownValue  = 0.0;
    int total = 0;
    if (coord.x - 1 >= 0) {
        total++;
        ivec2 leftCoord  = ivec2(coord.x - 1, coord.y);
        densLeftValue = imageLoad(density0, leftCoord).x;
    }
    if (coord.x + 1 < size.x) {
        total++;
        ivec2 rightCoord = ivec2(coord.x + 1, coord.y);
        densRightValue = imageLoad(density0, rightCoord).x;
    }
    if (coord.y - 1 >= 0) {
        total++;
        ivec2 upCoord    = ivec2(coord.x, coord.y - 1);
        densUpValue = imageLoad(density0, upCoord).x;
    }
    if (coord.y + 1 < size.y) {
        total++;
        ivec2 downCoord  = ivec2(coord.x, coord.y + 1);
        densDownValue = imageLoad(density0, downCoord).x;
    }
    return currentDens + k * (densLeftValue + densRightValue + densUpValue + densDownValue - total * currentDens);
}


void main() {

    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    float density = 0.0;
    /*
    if (checkBoundaries(coord)) {
        imageStore(densityTransi, coord, vec4(0.0, 0.0, 0.0, 0.0));
        return;
    }
    */
    density = inversedDiffusion(coord);
    imageStore(densityTransi, coord, vec4(density, 0.0, 0.0, 0.0));

}
