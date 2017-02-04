#version 330 core

in vec2 vTexCoords;

out vec4 ScreenColor;

uniform sampler2D AlbedoSpec;
uniform sampler2D Normal;

void main() {
	ScreenColor = vec4(texture(AlbedoSpec, vTexCoords).rgb * 0.5 + 0.5, 1.0);
}
