#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

out vec2 vTexCoords;
out float vHeight;

uniform mat4 m_mat;
uniform mat4 v_mat;
uniform mat4 p_mat;
uniform mat3 normal_mat;
uniform sampler2D heightmap;

void main() {
	float height = max(texture(heightmap, texcoord).r, 0) * 2;
	float real_height = position.z + height;
	vec3 adjusted = vec3(position.x, position.y, real_height);

	gl_Position = p_mat * v_mat * m_mat * vec4(adjusted, 1.0);
	vTexCoords = texcoord;
	vHeight = real_height;
}
