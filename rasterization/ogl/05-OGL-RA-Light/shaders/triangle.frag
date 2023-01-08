#version 460

struct DirectionalLight {
    vec4 color;
    vec3 direction;
    float intensity;
};

struct PointLight {
    vec4 color;
    vec3 pos;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

struct SpotLight {
    vec4 color;
    vec3 pos;
    float cutOff;
    vec3 direction;
    float outerCutOff;
    float intensity;
};

struct AmbientLight {
    vec4 color;
    float intensity;
};

struct Material {
    sampler2D diffuseMap;
    sampler2D normalMap;
    float specular;
    int shininess;
};

#define PI 3.1415926538
#define DIRECTIONAL_LIGHT_COUNT 1
#define AMBIENT_LIGHT_COUNT 1
#define POINT_LIGHT_COUNT 1
#define SPOT_LIGHT_COUNT 1

uniform DirectionalLight uDirectionalLight[DIRECTIONAL_LIGHT_COUNT];
uniform AmbientLight uAmbientLight[AMBIENT_LIGHT_COUNT];
uniform PointLight uPointLight[POINT_LIGHT_COUNT];
uniform SpotLight uSpotLight[SPOT_LIGHT_COUNT];
uniform vec3 uCameraPos;
uniform Material uMaterial;

in vec3 vNormal;
in vec3 vPosition;
in vec2 vUv;
in mat3 vTBN;

out vec4 fColor;

float calcDiffuseLight(vec3 normal,vec3 lightDir){
    return clamp(dot(-lightDir, normal),0.f,1.f);
}

float calcSpecularLight(vec3 viewDir,vec3 normal,vec3 lightDir){
    vec3 halfwayDir = normalize(lightDir-viewDir); //Blinn-Phong
    return pow(max(dot(normal,halfwayDir),0.),uMaterial.shininess);
}

vec4 calcColor(vec3 viewDir,vec3 normal,vec3 lightDir,vec4 color,float intensity){
    vec4 light = vec4(0.f);
    light+=calcDiffuseLight(normal,lightDir) * color;
    light+=calcSpecularLight(viewDir,normal,lightDir) * uMaterial.specular * color;
    return light * intensity;
}

vec4 calcDirectionalLight(vec3 viewDir,vec3 normal,DirectionalLight light){
    return calcColor(viewDir,normal,normalize(light.direction),light.color,light.intensity);
}

vec4 calcPointLight(vec3 viewDir,vec3 normal,PointLight light){
    vec3 lightDir = normalize(vPosition - light.pos);
    vec4 color = calcColor(viewDir,normal,lightDir,light.color,light.intensity);
    float dis = length(light.pos - vPosition);
    float attenuation = 1.0f / (light.constant + light.linear * dis +  light.quadratic * (dis * dis));
    return color * attenuation;
}

vec4 calcSpotLight(vec3 viewDir,vec3 normal,SpotLight light){
    vec4 color = vec4(0.);
    vec3 lightDir = normalize(vPosition-light.pos);
    float thetaLight = dot(lightDir, normalize(light.direction));
    float thetaNormal = dot(-lightDir,normal);
    float epsilon = light.cutOff - light.outerCutOff;
    float index = clamp((thetaLight - light.outerCutOff) / epsilon, 0.0f, 1.0f); 
    if(thetaLight > light.cutOff && thetaNormal > 0.f){
        color += calcColor(viewDir,normal,lightDir,light.color,light.intensity) * index;
    }
    return color;
}

void main() {
    vec4 color = texture(uMaterial.diffuseMap,vUv);
    vec3 normal = texture(uMaterial.normalMap,vUv).xyz;
    normal = normalize(normal * 2.f - vec3(1.f));
    normal = normalize(vTBN * normal);

    vec3 viewDir = normalize(vPosition-uCameraPos);

    vec4 lightColor = vec4(0.f);

    // ambient lights
    for(int i=0;i<AMBIENT_LIGHT_COUNT;++i){
        lightColor += uAmbientLight[i].color * uAmbientLight[i].intensity;
    };

    // point lights
    for(int i=0;i<POINT_LIGHT_COUNT;++i){
        lightColor += calcPointLight(viewDir,normal,uPointLight[i]);
    };

    // directional lights
    for(int i=0;i<DIRECTIONAL_LIGHT_COUNT;++i){
        lightColor += calcDirectionalLight(viewDir,normal,uDirectionalLight[i]);
    };

    // spot lights
    for(int i=0;i<SPOT_LIGHT_COUNT;++i){
        lightColor += calcSpotLight(viewDir,normal,uSpotLight[i]);
    };

    fColor = color * lightColor;
} 
