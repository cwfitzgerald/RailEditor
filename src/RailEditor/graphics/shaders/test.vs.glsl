#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec2 vTexCoords;

uniform mat4 pvm_mat;
uniform sampler2D heightmap;

void main() {
	// vTexCoords = texcoord;
	// gl_Position = pvm_mat * vec4(position, 1.0);
	vTexCoords = texcoord;
	gl_Position = pvm_mat * vec4(vec3(0, 0, texture(heightmap, texcoord).r * 2) + position, 1.0);
}
