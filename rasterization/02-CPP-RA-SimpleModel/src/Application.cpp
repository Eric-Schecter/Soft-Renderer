#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "Vertex.h"

Application::Application() {
	initWindow();
	initRenderer();
	initScene();
}

Application::~Application() {
	release(renderer);
	release(window);
}

void Application::run() {
	while (window->isRunning) {
		window->handleEvents();
		if (!window->init()) {
			break;
		}

		renderer->setFramebuffer(static_cast<uint32_t*>(window->pixels), window->width, window->height);
		renderer->clear();

		renderer->render(vertices,indices,glm::vec4(1.f));

		if (!window->render()) {
			break;
		}
	}
}

void Application::initWindow() {
	window = new Window();
}

void Application::initRenderer() {
	renderer = new Renderer();
}

void Application::initScene() {
	const std::string MODEL_PATH = "./models/african_head/african_head.obj";
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(MODEL_PATH,
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

}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}	