#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float visibility;

uniform sampler2D ourTexture;

const vec3 fogColor = vec3(0.4, 0.8, 1.0);

void main() {
    FragColor = texture(ourTexture, TexCoord);
    FragColor = mix(vec4(fogColor, 1.0), FragColor, visibility);
}