#include "ModelRenderer.h"

#include <gl/glew.h>
#include <string>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "Lights.h"
#include "AmbientLight.h"

void ModelRenderer::updateUniformsBasic(const Mesh& mesh, Camera* camera, const Lights& lights) const {
	auto uModelMatrix = mesh.uniforms.find("uModelMatrix");
	if (uModelMatrix != mesh.uniforms.end()) {
		glUniformMatrix4fv(uModelMatrix->second.location, 1, GL_FALSE, glm::value_ptr(mesh.modelMatrix));
	}
	auto uViewMatrix = mesh.uniforms.find("uViewMatrix");
	if (uViewMatrix != mesh.uniforms.end()) {
		glUniformMatrix4fv(uViewMatrix->second.location, 1, GL_FALSE, glm::value_ptr(camera->view));
	}
	auto uProjectionMatrix = mesh.uniforms.find("uProjectionMatrix");
	if (uProjectionMatrix != mesh.uniforms.end()) {
		glUniformMatrix4fv(uProjectionMatrix->second.location, 1, GL_FALSE, glm::value_ptr(camera->projection));
	}
	auto uNormalMatrix = mesh.uniforms.find("uNormalMatrix");
	if (uNormalMatrix != mesh.uniforms.end()) {
		auto normalMatrix = glm::transpose(glm::inverse(mesh.modelMatrix));
		glUniformMatrix4fv(uNormalMatrix->second.location, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	auto specular = mesh.uniforms.find("uMaterial.specular");
	if (specular != mesh.uniforms.end()) {
		glUniform1f(specular->second.location, mesh.uMaterial.specular);
	}
	auto shininess = mesh.uniforms.find("uMaterial.shininess");
	if (shininess != mesh.uniforms.end()) {
		glUniform1i(shininess->second.location, mesh.uMaterial.shininess);
	}
	auto uCameraPos = mesh.uniforms.find("uCameraPos");
	if (uCameraPos != mesh.uniforms.end()) {
		glUniform3fv(uCameraPos->second.location, 1, glm::value_ptr(camera->pos));
	}

	for (int i = 0; i < lights.ambientLights.size(); ++i) {
		auto uAmbientLightColor = mesh.uniforms.find("uAmbientLight[" + std::to_string(i) + "].color");
		if (uAmbientLightColor != mesh.uniforms.end()) {
			glUniform4fv(uAmbientLightColor->second.location, 1, glm::value_ptr(lights.ambientLights[i].color));
		}
		auto uAmbientLightIntensity = mesh.uniforms.find("uAmbientLight[" + std::to_string(i) + "].intensity");
		if (uAmbientLightIntensity != mesh.uniforms.end()) {
			glUniform1f(uAmbientLightIntensity->second.location, lights.ambientLights[i].intensity);
		}
	}

	for (int i = 0; i < lights.directionalLights.size(); ++i) {
		auto direction = mesh.uniforms.find("uDirectionalLight[" + std::to_string(i) + "].direction");
		if (direction != mesh.uniforms.end()) {
			glUniform3fv(direction->second.location, 1, glm::value_ptr(lights.directionalLights[i].direction));
		}
		auto color = mesh.uniforms.find("uDirectionalLight[" + std::to_string(i) + "].color");
		if (color != mesh.uniforms.end()) {
			glUniform4fv(color->second.location, 1, glm::value_ptr(lights.directionalLights[i].color));
		}
		auto intensity = mesh.uniforms.find("uDirectionalLight[" + std::to_string(i) + "].intensity");
		if (intensity != mesh.uniforms.end()) {
			glUniform1f(intensity->second.location, lights.directionalLights[i].intensity);
		}
	}

	for (int i = 0; i < lights.spotLights.size(); ++i) {
		auto pos = mesh.uniforms.find("uSpotLight[" + std::to_string(i) + "].pos");
		if (pos != mesh.uniforms.end()) {
			glUniform3fv(pos->second.location, 1, glm::value_ptr(lights.spotLights[i].pos));
		}
		auto direction = mesh.uniforms.find("uSpotLight[" + std::to_string(i) + "].direction");
		if (direction != mesh.uniforms.end()) {
			glUniform3fv(direction->second.location, 1, glm::value_ptr(lights.spotLights[i].direction));
		}
		auto cutOff = mesh.uniforms.find("uSpotLight[" + std::to_string(i) + "].cutOff");
		if (cutOff != mesh.uniforms.end()) {
			glUniform1f(cutOff->second.location, lights.spotLights[i].cutOff);
		}
		auto outerCutOff = mesh.uniforms.find("uSpotLight[" + std::to_string(i) + "].outerCutOff");
		if (outerCutOff != mesh.uniforms.end()) {
			glUniform1f(outerCutOff->second.location, lights.spotLights[i].outerCutOff);
		}
		auto color = mesh.uniforms.find("uSpotLight[" + std::to_string(i) + "].color");
		if (color != mesh.uniforms.end()) {
			glUniform4fv(color->second.location, 1, glm::value_ptr(lights.spotLights[i].color));
		}
		auto intensity = mesh.uniforms.find("uSpotLight[" + std::to_string(i) + "].intensity");
		if (intensity != mesh.uniforms.end()) {
			glUniform1f(intensity->second.location, lights.spotLights[i].intensity);
		}
	}

	for (int i = 0; i < lights.pointLights.size(); ++i) {
		auto pos = mesh.uniforms.find("uPointLight[" + std::to_string(i) + "].pos");
		if (pos != mesh.uniforms.end()) {
			glUniform3fv(pos->second.location, 1, glm::value_ptr(lights.pointLights[i].pos));
		}
		auto color = mesh.uniforms.find("uPointLight[" + std::to_string(i) + "].color");
		if (color != mesh.uniforms.end()) {
			glUniform4fv(color->second.location, 1, glm::value_ptr(lights.pointLights[i].color));
		}
		auto intensity = mesh.uniforms.find("uPointLight[" + std::to_string(i) + "].intensity");
		if (intensity != mesh.uniforms.end()) {
			glUniform1f(intensity->second.location, lights.pointLights[i].intensity);
		}
		auto constant = mesh.uniforms.find("uPointLight[" + std::to_string(i) + "].constant");
		if (constant != mesh.uniforms.end()) {
			glUniform1f(constant->second.location, lights.pointLights[i].constant);
		}
		auto linear = mesh.uniforms.find("uPointLight[" + std::to_string(i) + "].linear");
		if (linear != mesh.uniforms.end()) {
			glUniform1f(linear->second.location, lights.pointLights[i].linear);
		}
		auto quadratic = mesh.uniforms.find("uPointLight[" + std::to_string(i) + "].quadratic");
		if (quadratic != mesh.uniforms.end()) {
			glUniform1f(quadratic->second.location, lights.pointLights[i].quadratic);
		}
	}

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
}

void ModelRenderer::render(const Mesh& mesh, Camera* camera, const Lights& lights) const {
	glViewport(camera->viewport.x, camera->viewport.y, camera->viewport.z, camera->viewport.w);

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// bind program
	glUseProgram(mesh.program);
	// set vertex data
	glBindVertexArray(mesh.vao);
	// update uniform
	updateUniformsBasic(mesh,camera,lights);


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