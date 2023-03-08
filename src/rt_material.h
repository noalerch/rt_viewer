//
// Created by noax on 3/8/23.
//

#ifndef RT_VIEWER_RT_MATERIAL_H
#define RT_VIEWER_RT_MATERIAL_H

#include <glm/glm.hpp>
#include "rt_ray.h"
#include "rt_hitable.h"

namespace rt
{

    class material {
        public :
            virtual bool scatter(
                const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
                ) const = 0;
    };


}

#endif //RT_VIEWER_RT_MATERIAL_H
