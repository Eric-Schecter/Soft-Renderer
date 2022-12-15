#include "ModelRenderer.h"

#include <gl/glew.h>
#include <string>

void ModelRenderer::render(int width, int height,const Mesh& mesh) const {
	glViewport(0, 0, width, height);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// bind program
	glUseProgram(mesh.program);
	// set vertex data
	glBindVertexArray(mesh.vao);
	// draw
	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	// reset	
	glBindVertexArray(0);
	glUseProgram(0);
}