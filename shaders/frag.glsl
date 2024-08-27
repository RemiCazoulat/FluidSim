#version 460 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D colorTex;


void main(){
    vec3 color;
    color = texture(colorTex, TexCoords).xyz;
    fragColor = vec4(color, 1.0);
}
