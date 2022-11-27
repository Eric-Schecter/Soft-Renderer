#pragma once

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <memory>

#include "Shader.h"
#include "Material.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Vertex.h"
#include "DepthTexture.h"

class PhongShader : public Shader
{
public:
	glm::vec4 vertex(int vertexIndex,int index) override;
	bool fragment(const glm::vec3& bar, glm::vec4& gl_fragment, const std::vector<glm::vec4>& gl_FragCoord) override;

	// uniforms
	glm::mat4 u_projection = glm::mat4(1.f);
	glm::mat4 u_view = glm::mat4(1.f);
	glm::mat4 u_model = glm::mat4(1.f);
	glm::mat3 u_normalMatrix = glm::mat3(1.f);
	glm::vec3 u_cameraPos = glm::vec3(0.f);
	Material* u_material = nullptr;
	std::vector<AmbientLight> u_ambientLights;
	std::vector<DirectionalLight> u_directionalLights;
	std::vector<PointLight> u_pointLights;
	std::vector<SpotLight> u_spotLights;

	// varyings
	glm::vec2 v_uv[3];
	glm::mat3 v_TBN[3];
	glm::vec3 v_position[3];
	glm::vec4 v_lightSpacePos[3];

	// attributes
	std::vector<Vertex> vertices;

	// output
	glm::vec4 gl_position = glm::vec4(0.f);

private:
	float calcDiffuseLight(const glm::vec3& normal, const glm::vec3& lightDir);
	float calcSpecularLight(const glm::vec3& viewDir, const glm::vec3& normal, const glm::vec3& lightDir);
	glm::vec3 calcColor(const glm::vec3& viewDir, const glm::vec3& normal, const glm::vec3& lightDir, const glm::vec3& color, float intensity);
	glm::vec3 calcAmbientLight(const AmbientLight& ambientLight);
	glm::vec3 calcDirectionalLight(const glm::vec3& viewDir, const glm::vec3& normal, const DirectionalLight& light,const glm::vec4& lightSpacePos);
	glm::vec3 calcPointLight(const glm::vec3& position, const glm::vec3& viewDir, const glm::vec3& normal, const PointLight& light);
	glm::vec3 calcSpotLight(const glm::vec3& position, const glm::vec3& viewDir, const glm::vec3& normal,
		const SpotLight& light, const glm::vec4& lightSpacePos);
	float computeShadowRatio(const std::shared_ptr<DepthTexture> shadowmap, const glm::vec4& lightSpacePos,
		const glm::vec3& normal, const glm::vec3& lightDir);
};

