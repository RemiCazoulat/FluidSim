#version 460 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D pressureColorTex;
uniform sampler2D isBorderTex;


void main(){
    vec3 color;
    if (texture(isBorderTex, TexCoords).x != 0.0) {
        color = texture(pressureColorTex, TexCoords).xyz;
    }
    else {
        color = vec3(0.0, 0.0, 0.0);
    }
    fragColor = vec4(color, 1.0);
}
