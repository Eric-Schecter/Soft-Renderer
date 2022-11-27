#pragma once

#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "Material.h"
#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader {
public:
	glm::vec4 vertex( // default output position for interplation
		const glm::vec3& a_position,
		const glm::vec2& a_uv,
		const glm::vec3& a_tangent, 
		const glm::vec3& a_normal, 
		const glm::vec3& a_biTangent,
		int index
	);
	bool fragment(const glm::vec3& bar,glm::vec4& gl_fragment);

	glm::mat4 u_projection;
	glm::mat4 u_view;
	glm::mat4 u_model;
	glm::mat3 u_normalMatrix;
	glm::vec3 u_cameraPos;
	Material* u_material;

	AmbientLight* u_ambientLight = nullptr;
	DirectionalLight* u_directionalLight = nullptr;
	PointLight* u_pointLight = nullptr;
	SpotLight* u_spotLight = nullptr;

	glm::vec4 gl_position;
	glm::vec2 v_uv[3];
	glm::mat3 v_TBN[3];
	glm::vec3 v_position[3];

private:
	float calcDiffuseLight(const glm::vec3& normal, const glm::vec3& lightDir);
	float calcSpecularLight(const glm::vec3& viewDir, const glm::vec3& normal, const glm::vec3& lightDir);
	glm::vec4 calcColor(const glm::vec3& viewDir, const glm::vec3& normal, const glm::vec3& lightDir, const glm::vec4& color, float intensity);
	glm::vec4 calcAmbientLight();
	glm::vec4 calcDirectionalLight(const glm::vec3& viewDir, const glm::vec3& normal, DirectionalLight* light);
	glm::vec4 calcPointLight(const glm::vec3& position, const glm::vec3& viewDir, const glm::vec3& normal, PointLight* light);
	glm::vec4 calcSpotLight(const glm::vec3& position,const glm::vec3& viewDir, const glm::vec3& normal, SpotLight* light);
};