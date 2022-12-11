#include "Mesh.h"

#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "./loadShaders.h"
#include "Vertex.h"

void Mesh:: loadModel(std::string path) {
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

	//// cpu -> buffer -> gpu
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
	//// reset
	//glBindVertexArray(0); // can be ignored

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
	glNamedBufferData(vbo, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	glNamedBufferData(ibo, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
	//glNamedBufferStorage(vbo, 18 * sizeof(GL_FLOAT), vertices, GL_DYNAMIC_STORAGE_BIT);
	// bind buffer to variable in gpu
	// glVertexArrayVertexBuffer + glVertexArrayAttribFormat = glVertexAttribPointer + vao bind
	glVertexArrayVertexBuffer(vao, 0, vbo, 0, sizeof(GL_FLOAT) * 14);
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
	ShaderInfo shaders[] = {
	 {GL_VERTEX_SHADER,   VERTEX_SHADER_PATH.c_str()},
	 {GL_FRAGMENT_SHADER, FRAGMENT_SHADER_PATH.c_str()},
	 {GL_NONE,            nullptr}
	};

	program = LoadShaders(shaders);
}