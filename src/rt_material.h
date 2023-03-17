//
// Created by noax on 3/8/23.
//

#ifndef RT_VIEWER_RT_MATERIAL_H
#define RT_VIEWER_RT_MATERIAL_H

#include <glm/glm.hpp>
#include "rt_ray.h"
#include "rt_extra.h"
#include "rt_hitable.h"

namespace rt
{

    class Material {
        public :
            virtual bool scatter(
                const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
                ) const = 0;
    };


    class Lambertian : public Material {
        public :
            // TODO: no color?
            Lambertian(const glm::vec3& a) : albedo(a) {}

            virtual bool scatter(
                const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
            ) const override {
                auto scatter_direction = rec.normal + glm::normalize(random_vector());

                if (near_zero(scatter_direction)) {
                    scatter_direction = rec.normal;
                }

                scattered = Ray(rec.p, scatter_direction);
                attenuation = albedo;
                return true;
            }

        public :
            glm::vec3 albedo;

    };

    class Metal : public Material {
        public :
            Metal(const glm::vec3& a) : albedo(a) {};

            virtual bool scatter(
                    const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
            ) const override {
                glm::vec3 reflected = reflect(glm::normalize(r_in.direction()), rec.normal);
                scattered = Ray(rec.p, reflected);
                attenuation = albedo;
                return glm::dot(scattered.direction(), rec.normal) > 0;

            }
            glm::vec3 albedo;
    };
}

#endif //RT_VIEWER_RT_MATERIAL_H
