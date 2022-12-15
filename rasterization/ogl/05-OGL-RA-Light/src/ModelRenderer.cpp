#include "ModelRenderer.h"

#include <gl/glew.h>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "Lights.h"

void ModelRenderer::render(const Mesh& mesh, Camera* camera, const Lights& lights) const {
	glViewport(camera->viewport.x, camera->viewport.y, camera->viewport.z, camera->viewport.w);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// program, vertex handle and texture need to be reset
	// uniform belongs to program, so update uniform only when needed
	// texture contains two parts, uniform and texture data, update when needed
	
	//// bind program
	//glUseProgram(mesh.program);
	//// set vertex data
	//glBindVertexArray(mesh.vao);
	//// draw
	//glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	//// reset	
	//glBindVertexArray(0);
	//glUseProgram(0);

	// bind pipeline
	glBindProgramPipeline(mesh.pipeline);
	// set vertex data
	glBindVertexArray(mesh.vao);
	// draw
	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	// reset	
	glBindVertexArray(0);
	glUseProgram(0);
}