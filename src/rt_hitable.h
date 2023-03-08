#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>

// #include "rt_extra.h"
#include <memory>

namespace rt {

class Material; // prevent circular references

struct HitRecord {
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    std::shared_ptr<rt::Material> material;
};

class Hitable {
public:
    virtual bool hit(const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
};

} // namespace rt
