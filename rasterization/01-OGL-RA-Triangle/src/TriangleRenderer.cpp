#include "TriangleRenderer.h"

#include <gl/glew.h>
#include <string>

#include "./loadShaders.h"

TriangleRenderer::TriangleRenderer() {
	GLfloat vertices[] = {
	-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	// cpu -> buffer -> gpu

	GLuint vbo;
	// init buffer
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	// bind buffer
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	// pass data from cpu to buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// bind buffer to variable in gpu
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (void*)(sizeof(GLfloat) * 3));
	// enable variable
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	// reset
	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	glDrawArrays(GL_TRIANGLES, 0, 3);
	// reset
	glBindVertexArray(0);
	glUseProgram(0);
}