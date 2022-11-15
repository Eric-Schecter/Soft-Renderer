#include "Shader.h"

#include <algorithm>

template <typename T> T interpolate(const glm::vec3 bary, T x[]) {
	return bary.x * x[0] + bary.y * x[1] + bary.z * x[2];
}

glm::vec4 Shader::vertex(const glm::vec3& a_position,const glm::vec2& a_uv,
	const glm::vec3& a_tangent, const glm::vec3& a_normal, const glm::vec3& a_biTangent,int index) {
	v_uv[index] = a_uv;
	v_position[index] = glm::vec3(u_model * glm::vec4(a_position, 1.f)); // world space

	glm::vec3 T = glm::vec3(glm::normalize(u_normalMatrix * glm::vec4(a_tangent,1.f)));
	glm::vec3 N = glm::vec3(glm::normalize(u_normalMatrix * glm::vec4(a_normal,1.f)));
	glm::vec3 B = glm::vec3(glm::normalize(u_normalMatrix * glm::vec4(a_biTangent,1.f)));

	v_TBN[index] = glm::mat3(T, B, N);

	return u_projection * u_view * u_model * glm::vec4(a_position, 1.f);
}

bool Shader::fragment(const glm::vec3& bary, glm::vec4& gl_fragment) {
	// interpolation
	glm::vec2 uv = interpolate(bary, v_uv);
	glm::vec3 position = interpolate(bary, v_position);
	glm::mat3 TBN = interpolate(bary, v_TBN);

	glm::vec4 color = u_material->diffuseMap->sample(uv);
	glm::vec3 viewDir = glm::normalize(position - u_cameraPos);

	glm::vec3 normal = glm::vec3(u_material->normalMap->sample(uv));
	normal = glm::normalize(normal * 2.f - glm::vec3(1.f));
	normal = glm::normalize(TBN * normal);

	glm::vec4 lightColor(0.f);
	lightColor += calcAmbientLight();
	lightColor += calcDirectionalLight(viewDir,normal,u_directionalLight);
	lightColor += calcPointLight(position,viewDir,normal,u_pointLight);
	lightColor += calcSpotLight(position,viewDir,normal,u_spotLight);

	gl_fragment = glm::clamp(lightColor * color, 0.f, 1.f);

	return false;
}

float Shader::calcDiffuseLight(const glm::vec3& normal, const glm::vec3& lightDir) {
	return glm::clamp(dot(-lightDir, normal), 0.f, 1.f);
}

float Shader::calcSpecularLight(const glm::vec3& viewDir, const glm::vec3& normal, const glm::vec3& lightDir) {
	glm::vec3 halfwayDir = glm::normalize(lightDir - viewDir); //Blinn-Phong
	return std::pow(std::max(glm::dot(normal, halfwayDir), 0.f), u_material->shininess);
}

glm::vec4 Shader::calcColor(const glm::vec3& viewDir, const glm::vec3& normal, const glm::vec3& lightDir, const glm::vec4& color, float intensity){
	glm::vec4 light = glm::vec4(0.);
	light += calcDiffuseLight(normal, lightDir) * color;
	light += calcSpecularLight(viewDir, normal, lightDir) * u_material->specular * color;
	return light * intensity;
}

glm::vec4 Shader::calcAmbientLight() {
	return u_ambientLight->color* u_ambientLight->intensity;
}

glm::vec4 Shader::calcDirectionalLight(const glm::vec3& viewDir, const glm::vec3& normal, DirectionalLight* light) {
	return calcColor(viewDir, normal, glm::normalize(light->direction), light->color, light->intensity);
}

glm::vec4 Shader::calcPointLight(const glm::vec3& position,const glm::vec3& viewDir, const glm::vec3& normal, PointLight* light) {
	glm::vec3 lightDir = glm::normalize(position - light->pos);
	glm::vec4 color = calcColor(viewDir, normal, lightDir, light->color, light->intensity);
	float dis = glm::length(light->pos - position);
	float attenuation = 1.0f / (light->constant + light->linear * dis + light->quadratic * (dis * dis));
	return color * attenuation;
}

glm::vec4 Shader::calcSpotLight(const glm::vec3& position,const glm::vec3& viewDir,const glm::vec3& normal, SpotLight* light) {
	glm::vec4 color = glm::vec4(0.);
	glm::vec3 lightDir = glm::normalize(position - light->pos);
	float thetaLight = glm::dot(lightDir, glm::normalize(light->direction));
	float thetaNormal = dot(-lightDir, normal);
	float epsilon = light->cutOff - light->outerCutOff;
	float index = glm::clamp((thetaLight - light->outerCutOff) / epsilon, 0.0f, 1.0f);
	if (thetaLight > light->cutOff && thetaNormal > 0) {
		color += calcColor(viewDir, normal, lightDir, light->color, light->intensity) * index;
	}
	return color;
}
