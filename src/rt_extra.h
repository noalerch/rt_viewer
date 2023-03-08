//
// Created by noax on 3/8/23.
//

#pragma once

#include <glm/glm.hpp>
#include <cstdlib>
#include <random>

namespace rt
{

// taken from RTOW
    float random_float() {
        static std::uniform_real_distribution<float> distribution(0.0, 1.0);
        static std::mt19937 generator;
        return distribution(generator);
    }

    float random_float(float min, float max) {
        static std::uniform_real_distribution<float> distribution(min, max);
        static std::mt19937 generator;
        return distribution(generator);
    }

    glm::vec3 random_vector() {
        return glm::vec3(random_float(), random_float(), random_float());
    }

    glm::vec3 random_vector(float min, float max) {
        return glm::vec3(random_float(min, max), random_float(min, max), random_float(min, max));
    }

    glm::vec3 random_in_unit_sphere() {
        while (true) {
            auto p = random_vector(-1,1);
            if (pow(glm::length(p), 2) >= 1) continue;
            return p;
        }
    }

    bool near_zero(glm::vec3 vec) {
        const auto s = 1e-8;
        return (fabs(vec[0]) < s) && (fabs(vec[1]) < s) && (fabs(vec[2]) < s);
    }

    glm::vec3 reflect(const glm::vec3& v, const glm::vec3& n) {
        return v - 2 * glm::dot(v, n) * n;
    }



}

