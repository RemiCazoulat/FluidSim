#version 460 core

in vec2 TexCoords;

out vec4 fragColor;

uniform sampler2D velTex;
uniform sampler2D densTex;

void main(){
    vec3 color = texture(velTex, TexCoords).xyy;
    fragColor = vec4(color, 1.0);
}
