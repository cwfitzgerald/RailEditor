#version 330 core

in vec2 vTexCoords;
in float vHeight;

layout (location = 0) out vec4 AlbedoSpec;
layout (location = 1) out vec3 Normal;

uniform sampler2D heightmap;
uniform mat3 normal_mat;

const vec2 size = vec2(0.05, 0.0002);
const ivec3 off = ivec3(-1, 0, 1);

void main() {
	float s11 = max(texture(heightmap, vTexCoords).r, 0) * 2;
	float s01 = max(textureOffset(heightmap, vTexCoords, off.xy).x, 0) * 2;
	float s21 = max(textureOffset(heightmap, vTexCoords, off.zy).x, 0) * 2;
	float s10 = max(textureOffset(heightmap, vTexCoords, off.yx).x, 0) * 2;
	float s12 = max(textureOffset(heightmap, vTexCoords, off.yz).x, 0) * 2;
	vec3 va = normalize(vec3(size.xy, s21 - s01));
	vec3 vb = normalize(vec3(size.yx, s12 - s10));
	vec4 bump = vec4(cross(va, vb), s11);
	vec3 normal = normalize(bump.xyz * 2.0 - 1.0);

	float height = vHeight;
	vec3 color = height >= 0.45 ? vec3(1, 1, 1) :
	             height >= 0.4  ? vec3(0.93, 0.62, 0.20) :
	             height >  0.0  ? vec3(0.136, 0.855, 0.16) :
	                              vec3(0.0898, 0.027, 0.9727);

	AlbedoSpec = vec4(color, 1.0);
	//Normal = normalize( normal);
	Normal = normal_mat * bump.xyz;
}