#include "Mesh.h"

#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "./loadShaders.h"
#include "Vertex.h"
#include "ImageLoader.h"

void Mesh::setupModel(std::string path) {
	// vertex data
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(path,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	for (size_t i = 0; i < pScene->mNumMeshes; ++i) {
		const aiMesh* paiMesh = pScene->mMeshes[i];

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

		for (size_t i = 0; i < paiMesh->mNumVertices; ++i) {
			const aiVector3D* pPos = &(paiMesh->mVertices[i]);
			const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
			const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
			const aiVector3D* pTangent = paiMesh->mTangents == nullptr ? &Zero3D : &(paiMesh->mTangents[i]);

			auto pos = glm::vec3(pPos->x, pPos->y, pPos->z);
			auto normal = glm::vec3(pNormal->x, pNormal->y, pNormal->z);
			auto uv = glm::vec2(pTexCoord->x, 1.f - pTexCoord->y);
			auto tangent = glm::vec3(pTangent->x, pTangent->y, pTangent->z);
			auto biTangent = glm::cross(tangent, normal);

			Vertex v{ pos, normal, uv, tangent, biTangent };

			vertices.push_back(v);
		}

		for (size_t i = 0; i < paiMesh->mNumFaces; ++i) {
			const aiFace& face = paiMesh->mFaces[i];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

	}

	textures.push_back(Texture("uMaterial.diffuseMap", aiTextureType_DIFFUSE));
	textures.push_back(Texture("uMaterial.normalMap", aiTextureType_NORMALS));

	std::string folder = path.substr(0, path.find_last_of("/") + 1);

	aiString src;
	for (unsigned int i = 0, j = 0; i < pScene->mNumMaterials; ++i) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		for (size_t j = 0; j < textures.size(); ++j) {
			if (pMaterial->GetTextureCount(textures[j].type) > 0) {
				if (pMaterial->GetTexture(textures[j].type, 0, &src, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
					std::string fullPath = folder + src.data;
					textures[j].load(fullPath);
				}
			}
		}
	}

	// cpu -> buffer -> gpu
	// vao style
	//GLuint vbo;
	//GLuint ibo;
	//// init buffer
	//glGenVertexArrays(1, &vao);
	//glGenBuffers(1, &vbo);
	//glGenBuffers(1, &ibo);
	//// bind buffer
	//glBindVertexArray(vao);
	//glBindBuffer(GL_ARRAY_BUFFER, vbo);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	//// pass data from cpu to buffer
	//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	//// bind buffer to variable in gpu
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), 0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (void*)(sizeof(GLfloat) * 3));
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (void*)(sizeof(GLfloat) * 5));
	//glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (void*)(sizeof(GLfloat) * 8));
	//glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(GL_FLOAT), (void*)(sizeof(GLfloat) * 11));
	//// enable variable
	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);
	//glEnableVertexAttribArray(3);
	//glEnableVertexAttribArray(4);

	// DSA style
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
	glNamedBufferData(vbo, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glNamedBufferData(ibo, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	//glNamedBufferStorage(vbo, 18 * sizeof(GL_FLOAT), vertices, GL_DYNAMIC_STORAGE_BIT);
	// bind buffer to variable in gpu
	// glVertexArrayVertexBuffer + glVertexArrayAttribFormat = glVertexAttribPointer + vao bind
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(vao, ibo);

	glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
	glVertexArrayAttribFormat(vao, 2, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, uv));
	glVertexArrayAttribFormat(vao, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
	glVertexArrayAttribFormat(vao, 4, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, biTangent));

	glVertexArrayAttribBinding(vao, 0, 0);
	glVertexArrayAttribBinding(vao, 1, 0);
	glVertexArrayAttribBinding(vao, 2, 0);
	glVertexArrayAttribBinding(vao, 3, 0);
	glVertexArrayAttribBinding(vao, 4, 0);

	// enable variable
	// glEnableVertexArrayAttrib(DSA) = glEnableVertexAttribArray(VAO)
	glEnableVertexArrayAttrib(vao, 0);
	glEnableVertexArrayAttrib(vao, 1);
	glEnableVertexArrayAttrib(vao, 2);
	glEnableVertexArrayAttrib(vao, 3);
	glEnableVertexArrayAttrib(vao, 4);
}

