#include "ModelRenderer.h"

#include <gl/glew.h>
#include <string>
#include <glm/gtc/type_ptr.hpp>

void ModelRenderer::render(const Mesh& mesh, Camera* camera) const {
	glViewport(camera->viewport.x, camera->viewport.y, camera->viewport.z, camera->viewport.w);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// bind program
	glUseProgram(mesh.program);
	// set vertex data
	glBindVertexArray(mesh.vao);
	// update uniform
	glUniformMatrix4fv(mesh.uModelMatrix, 1, GL_FALSE, glm::value_ptr(mesh.modelMatrix));
	glUniformMatrix4fv(mesh.uViewMatrix, 1, GL_FALSE, glm::value_ptr(camera->view));
	glUniformMatrix4fv(mesh.uProjectionMatrix, 1, GL_FALSE, glm::value_ptr(camera->projection));
	auto normalMatrix = glm::transpose(glm::inverse(mesh.modelMatrix));
	glUniformMatrix4fv(mesh.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	
	// update texture
	for (size_t i = 0; i < mesh.textures.size(); ++i) {
		// vao style
		//glActiveTexture(GL_TEXTURE0 + i);
		//glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);

		// DSA style
		// glBindTextureUnit = glActiveTexture + glBindTexture
		glBindTextureUnit(i, mesh.textures[i].id);

		glUniform1i(mesh.textures[i].location, i);
	}

	// draw
	glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	// reset	
	glBindVertexArray(0);
	glUseProgram(0);

	//// bind pipeline
	//glBindProgramPipeline(mesh.pipeline);
	//// set vertex data
	//glBindVertexArray(mesh.vao);
	//// draw
	//glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
	//// reset	
	//glBindVertexArray(0);
	//glUseProgram(0);
}