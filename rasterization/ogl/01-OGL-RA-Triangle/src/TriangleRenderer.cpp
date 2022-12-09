#include "TriangleRenderer.h"

#include <gl/glew.h>
#include <string>

#include "./loadShaders.h"

TriangleRenderer::TriangleRenderer() {
	// cpu -> buffer -> gpu
	GLuint vbo;
	GLuint ibo;
	// init buffer
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);
	// bind buffer
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	// pass data from cpu to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
	// bind buffer to variable in gpu
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(sizeof(GLfloat) * 3));
	// enable variable
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// reset
	glBindVertexArray(0); // can be ignored

	const std::string VERTEX_SHADER_PATH = "shaders/triangle.vert";
	const std::string FRAGMENT_SHADER_PATH = "shaders/triangle.frag";
	ShaderInfo shaders[] = {
	 {GL_VERTEX_SHADER,   VERTEX_SHADER_PATH.c_str()},
	 {GL_FRAGMENT_SHADER, FRAGMENT_SHADER_PATH.c_str()},
	 {GL_NONE,            nullptr}
	};

	program = LoadShaders(shaders);
}

void TriangleRenderer::render(int width, int height) const {
	glViewport(0, 0, width, height);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);

	// bind program
	glUseProgram(program);
	// set vertex data
	glBindVertexArray(vao);
	// draw
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	// reset	
	glBindVertexArray(0);
	glUseProgram(0);
}