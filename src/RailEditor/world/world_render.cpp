#include "world.hpp"

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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(graphics::Vertex), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(graphics::Vertex), (GLvoid*) (0 * sizeof(GLfloat))); // location
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(graphics::Vertex), (GLvoid*) (3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(graphics::Vertex), (GLvoid*) (6 * sizeof(GLfloat))); // texcoords

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(decltype(indices)::value_type), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void world::Surface::render() {
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
}
