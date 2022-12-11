#include "TriangleRendererDSA.h"

#include <gl/glew.h>
#include <string>

#include "./loadShaders.h"

// DSA would bind automatically, so no glBind* commands
TriangleRendererDSA::TriangleRendererDSA() {
	// cpu -> buffer -> gpu
	GLuint vbo;
	GLuint ibo;
	// init and bind buffer
	// glCreate* = glGen* + glBindBuffer
	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	glCreateBuffers(1, &ibo);
	// pass data from cpu to buffer	
	// glNamedBufferData = glBufferData
	// get buffer target type by buffer itself
	glNamedBufferData(vbo, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
	glNamedBufferData(ibo, sizeof(indices), indices.data(), GL_STATIC_DRAW);
	//glNamedBufferStorage(vbo, 18 * sizeof(GL_FLOAT), vertices, GL_DYNAMIC_STORAGE_BIT);
	// bind buffer to variable in gpu
	// glVertexArrayVertexBuffer + glVertexArrayAttribFormat = glVertexAttribPointer + vao bind
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(GL_FLOAT) * 6);
	glVertexArrayElementBuffer(vao, ibo);

	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3);

	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);

	// enable variable
	// glEnableVertexArrayAttrib(DSA) = glEnableVertexAttribArray(VAO)
	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);

	const std::string VERTEX_SHADER_PATH = "shaders/triangle.vert";
	const std::string FRAGMENT_SHADER_PATH = "shaders/triangle.frag";
	ShaderInfo shaders[] = {
	 {GL_VERTEX_SHADER,   VERTEX_SHADER_PATH.c_str()},
	 {GL_FRAGMENT_SHADER, FRAGMENT_SHADER_PATH.c_str()},
	 {GL_NONE,            nullptr}
	};

	program = LoadShaders(shaders);
}

void TriangleRendererDSA::render(int width, int height) const {
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