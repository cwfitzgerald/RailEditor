#version 330 core

in vec2 vTexCoords;
out vec4 FragColor;

uniform sampler2D heightmap;

void main() {
	FragColor = vec4(texture(heightmap, vTexCoords).rrr, 1.0);
}