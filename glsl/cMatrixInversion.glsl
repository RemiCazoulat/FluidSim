#version 460 core

layout (local_size_x = 1, local_size_y = 1) in;

layout (r32f, binding = 0) uniform image2D initMatrix;
layout (r32f, binding = 1) uniform image2D invMatrix;

uniform bool normalize;
uniform int matrixSize;
// Only for normalization
uniform float ii;
uniform int currentLine;
// Only for substraction
uniform int lineToSub;


void normalizeLine(ivec2 coord) {
    // if coord is before pivot point of the initial line
    if(coord.x < coord.y) return;
    // if coord is after pivot point of the inversed line
    if(coord.x > matrixSize + coord.y) return;
    ////////////////////////////////////
    // if coord is in the initial line
    ////////////////////////////////////
    if (coord.x < matrixSize) {
        float value = imageLoad(initMatrix, coord).x;
        if (value == 0.0) return;
        float newValue = 1.0;
        if(coord.x != coord.y) newValue = value / ii;
        imageStore(initMatrix, coord, vec4(newValue, 0.0, 0.0, 0.0));
        return;
    }
    ////////////////////////////////////
    // if coord is in the inversed line
    ////////////////////////////////////
    if (coord.x >= matrixSize) {
        ivec2 newCoord = ivec2(coord.x - matrixSize, coord.y);
        float value = imageLoad(invMatrix, newCoord).x;
        if (value == 0.0) return;
        float newValue = value / ii;
        imageStore(invMatrix, newCoord, vec4(newValue, 0.0, 0.0, 0.0));
        return;
    }
}

void subtract(uint cLine){
    ivec2 multCoord = ivec2(lineToSub, cLine);
    float mult = imageLoad(initMatrix,multCoord).x;
    if(mult == 0.0) return;
    imageStore(initMatrix, multCoord, vec4(0.0, 0.0, 0.0, 0.0));
    ///////////////////////////////////////
    // loop to substract the initial line
    ///////////////////////////////////////
    for(int i = lineToSub + 1; i < matrixSize; i ++) {
        ivec2 loopCoord = ivec2(i, cLine);
        ivec2 loopCoordSub = ivec2(i, lineToSub);
        float value = imageLoad(initMatrix, loopCoord).x;
        float newValue = value - mult * imageLoad(initMatrix, loopCoordSub).x;
        imageStore(initMatrix, loopCoord, vec4(newValue, 0.0, 0.0, 0.0));
    }
    ///////////////////////////////////////
    // loop to substract the inversed line
    ///////////////////////////////////////
    for(int i = 0; i < lineToSub; i ++) {
        ivec2 loopCoord = ivec2(i, cLine);
        ivec2 loopCoordSub = ivec2(i, lineToSub);
        float value = imageLoad(invMatrix, loopCoord).x;
        float newValue = value - mult * imageLoad(invMatrix, loopCoordSub).x;
        imageStore(invMatrix, loopCoord, vec4(newValue, 0.0, 0.0, 0.0));
    }
}

void main() {
    uint coord = gl_GlobalInvocationID.x;
    if (normalize) {
        // Here, coord is the x of the line to normalize
        ivec2 newCoord = ivec2(coord, currentLine);
        normalizeLine(newCoord);
    }
    else {
        // Here, coord is the index of the line we want to modify
        subtract(coord);
    }
}
