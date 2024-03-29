#include "rt_raytracing.h"
#include "rt_ray.h"
#include "rt_hitable.h"
#include "rt_sphere.h"
#include "rt_triangle.h"
#include "rt_box.h"
#include "rt_material.h"

#include "cg_utils2.h"  // Used for OBJ-mesh loading
#include <stdlib.h>     // Needed for drand48()
#include "rt_extra.h" // written by us!

namespace rt {

// Store scene (world) in a global variable for convenience
struct Scene {
    Sphere ground;
    std::vector<Sphere> spheres;
    std::vector<Box> boxes;
    std::vector<Triangle> mesh;
    Box mesh_bbox;
} g_scene;


bool hit_world(const Ray &r, float t_min, float t_max, HitRecord &rec)
{
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    if (g_scene.ground.hit(r, t_min, closest_so_far, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
    }
    for (int i = 0; i < g_scene.spheres.size(); ++i) {
        if (g_scene.spheres[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
    for (int i = 0; i < g_scene.boxes.size(); ++i) {
        if (g_scene.boxes[i].hit(r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }
	if (g_scene.mesh_bbox.hit(r, t_min, closest_so_far, temp_rec)) {
		for (int i = 0; i < g_scene.mesh.size(); ++i) {
			if (g_scene.mesh[i].hit(r, t_min, closest_so_far, temp_rec)) {
				hit_anything = true;
				closest_so_far = temp_rec.t;
				rec = temp_rec;
			}
		}
	}
    return hit_anything;
}

// This function should be called recursively (inside the function) for
// bouncing rays when you compute the lighting for materials, like this
//
// if (hit_world(...)) {
//     ...
//     return color(rtx, r_bounce, max_bounces - 1);
// }
//
// See Chapter 7 in the "Ray Tracing in a Weekend" book
glm::vec3 color(RTContext &rtx, const Ray &r, int max_bounces)
{
    if (max_bounces < 0) return glm::vec3(0.0f);

    HitRecord rec;
    // nonzero min prevents shadow acne
    if (hit_world(r, 0.001f, 9999.0f, rec)) {
        rec.normal = glm::normalize(rec.normal);  // Always normalise before use!
        if (rtx.show_normals) {
            return rec.normal * 0.5f + 0.5f;
        }

        // Implement lighting for materials here
       // Ray target;
        // glm::vec3 target = rec.p + rec.normal + glm::normalize(random_in_unit_sphere());
        // rt::Ray target_ray = rt::Ray(rec.p, target - rec.p);
        glm::vec3 attenuation = glm::vec3(1.0f);
        rt::Ray scattered;
        if (rec.material->scatter(r, rec, attenuation, scattered)) {
            return attenuation * color(rtx, scattered, max_bounces - 1);
        }
        else {
            return glm::vec3(0.0f, 0.0f, 0.0f);
        }
        // return 0.5f * color(rtx, target_ray, max_bounces - 1);

        // return glm::vec3(0.0f);
    }

    // If no hit, return sky color
    glm::vec3 unit_direction = glm::normalize(r.direction());
    float t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - t) * rtx.ground_color + t * rtx.sky_color;
}

void updateGroundColor(RTContext &rtx)
{
	g_scene.ground = Sphere(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.f, std::make_shared<Lambertian>(rtx.ground_color));
}

// MODIFY THIS FUNCTION!
void setupScene(RTContext &rtx, const char *filename)
{
    auto red = std::make_shared<Lambertian>(glm::vec3(1.0f,0.0f,0.0f));
    auto green = std::make_shared<Lambertian>(glm::vec3(0.0f,1.0f,0.0f));
    auto grass = std::make_shared<Lambertian>(glm::vec3(0.2f,0.9f,0.1f));
    auto blue = std::make_shared<Lambertian>(glm::vec3(0.0f,0.0f,1.0f));
    auto white = std::make_shared<Lambertian>(glm::vec3(1.0f,1.0f,1.0f));
    auto black = std::make_shared<Lambertian>(glm::vec3(1.0f,0.0f,0.0f));
    auto metal = std::make_shared<Metal>(glm::vec3(1.0f,1.0f,1.0f));
    auto metal_dark = std::make_shared<Metal>(glm::vec3(0.5f,0.5f,0.5f));
	auto ground_color = std::make_shared<Lambertian>(rtx.ground_color);

    g_scene.ground = Sphere(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, green);
    g_scene.spheres = {
        Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 0.2f, metal_dark),
        Sphere(glm::vec3(1.5f, 0.0f, 0.0f), 0.2f, green),
        Sphere(glm::vec3(-1.0f, 0.0f, 0.0f), 0.2f, white),
		Sphere(glm::vec3(-1.0f, 0.0f, 1.0f), 0.1f, blue),
		Sphere(glm::vec3(-1.0f, 0.0f, 1.5f), 0.1f, red),
        Sphere(glm::vec3(1.0f, 1.5f, 0.0f), 0.2f, white),
        Sphere(glm::vec3(1.0f, 1.5f, 2.0f), 0.1f, black),
        Sphere(glm::vec3(3.0f, 1.5f, 2.0f), 0.6f, metal),
    };
    //g_scene.boxes = {
    //    Box(glm::vec3(0.0f, -0.25f, 0.0f), glm::vec3(0.25f)),
    //    Box(glm::vec3(1.0f, -0.25f, 0.0f), glm::vec3(0.25f)),
    //    Box(glm::vec3(-1.0f, -0.25f, 0.0f), glm::vec3(0.25f)),
    //};

    cg::OBJMesh mesh;
    cg::objMeshLoad(mesh, filename);
    g_scene.mesh.clear();
	glm::vec3 ub; // bounding box upper bound
	glm::vec3 lb; // bounding box lower bound
    for (int i = 0; i < mesh.indices.size(); i += 3) {
        int i0 = mesh.indices[i + 0];
        int i1 = mesh.indices[i + 1];
        int i2 = mesh.indices[i + 2];
        glm::vec3 v0 = mesh.vertices[i0] + glm::vec3(0.0f, 0.135f, 0.0f);
        glm::vec3 v1 = mesh.vertices[i1] + glm::vec3(0.0f, 0.135f, 0.0f);
        glm::vec3 v2 = mesh.vertices[i2] + glm::vec3(0.0f, 0.135f, 0.0f);
		ub = glm::max(glm::max(glm::max(ub, v0), v1), v2);
		lb = glm::min(glm::min(glm::min(lb, v0), v1), v2);
        g_scene.mesh.push_back(Triangle(v0, v1, v2, metal));
    }
	// Define axis-aligned bounding box
	glm::vec3 center = (ub + lb) * 0.5f;
	glm::vec3 rad = glm::abs(ub - lb) * 0.5f;

	g_scene.mesh_bbox = Box(center, rad);
}


#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
// MODIFY THIS FUNCTION!
void updateLine(RTContext &rtx, int y)
{
    int nx = rtx.width;
    int ny = rtx.height;
    float aspect = float(nx) / float(ny);
    glm::vec3 lower_left_corner(-1.0f * aspect, -1.0f, -1.0f);
    glm::vec3 horizontal(2.0f * aspect, 0.0f, 0.0f);
    glm::vec3 vertical(0.0f, 2.0f, 0.0f);
    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::mat4 world_from_view = glm::inverse(rtx.view);

    // random offset for antialiasing
    float random_offset_u;
    float random_offset_v;

    if (rtx.anti_aliasing_enabled) {
        random_offset_u = random_float();
        random_offset_v = random_float();
    } else {
        random_offset_u = 0.0f;
        random_offset_v = 0.0f;
    }

    #pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < nx; ++x) {
        // float u = (float(x) + 0.5f) / float(nx);
        float u = (random_offset_u + float(x) + 0.5f) / float(nx);
        // float v = (float(y) + 0.5f) / float(ny);
        float v = (random_offset_v + float(y) + 0.5f) / float(ny);
        Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
        r.A = glm::vec3(world_from_view * glm::vec4(r.A, 1.0f));
        r.B = glm::vec3(world_from_view * glm::vec4(r.B, 0.0f));

        // Note: in the RTOW book, they have an inner loop for the number of
        // samples per pixel. Here, you do not need this loop, because we want
        // some interactivity and accumulate samples over multiple frames
        // instead (until the camera moves or the rendering is reset).

        if (rtx.current_frame <= 0) {
            // Here we make the first frame blend with the old image,
            // to smoothen the transition when resetting the accumulation
            glm::vec4 old = rtx.image[y * nx + x];
            rtx.image[y * nx + x] = glm::clamp(old / glm::max(1.0f, old.a), 0.0f, 1.0f);
        }
        glm::vec3 c = color(rtx, r, rtx.max_bounces);
        rtx.image[y * nx + x] += glm::vec4(c, 1.0f);
    }
}
#pragma clang diagnostic pop

void updateImage(RTContext &rtx)
{
    if (rtx.freeze) return;                    // Skip update
    rtx.image.resize(rtx.width * rtx.height);  // Just in case...

    updateLine(rtx, rtx.current_line % rtx.height);

    if (rtx.current_frame < rtx.max_frames) {
        rtx.current_line += 1;
        if (rtx.current_line >= rtx.height) {
            rtx.current_frame += 1;
            rtx.current_line = rtx.current_line % rtx.height;
        }
    }
}

void resetImage(RTContext &rtx)
{
    rtx.image.clear();
    rtx.image.resize(rtx.width * rtx.height);
    rtx.current_frame = 0;
    rtx.current_line = 0;
    rtx.freeze = false;
}

void resetAccumulation(RTContext &rtx)
{
    rtx.current_frame = -1;
}

}  // namespace rt
