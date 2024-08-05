#version 460 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D velTex;
uniform sampler2D densTex;


void main(){
    vec3 currentDens = texture(densTex, TexCoords).xxx;
    fragColor = vec4(currentDens, 1.0);
}
