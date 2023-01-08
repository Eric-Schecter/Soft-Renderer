#include "Application.h"

#include <iostream>
#include <cstdint>
#include <glm/vec3.hpp>
#include <glm/gtx/transform.hpp>

#include "ModelRenderer.h"
#include "Vertex.h"
#include "Mesh.h"
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
	release(window);
}

void Application::initWindow() {
	window = new Window();
}

void Application::initRenderer() {
	renderer = std::make_shared<ModelRenderer>();
}

void Application::initScene() {
	// load camera
	const float fov = glm::radians(60.f);
	const float aspect = static_cast<float>(window->width) / window->height;
	const float near = 0.1f;
	const float far = 1000.f;
	camera = new Camera();
	camera->pos = glm::vec3(1.f, 1.f, 2.f);
	camera
		->setView(glm::vec3(0.f, 0.f, 0.f))
		->setProjection(fov, aspect, near, far)
		->setViewport(0, 0, window->width, window->height);
	
	// load lights
	SpotLight spotLight{
		glm::vec4(1.f, 1.f, 1.f, 1.f),
		glm::vec3(0.f, 10.f, 0.f),
		glm::cos(glm::radians(12.5f)),
		glm::normalize(glm::vec3(0.f, -1.f, 0.f)),
		glm::cos(glm::radians(17.5f)),
		.3f
	};
	lights.spotLights.push_back(spotLight);

	PointLight pointLight{
		glm::vec4(1.f, 1.f, 1.f, 1.f),
		glm::vec3(-1.f),
		1.f,
		.5f,
		.032f,
		0.09f,
	};
	lights.pointLights.push_back(pointLight);

	DirectionalLight directionalLight{
		glm::vec4(1.f, 1.f, 1.f, 1.f),
		glm::normalize(glm::vec3(-1.f, 1.f, 1.f)),
		.3f
	};
	lights.directionalLights.push_back(directionalLight);

	AmbientLight ambientLight{
		glm::vec4(1.f, 1.f, 1.f, 1.f),
		.3f
	};

	lights.ambientLights.push_back(ambientLight);

	// load mesh
	const std::string MODEL_PATH = "./models/african_head/african_head.obj";
	const std::string VERTEX_SHADER_PATH = "./shaders/triangle.vert";
	const std::string FRAGMENT_SHADER_PATH = "./shaders/triangle.frag";
	Mesh mesh;
	mesh.setupModel(MODEL_PATH);
	mesh.uMaterial.shininess = 16;
	mesh.uMaterial.specular = 1.f;
	mesh.setupProgram(
		VERTEX_SHADER_PATH,
		FRAGMENT_SHADER_PATH
	);
	mesh.setupUniforms(camera, lights);
	scene.push_back(mesh);
}

void Application::run() {
	window->render(renderer, scene, camera, lights);
}

void Application::release(void* pointer) {
	delete pointer;
	pointer = nullptr;
}