#version 460 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D colorTex;


void main(){
    vec4 color;
    color = texture(colorTex, TexCoords);
    fragColor = color;
}
