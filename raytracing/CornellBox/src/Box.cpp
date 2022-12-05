#include "Box.h"

Box::Box(const glm::vec3& bmin, const glm::vec3& bmax, const std::shared_ptr<Material>& _material): material(_material) {
    bounds[0] = bmin;
    bounds[1] = bmax;
}

// compare with min/max point to get normal
glm::vec3 Box::getNormal(const glm::vec3& p) {
    float epsilon = 0.001f;
    if (std::abs(p.x-bounds[0].x) < epsilon) {
        return glm::vec3(-1,0,0);
    }
    else if (std::abs(p.x - bounds[1].x) < epsilon) {
        return glm::vec3(1, 0, 0);
    }
    else if (std::abs(p.y - bounds[0].y) < epsilon) {
        return glm::vec3(0, -1, 0);
    }
    else if (std::abs(p.y - bounds[1].y) < epsilon) {
        return glm::vec3(0, 1, 0);
    }
    else if (std::abs(p.z - bounds[0].z) < epsilon) {
        return glm::vec3(0, 0, 1);
    }
    else {
        return glm::vec3(0, 0, -1);
    }
}

glm::vec2 Box::getUV(const glm::vec3& p) {
    return glm::vec3(0.f); // not implemented
}

bool Box::hit(const Ray& ray, Record& record) {
    // get tmin,tmax x.y.z compoent
    // multiple invdir would get the length of t x/y/z component, which is also the length of ray
    // get the min component, then get the min length of ray
    float txmin = (bounds[ray.sign[0]].x - ray.origin.x) * ray.invdir.x;
    float txmax = (bounds[1 - ray.sign[0]].x - ray.origin.x) * ray.invdir.x;
    float tymin = (bounds[ray.sign[1]].y - ray.origin.y) * ray.invdir.y;
    float tymax = (bounds[1 - ray.sign[1]].y - ray.origin.y) * ray.invdir.y;
    float tzmin = (bounds[ray.sign[2]].z - ray.origin.z) * ray.invdir.z;
    float tzmax = (bounds[1 - ray.sign[2]].z - ray.origin.z) * ray.invdir.z;
    float tmin = txmin;
    float tmax = txmax;

    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }

    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }

    if (tzmin > tmin) {
        tmin = tzmin;
    }
    if (tzmax < tmax) {
        tmax = tzmax;
    }

    float root = tmin > 0.f ? tmin : tmax;
    if (root > record.t) {
        return false;
    }

    record.isHit = true;
    record.t = root;
    record.pos = ray.at(root);
    glm::vec3 normal = getNormal(record.pos);
    record.isFrontFace = checkFrontFace(ray.direction, normal);
    record.normal = record.isFrontFace ? normal : -normal;
    record.material = material;
    record.uv = getUV(normal);

    return true;
}