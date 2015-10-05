#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
#include "path_tracing_renderer.h"

namespace raytracer11
{
	vec3 path_tracing_material::shade(renderer* rndr, const ray& r, vec3 l, vec3 lc, const hit_record& hr, uint depth)
	{
		if (dot(Le(hr),Le(hr)) > 0)
			return Le(hr);

		vec3 v = normalize(-r.d);
		float pk = 0.f;
		vec3 nrd = random_ray(hr.norm, v, &pk);
		if(pk <= 0.f) return vec3(0.f);
		return (brdf(nrd, v, hr) * 
				rndr->raycolor(ray(r(hr.t) + nrd*.001f, nrd), depth + 1) * glm::max(0.f,dot(hr.norm, nrd))) / pk;
	}

	vec3 path_tracing_renderer::raycolor(const ray& r, uint depth)
	{
		if (depth > max_depth) return vec3(0);
		hit_record hr(10000.f);
		if (_scene->hit(r, hr))
		{
			return hr.hit_surface->mat()->shade(this, r, vec3(), vec3(), hr, depth);
		}
		else return vec3(0.0f);
	}
}
