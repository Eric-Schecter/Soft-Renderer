#include "PhongShader.h"

glm::vec4 PhongShader::vertex(int vertexIndex, int index) {
	auto a_uv = vertices[vertexIndex].uv;
	auto a_position = vertices[vertexIndex].position;
	auto a_tangent = vertices[vertexIndex].tangent;
	auto a_normal = vertices[vertexIndex].normal;
	auto a_biTangent = vertices[vertexIndex].biTangent;

	v_uv[index] = a_uv;
	v_position[index] = glm::vec3(u_model * glm::vec4(a_position, 1.f)); // world space
	v_lightSpacePos[index] = u_spotLights[0].shadowMatrix * u_model * glm::vec4(a_position, 1.f); // consider one light for now

	glm::vec3 T = glm::vec3(glm::normalize(u_normalMatrix * glm::vec4(a_tangent, 1.f)));
	glm::vec3 N = glm::vec3(glm::normalize(u_normalMatrix * glm::vec4(a_normal, 1.f)));
	glm::vec3 B = glm::vec3(glm::normalize(u_normalMatrix * glm::vec4(a_biTangent, 1.f)));

	v_TBN[index] = glm::mat3(T, B, N);

	return u_projection * u_view * u_model * glm::vec4(a_position, 1.f);
}

float PhongShader::computeShadowRatio(const std::shared_ptr<DepthTexture> shadowmap,
	const glm::vec4& lightSpacePos,const glm::vec3& normal,const glm::vec3& lightDir) {
	glm::vec3 projCoords = glm::vec3(lightSpacePos / lightSpacePos.w);
	projCoords = projCoords * 0.5f + 0.5f;
	float currentDepth = projCoords.z;
	if (currentDepth > 1.f) {
		return 0.f;
	}
	float bias = std::max(0.002 * (1.0 - dot(normal, -lightDir)), 0.001);
	float cloestDepth = shadowmap->sample(glm::vec2(projCoords.x, projCoords.y)).z;
	return cloestDepth - bias < currentDepth ? 1.f : 0.f;
}

bool PhongShader::fragment(const glm::vec3& bary, glm::vec4& gl_fragment, const std::vector<glm::vec4>& gl_FragCoord) {
	// interpolation
	glm::vec2 uv = interpolate(bary, v_uv);
	glm::vec3 position = interpolate(bary, v_position);
	glm::mat3 TBN = interpolate(bary, v_TBN);
	glm::vec4 lightSpacePos = interpolate(bary, v_lightSpacePos);

	glm::vec4 color = u_material->diffuseMap->sample(uv);
	glm::vec3 viewDir = glm::normalize(position - u_cameraPos);

	glm::vec3 normal = glm::vec3(u_material->normalMap->sample(uv));
	normal = glm::normalize(normal * 2.f - glm::vec3(1.f));
	normal = glm::normalize(TBN * normal);

	glm::vec3 lightColor(0.f);
	for (const auto &ambientLight : u_ambientLights) {
		//lightColor += calcAmbientLight(ambientLight);
	}

	for (const auto &directionalLight : u_directionalLights) {
		lightColor += calcDirectionalLight(viewDir, normal, directionalLight,lightSpacePos);
	}

	for (const auto &pointLight : u_pointLights) {
		lightColor += calcPointLight(position, viewDir, normal, pointLight);
	}

	for (const auto &spotLight : u_spotLights) {
		lightColor += calcSpotLight(position, viewDir, normal, spotLight, lightSpacePos);
	}

	gl_fragment = glm::clamp(glm::vec4(lightColor,1.f),0.f,1.f); // not consider alpha for now

	return false;
}

float PhongShader::calcDiffuseLight(const glm::vec3& normal, const glm::vec3& lightDir) {
	return glm::clamp(dot(-lightDir, normal), 0.f, 1.f);
}

float PhongShader::calcSpecularLight(const glm::vec3& viewDir, const glm::vec3& normal, const glm::vec3& lightDir) {
	glm::vec3 halfwayDir = glm::normalize(lightDir + viewDir); //Blinn-Phong
	return std::pow(std::max(glm::dot(normal, halfwayDir), 0.f), u_material->shininess);
}

glm::vec3 PhongShader::calcColor(const glm::vec3& viewDir, const glm::vec3& normal, const glm::vec3& lightDir, const glm::vec3& color, float intensity) {
	glm::vec3 light(0.f);
	light += calcDiffuseLight(normal, lightDir) * color;
	light += calcSpecularLight(viewDir, normal, lightDir) * u_material->specular * color;
	return light * intensity;
}

glm::vec3 PhongShader::calcAmbientLight(const AmbientLight& ambientLight) {
	return ambientLight.color * ambientLight.intensity;
}

glm::vec3 PhongShader::calcDirectionalLight(const glm::vec3& viewDir, const glm::vec3& normal, const DirectionalLight& light,const glm::vec4& lightSpacePos) {
	float shadow = computeShadowRatio(light.shadowmap,lightSpacePos, normal, light.direction);
	return calcColor(viewDir, normal, glm::normalize(light.direction), light.color, light.intensity) * shadow;
}

glm::vec3 PhongShader::calcPointLight(const glm::vec3& position, const glm::vec3& viewDir, const glm::vec3& normal, const PointLight& light) {
	glm::vec3 lightDir = glm::normalize(position - light.pos);
	glm::vec3 color = calcColor(viewDir, normal, lightDir, light.color, light.intensity);
	float dis = glm::length(light.pos - position);
	float attenuation = 1.0f / (light.constant + light.linear * dis + light.quadratic * (dis * dis));
	return color * attenuation;
}

glm::vec3 PhongShader::calcSpotLight(const glm::vec3& position, const glm::vec3& viewDir, const glm::vec3& normal,
	const SpotLight& light,const glm::vec4& lightSpacePos) {
	glm::vec3 color(0.f);
	glm::vec3 lightDir = glm::normalize(position - light.pos);
	float thetaLight = glm::dot(lightDir, glm::normalize(light.direction));
	float thetaNormal = dot(-lightDir, normal);
	float epsilon = light.cutOff - light.outerCutOff;
	float index = glm::clamp((thetaLight - light.outerCutOff) / epsilon, 0.0f, 1.0f);
	float shadow = computeShadowRatio(light.shadowmap, lightSpacePos, normal, lightDir);
	if (thetaLight > light.cutOff && thetaNormal > 0) {
		color += calcColor(viewDir, normal, lightDir, light.color, light.intensity) * index * shadow;
	}
;	return color;
}