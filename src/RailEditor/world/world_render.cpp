#include "world.hpp"
#include "utilities/math.hpp"

#include <algorithm>

float world::Heightmap::get_value(float x, float y) {
	float scaled_x = x * width;
	float scaled_y = y * height;

	scaled_x = utilities::clamp(scaled_x, 0.0f, width - 2);
	scaled_y = utilities::clamp(scaled_y, 0.0f, height - 2);

	auto index_x1 = static_cast<std::size_t>(std::floor(scaled_x));
	auto index_y1 = static_cast<std::size_t>(std::floor(scaled_y));
	auto index_x2 = static_cast<std::size_t>(std::floor(scaled_x) + 1);
	auto index_y2 = static_cast<std::size_t>(std::floor(scaled_y) + 1);

	auto val00 = vals[index_y1 * width + index_x1];
	auto val01 = vals[index_y2 * width + index_x1];
	auto val10 = vals[index_y1 * width + index_x2];
	auto val11 = vals[index_y2 * width + index_x2];

	return std::max(utilities::lerp2d(scaled_x - std::floor(scaled_x),
	                                  scaled_y - std::floor(scaled_y), val00, val10, val01, val11),
	                0.0f);
}

void world::Heightmap::upload() {
	if (gl_tex == 0) {
		glGenTextures(1, &gl_tex);
	}

	glBindTexture(GL_TEXTURE_2D, gl_tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, vals.data());
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void world::Heightmap::bind(std::size_t texnum) {
	glActiveTexture(GL_TEXTURE0 + texnum);
	glBindTexture(GL_TEXTURE_2D, gl_tex);
}

void world::Surface::upload() {
	if (vao == 0) {
		glGenVertexArrays(1, &vao);
	}
	if (vert_buff == 0) {
		glGenBuffers(1, &vert_buff);
	}
	if (index_buff == 0) {
		glGenBuffers(1, &index_buff);
	}

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vert_buff);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(graphics::Vertex), vertices.data(),
	             GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(graphics::Vertex),
	                      (GLvoid*) (0 * sizeof(GLfloat))); // location
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(graphics::Vertex),
	                      (GLvoid*) (3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(graphics::Vertex),
	                      (GLvoid*) (6 * sizeof(GLfloat))); // texcoords

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(decltype(indices)::value_type),
	             indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void world::Surface::render() {
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
