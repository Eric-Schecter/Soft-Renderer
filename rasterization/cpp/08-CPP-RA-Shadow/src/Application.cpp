#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec4.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <memory>

#include "Vertex.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

Application::Application() {
	initWindow();
	initRenderer();
	initScene();
}

Application::~Application() {
	release(pipeline);
	release(window);
	release(camera);
	release(mesh);
	release(phongShader);
	release(depthShader);
}

void Application::update() {
	// shadow pass
	pipeline->setViewport(0, 0, window->width, window->height);
	pipeline->setFramebuffer(nullptr, nullptr, window->width, window->height);
	pipeline->clear(BUFFER_BIT::COLOR_BUFFER_BIT | BUFFER_BIT::DEPTH_BUFFER_BIT);
	pipeline->setShader(depthShader);
	pipeline->render(mesh);

	// color pass
	phongShader->u_spotLights[0].shadowmap->m_data = pipeline->getZBuffer();
	pipeline->setFramebuffer(static_cast<uint32_t*>(window->pixels), nullptr, window->width, window->height);
	pipeline->clear(BUFFER_BIT::COLOR_BUFFER_BIT | BUFFER_BIT::DEPTH_BUFFER_BIT);
	pipeline->setShader(phongShader);
	pipeline->render(mesh);
}

void Application::run() {
	while (window->isRunning) {
		window->handleEvents();
		window->init();
		update();
		window->render();
	}
}

void Application::initWindow() {
	window = new Window();
}

void Application::initRenderer() {
	pipeline = new Pipeline();
}

Mesh* Application::loadModel(const std::string& objPath,std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, Material* material) {
	Assimp::Importer importer;
	const aiScene* pScene = importer.ReadFile(objPath,
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);

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

	material->shininess = 16;
	material->specular = 1.f;

	for (unsigned int i = 0, j = 0; i < pScene->mNumMaterials; ++i) {
		const aiMaterial* pMaterial = pScene->mMaterials[i];
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullPath = MODEL_FOLDER_PATH + path.data;
				unsigned char* image_data = stbi_load(fullPath.c_str(), &width, &height, &bpp, 0);
				material->diffuseMap = new ColorTexture(width, height, image_data);
			}
		}

		if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0) {
			if (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				std::string fullPath = MODEL_FOLDER_PATH + path.data;
				unsigned char* image_data = stbi_load(fullPath.c_str(), &width, &height, &bpp, 0);
				material->normalMap = new ColorTexture(width, height, image_data);
			}
		}
	}

	glm::mat4 modelMatrix = glm::mat4(1.f);

	return new Mesh{
		vertices,
		indices,
		material,
		modelMatrix
	};
}

void Application::initScene() {
	// load model
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	Material* material = new Material();
	const std::string MODEL_PATH = "./models/african_head/african_head.obj";
	mesh = loadModel(MODEL_PATH, vertices, indices, material);

	// setup camera
	const float fov = glm::radians(60.f);
	const float aspect = static_cast<float>(window->width) / window->height;
	const float near = 0.1f;
	const float far = 100.f;
	camera = new Camera();
	camera->pos = glm::vec3(1.f, 1.f, 2.f);
	camera
		->setView(glm::vec3(0.f, 0.f, 0.f))
		->setProjection(fov, aspect, near, far);

	// setup lights
	lights->ambientLights.push_back(AmbientLight{ glm::vec3(1.f),.1f });


	//glm::vec3 pos1(4.f, 4.f, -4.f);
	//float size = 100.f;
	//auto lightCamera1 = new Camera();
	//lightCamera1->pos = pos1;
	//lightCamera1
	//	->setView(glm::vec3(0.f, 0.f, 0.f))
	//	->setOrthographic(-size,size,size,-size,-100.f,100.f);

	//auto depthTexture1 = std::make_shared<DepthTexture>(window->width, window->height, nullptr);

	//DirectionalLight directionalLight{
	//	glm::normalize(glm::vec3(-1.f, -1.f,0.f)),
	//	glm::vec4(1.f, 1.f, 1.f, 1.f),
	//	1.f,
	//	lightCamera1->getVP(),
	//	depthTexture1
	//};
	//lights->directionalLights.push_back(directionalLight);


	//lights->pointLights.push_back(PointLight{
	//	glm::vec4(1.f, 1.f, 1.f, 1.f),
	//	glm::vec3(0.f,0.f,2.f),
	//	1.f,
	//	1.f,
	//	0.09f,
	//	.032f
	//});

	glm::vec3 pos(4.f, 4.f, -4.f);
	auto lightCamera = new Camera();
	lightCamera->pos = pos;
	lightCamera
		->setView(glm::vec3(0.f, 0.f, 0.f))
		->setProjection(fov, aspect, near, far);

	auto depthTexture = std::make_shared<DepthTexture>(window->width, window->height, nullptr);
	SpotLight spotLight{
		glm::vec3(1.f),
		pos,
		glm::normalize(glm::vec3(0.f, 0.f, 0.f) - pos),
		1.f,
		glm::cos(glm::radians(12.5f)),
		glm::cos(glm::radians(17.5f)),
		lightCamera->getVP(),
		depthTexture
	};

	lights->spotLights.push_back(spotLight);

	// setup shaders
	depthShader->u_projection = lightCamera->projection;
	depthShader->u_view = lightCamera->view;
	depthShader->u_model = mesh->modelMatrix;
	depthShader->vertices = mesh->vertices;

	phongShader->u_projection = camera->projection;
	phongShader->u_view = camera->view;
	phongShader->u_cameraPos = camera->pos;
	phongShader->u_model = mesh->modelMatrix;
	phongShader->u_material = mesh->material;
	auto normalMatrix = glm::transpose(glm::inverse(mesh->modelMatrix));
	phongShader->u_normalMatrix = normalMatrix;
	phongShader->u_ambientLights = lights->ambientLights;
	phongShader->u_directionalLights = lights->directionalLights;
	phongShader->u_pointLights = lights->pointLights;
	phongShader->u_spotLights = lights->spotLights;
	phongShader->vertices = mesh->vertices;
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}	