void Mesh::setupProgram(std::string VERTEX_SHADER_PATH, std::string FRAGMENT_SHADER_PATH) {
	// program
	ShaderInfo shaders[] = {
	 {GL_VERTEX_SHADER,   VERTEX_SHADER_PATH.c_str()},
	 {GL_FRAGMENT_SHADER, FRAGMENT_SHADER_PATH.c_str()},
	 {GL_NONE,            nullptr}
	};

	program = LoadShaders(shaders);

	// pipeline
	//ShaderInfoPipeline shaders[] = {
	// {GL_VERTEX_SHADER,   GL_VERTEX_SHADER_BIT, VERTEX_SHADER_PATH.c_str(), &vertProg},
	// {GL_FRAGMENT_SHADER, GL_FRAGMENT_SHADER_BIT, FRAGMENT_SHADER_PATH.c_str(), &fragProg},
	// {GL_NONE,            GL_NONE, nullptr, 0}
	//};

	//pipeline = LoadShadersPipeline(shaders);
}

void Mesh::updateUniformsBasic(Camera* camera, const Lights& lights) const {
	auto uModelMatrix = uniforms.find("uModelMatrix");
	if (uModelMatrix != uniforms.end()) {
		glUniformMatrix4fv(uModelMatrix->second.location, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	}
	auto uViewMatrix = uniforms.find("uViewMatrix");
	if (uViewMatrix != uniforms.end()) {
		glUniformMatrix4fv(uViewMatrix->second.location, 1, GL_FALSE, glm::value_ptr(camera->view));
	}
	auto uProjectionMatrix = uniforms.find("uProjectionMatrix");
	if (uProjectionMatrix != uniforms.end()) {
		glUniformMatrix4fv(uProjectionMatrix->second.location, 1, GL_FALSE, glm::value_ptr(camera->projection));
	}
	auto uNormalMatrix = uniforms.find("uNormalMatrix");
	if (uNormalMatrix != uniforms.end()) {
		auto normalMatrix = glm::transpose(glm::inverse(modelMatrix));
		glUniformMatrix4fv(uNormalMatrix->second.location, 1, GL_FALSE, glm::value_ptr(normalMatrix));
	}
	auto specular = uniforms.find("uMaterial.specular");
	if (specular != uniforms.end()) {
		glUniform1f(specular->second.location, uMaterial.specular);
	}
	auto shininess = uniforms.find("uMaterial.shininess");
	if (shininess != uniforms.end()) {
		glUniform1i(shininess->second.location, uMaterial.shininess);
	}
	auto uCameraPos = uniforms.find("uCameraPos");
	if (uCameraPos != uniforms.end()) {
		glUniform3fv(uCameraPos->second.location, 1, glm::value_ptr(camera->pos));
	}

	for (int i = 0; i < lights.ambientLights.size(); ++i) {
		auto uAmbientLightColor = uniforms.find("uAmbientLight[" + std::to_string(i) + "].color");
		if (uAmbientLightColor != uniforms.end()) {
			glUniform4fv(uAmbientLightColor->second.location, 1, glm::value_ptr(lights.ambientLights[i].color));
		}
		auto uAmbientLightIntensity = uniforms.find("uAmbientLight[" + std::to_string(i) + "].intensity");
		if (uAmbientLightIntensity != uniforms.end()) {
			glUniform1f(uAmbientLightIntensity->second.location, lights.ambientLights[i].intensity);
		}
	}

	for (int i = 0; i < lights.directionalLights.size(); ++i) {
		auto direction = uniforms.find("uDirectionalLight[" + std::to_string(i) + "].direction");
		if (direction != uniforms.end()) {
			glUniform3fv(direction->second.location, 1, glm::value_ptr(lights.directionalLights[i].direction));
		}
		auto color = uniforms.find("uDirectionalLight[" + std::to_string(i) + "].color");
		if (color != uniforms.end()) {
			glUniform4fv(color->second.location, 1, glm::value_ptr(lights.directionalLights[i].color));
		}
		auto intensity = uniforms.find("uDirectionalLight[" + std::to_string(i) + "].intensity");
		if (intensity != uniforms.end()) {
			glUniform1f(intensity->second.location, lights.directionalLights[i].intensity);
		}
	}

	for (int i = 0; i < lights.spotLights.size(); ++i) {
		auto pos = uniforms.find("uSpotLight[" + std::to_string(i) + "].pos");
		if (pos != uniforms.end()) {
			glUniform3fv(pos->second.location, 1, glm::value_ptr(lights.spotLights[i].pos));
		}
		auto direction = uniforms.find("uSpotLight[" + std::to_string(i) + "].direction");
		if (direction != uniforms.end()) {
			glUniform3fv(direction->second.location, 1, glm::value_ptr(lights.spotLights[i].direction));
		}
		auto cutOff = uniforms.find("uSpotLight[" + std::to_string(i) + "].cutOff");
		if (cutOff != uniforms.end()) {
			glUniform1f(cutOff->second.location, lights.spotLights[i].cutOff);
		}
		auto outerCutOff = uniforms.find("uSpotLight[" + std::to_string(i) + "].outerCutOff");
		if (outerCutOff != uniforms.end()) {
			glUniform1f(outerCutOff->second.location, lights.spotLights[i].outerCutOff);
		}
		auto color = uniforms.find("uSpotLight[" + std::to_string(i) + "].color");
		if (color != uniforms.end()) {
			glUniform4fv(color->second.location, 1, glm::value_ptr(lights.spotLights[i].color));
		}
		auto intensity = uniforms.find("uSpotLight[" + std::to_string(i) + "].intensity");
		if (intensity != uniforms.end()) {
			glUniform1f(intensity->second.location, lights.spotLights[i].intensity);
		}
	}

	for (int i = 0; i < lights.pointLights.size(); ++i) {
		auto pos = uniforms.find("uPointLight[" + std::to_string(i) + "].pos");
		if (pos != uniforms.end()) {
			glUniform3fv(pos->second.location, 1, glm::value_ptr(lights.pointLights[i].pos));
		}
		auto color = uniforms.find("uPointLight[" + std::to_string(i) + "].color");
		if (color != uniforms.end()) {
			glUniform4fv(color->second.location, 1, glm::value_ptr(lights.pointLights[i].color));
		}
		auto intensity = uniforms.find("uPointLight[" + std::to_string(i) + "].intensity");
		if (intensity != uniforms.end()) {
			glUniform1f(intensity->second.location, lights.pointLights[i].intensity);
		}
		auto constant = uniforms.find("uPointLight[" + std::to_string(i) + "].constant");
		if (constant != uniforms.end()) {
			glUniform1f(constant->second.location, lights.pointLights[i].constant);
		}
		auto linear = uniforms.find("uPointLight[" + std::to_string(i) + "].linear");
		if (linear != uniforms.end()) {
			glUniform1f(linear->second.location, lights.pointLights[i].linear);
		}
		auto quadratic = uniforms.find("uPointLight[" + std::to_string(i) + "].quadratic");
		if (quadratic != uniforms.end()) {
			glUniform1f(quadratic->second.location, lights.pointLights[i].quadratic);
		}
	}

	// update texture
	for (size_t i = 0; i < textures.size(); ++i) {
		// vao style
		//glActiveTexture(GL_TEXTURE0 + i);
		//glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);

		// DSA style
		// glBindTextureUnit = glActiveTexture + glBindTexture
		glBindTextureUnit(i, textures[i].id);

		glUniform1i(textures[i].location, i);
	}
}

void Mesh::setupUniforms(Camera* camera, const Lights& lights) {
	// basic
	for (size_t j = 0; j < textures.size(); ++j) {
		textures[j].location = glGetUniformLocation(program, textures[j].name.c_str());
	}
	
	GLint uniform_count = 0;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

	if (uniform_count != 0) {
		GLint 	max_name_len = 0;
		glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
		auto uniform_name = std::make_unique<char[]>(max_name_len);

		GLsizei length = 0;
		GLsizei count = 0;
		GLenum 	type = GL_NONE;
		for (GLint i = 0; i < uniform_count; ++i)
		{
			glGetActiveUniform(program, i, max_name_len, &length, &count, &type, uniform_name.get());

			UniformInfo uniform_info = {};
			uniform_info.location = glGetUniformLocation(program, uniform_name.get());
			uniform_info.count = count;

			uniforms.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));
		}
	}
	glUseProgram(program);
	updateUniformsBasic(camera, lights);
	glUseProgram(0);

	// spread program
	//for (size_t j = 0; j < textures.size(); ++j) {
	//	textures[j].location = glGetUniformLocation(program, textures[j].name.c_str());
	//}

	//GLint uniform_count = 0;
	//glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

	//if (uniform_count != 0) {
	//	GLint 	max_name_len = 0;
	//	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
	//	auto uniform_name = std::make_unique<char[]>(max_name_len);

	//	GLsizei length = 0;
	//	GLsizei count = 0;
	//	GLenum 	type = GL_NONE;
	//	for (GLint i = 0; i < uniform_count; ++i)
	//	{
	//		glGetActiveUniform(program, i, max_name_len, &length, &count, &type, uniform_name.get());

	//		UniformInfo uniform_info = {};
	//		uniform_info.location = glGetUniformLocation(program, uniform_name.get());
	//		uniform_info.count = count;

	//		uniforms.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));
	//	}
	//}

	//GLint uniform_count = 0;
	//glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &uniform_count);

	//if (uniform_count != 0) {
	//	GLint 	max_name_len = 0;
	//	glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_len);
	//	auto uniform_name = std::make_unique<char[]>(max_name_len);

	//	GLsizei length = 0;
	//	GLsizei count = 0;
	//	GLenum 	type = GL_NONE;
	//	for (GLint i = 0; i < uniform_count; ++i)
	//	{
	//		glGetActiveUniform(program, i, max_name_len, &length, &count, &type, uniform_name.get());

	//		UniformInfo uniform_info = {};
	//		uniform_info.location = glGetUniformLocation(program, uniform_name.get());
	//		uniform_info.count = count;

	//		uniforms.emplace(std::make_pair(std::string(uniform_name.get(), length), uniform_info));
	//	}
	//}

	//glProgramUniformMatrix4fv(vertProg, uModelMatrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));
	//glProgramUniformMatrix4fv(vertProg, uViewMatrix, 1, GL_FALSE, glm::value_ptr(camera->view));
	//glProgramUniformMatrix4fv(vertProg, uProjectionMatrix, 1, GL_FALSE, glm::value_ptr(camera->projection));

	//// ubo style
	//GLuint uniformBlockIndex = glGetUniformBlockIndex(vertProg, "UniformBufferObject");
	//glUniformBlockBinding(vertProg, uniformBlockIndex, 0);

	//GLuint ubo;
	//// gen + bind + bufferdata
	//glGenBuffers(1, &ubo);
	//glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	//glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

	//// create + namedbufferdata
	//glCreateBuffers(1, &ubo);
	//glNamedBufferData(ubo, 3 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);

	//glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 3 * sizeof(glm::mat4));
	//glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(camera->view));
	//glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera->projection));
	//glBufferSubData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(modelMatrix));
}