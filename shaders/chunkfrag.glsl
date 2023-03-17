#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float visibility;
in float Light;

uniform sampler2D ourTexture;

const vec3 fogColor = vec3(0.7, 0.9, 1.0);

void main() {
    FragColor = texture(ourTexture, TexCoord);
    FragColor = vec4(FragColor.r * Light, FragColor.g * Light, FragColor.b * Light, FragColor.a);
    FragColor = mix(vec4(fogColor, 1.0), FragColor, visibility);
}