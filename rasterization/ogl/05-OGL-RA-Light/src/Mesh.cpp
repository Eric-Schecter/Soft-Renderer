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

	uMaterial.shininess = 16;
	uMaterial.specular = 1.f;

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

void Mesh::setupUniforms(Camera* camera) {
	// basic
	//uModelMatrix = glGetUniformLocation(program, "uModelMatrix");
	//uViewMatrix = glGetUniformLocation(program, "uViewMatrix");
	//uProjectionMatrix = glGetUniformLocation(program, "uProjectionMatrix");
	//uNormalMatrix = glGetUniformLocation(program, "uNormalMatrix");

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

	// spread program
	//uModelMatrix = glGetUniformLocation(vertProg, "uModelMatrix");
	//uViewMatrix = glGetUniformLocation(vertProg, "uViewMatrix");
	//uProjectionMatrix = glGetUniformLocation(vertProg, "uProjectionMatrix");

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