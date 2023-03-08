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

    // Does not work properly, likely due to issue with front face
    class Dielectric : public Material {
    public:
        Dielectric(float index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
                const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
        ) const override {
            attenuation = glm::vec3(1.0, 1.0, 1.0);
            // correctness of below line disputed
            double refraction_ratio = (0 > glm::dot(r_in.direction(), rec.normal)) ? (1.0 / ir) : ir; //assuming front face is normal

            glm::vec3 unit_direction = glm::normalize(r_in.direction());
            float cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
            float sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            glm::vec3 direction;

            if (cannot_refract)
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, refraction_ratio);

            scattered = Ray(rec.p, direction);
            return true;
        }

        public:
            double ir; // Index of Refraction
    };

}

#endif //RT_VIEWER_RT_MATERIAL_H
