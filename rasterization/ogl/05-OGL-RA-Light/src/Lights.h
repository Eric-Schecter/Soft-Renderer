#pragma once

#include <vector>

#include "AmbientLight.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"

struct Lights {
	std::vector<AmbientLight> ambientLights;
	std::vector<DirectionalLight> directionalLights;
	std::vector<SpotLight> spotLights;
	std::vector<PointLight> pointLights;
};