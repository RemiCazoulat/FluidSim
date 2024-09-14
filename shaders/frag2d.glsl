#version 460 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D colorTex;


void main(){
    vec4 color;
    color = texture(colorTex, TexCoords);
    //color = vec4(TexCoords, 0.0, 1.0);
    fragColor = color;
}
