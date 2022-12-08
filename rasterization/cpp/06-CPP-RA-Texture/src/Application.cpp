#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stb_image.h>

#include "Vertex.h"

Application::Application() {
	initWindow();
	initRenderer();
	initScene();
}

Application::~Application() {
	release(renderer);
	release(window);
	release(camera);

}

void Application::run() {
	glm::mat4 modelMatrix = glm::mat4(1.f);
	//modelMatrix = glm::rotate(modelMatrix, glm::radians(180.f), glm::vec3(0.,1.,0.));

	while (window->isRunning) {
		window->handleEvents();
		if (!window->init()) {
			break;
		}

		//modelMatrix = glm::rotate(modelMatrix, glm::radians(1.f), glm::vec3(0.,1.,0.));
		//modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f, 0.f, 0.f));

		renderer->setFramebuffer(static_cast<uint32_t*>(window->pixels), window->width, window->height);
		renderer->clear();
		renderer->render(vertices,indices,camera, modelMatrix,diffuse,normal);

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
	// load model
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

	// load texture
	const std::string MODEL_FOLDER_PATH = "models/african_head/";
	aiString path;
	int width = 0;
	int height = 0;
	int bpp = 0;

	for (unsigned int i = 0, j = 0; i < pScene->mNumMaterials; ++i) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullPath = MODEL_FOLDER_PATH + path.data;
				unsigned char* image_data = stbi_load(fullPath.c_str(), &width, &height, &bpp, 0);
				diffuse = new Image(width, height, bpp, image_data);
			}
		}

		if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
			if (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullPath = MODEL_FOLDER_PATH + path.data;
				unsigned char* image_data = stbi_load(fullPath.c_str(), &width, &height, &bpp, 0);
				normal = new Image(width, height, bpp, image_data);
			}
		}
	}

	// setup camera
	const float fov = glm::radians(60.f);
	const float aspect = static_cast<float>(window->width) / window->height;
	const float near = 0.1f;
	const float far = 1000.f;
	camera = new Camera();
	camera->pos = glm::vec3(1.f, 1.f, 2.f);
	camera
		->setView( glm::vec3(0.f, 0.f, 0.f))
		->setProjection(fov, aspect, near, far)
		->setViewport(0,0, window->width, window->height);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}	