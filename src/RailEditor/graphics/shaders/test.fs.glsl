#version 330 core

in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D heightmap;

void main() {
	float height = texture(heightmap, vTexCoords).r;
	vec3 color = height >= 0.45 ? vec3(1, 1, 1) :
	             height >= 0.4  ? vec3(0.93, 0.62, 0.20) :
	             height >= 0.0  ? vec3(0.136, 0.855, 0.16) :
	                              vec3(0.0898, 0.027, 0.9727);

	FragColor = vec4(color, 1.0);
}