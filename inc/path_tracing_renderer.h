#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
namespace raytracer11
{

inline float squlen(vec3 v)
{
	return dot(v, v);
}

struct path_tracing_material
	: public material
{
	vec3 Le;
	path_tracing_material(vec3 e)
		: Le(e){}

	virtual vec3 brdf(vec3 ki, vec3 ko) = 0;
	virtual vec3 random_ray(vec3 n, vec3 ki) = 0;
	vec3 shade(renderer* rndr, const ray& r, vec3 l, vec3 lc, const hit_record& hr, uint depth = 0)override;
};

struct emmisive_material : public path_tracing_material
{
	emmisive_material(vec3 e)
		: path_tracing_material(e) {}

	vec3 brdf(vec3 ki, vec3 ko)	override
	{
		return vec3(0);
	}

	vec3 random_ray(vec3 n, vec3 ki) override
	{
		return vec3(0);
	}
};

struct diffuse_material
	: public path_tracing_material
{
	vec3 R;

	diffuse_material(vec3 r)
		: R(r), path_tracing_material(vec3(0)) {}

	vec3 brdf(vec3 ki, vec3 ko)	override
	{
		return R;
	}

	vec3 random_ray(vec3 n, vec3 ki) override
	{
		return cosine_distribution(n);
	}
};

class path_tracing_renderer :
	public parallel_tiles_renderer
{
public:
	const uint max_depth = 4;

	vec3 raycolor(const ray& r, uint depth = 0) override;


	path_tracing_renderer(camera c, surface* s, texture<vec3, uvec2, vec2>* rt, uvec2 ts = uvec2(32), int numt = -1)
		: parallel_tiles_renderer(c, s, rt, numt, ts){}
};
}