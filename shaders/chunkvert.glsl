#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out float visibility;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

const float fogDensity = 0.012;
const float fogGradient = 4.0;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
	// calculate distance from camera of this vertex
	float distanceFromCamera = length(view * model * vec4(aPos, 1.0));
	visibility = exp(-pow((distanceFromCamera*fogDensity), fogGradient));
	visibility = clamp(visibility, 0.0, 1.0);
}