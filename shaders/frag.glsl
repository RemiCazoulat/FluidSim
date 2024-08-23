#version 460 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D velocityTex;
uniform sampler2D pressureTex;

void main(){
    vec3 color = texture(pressureTex, TexCoords).xxx;
    fragColor = vec4(color, 1.0);
}
