#pragma once
#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
namespace raytracer11
{

	struct color_property {
		vec3 col; bool both;
		texture<vec3, uvec2, vec2>* tex;
	public:
		color_property(vec3 c = vec3(0.f))
			: col(c), tex(nullptr), both(false) {}
		color_property(texture<vec3, uvec2, vec2>* t)
			: tex(t), col(0.f), both(false) {}
		color_property(vec3 c, texture<vec3, uvec2, vec2>* t)
			: tex(t), col(c), both(true) {
			if (col == vec3(1.f) || col == vec3(0.f) || tex == nullptr) both = false;
		}

		void operator =(vec3 c) { col = c; tex = nullptr; both = false; }
		void operator =(texture<vec3, uvec2, vec2>* t) { tex = t; both = false; }
		void operator =(tuple<vec3, texture<vec3, uvec2, vec2>*> x) {
			col = get<0>(x);
			tex = get<1>(x);
			both = true;
		}

		inline vec3 operator()(const hit_record& hr) const {
			if (both) return tex->texel(hr.texcoord)*col;
			else if (tex) return tex->texel(hr.texcoord);
			else return col;
		}
	};


struct path_tracing_material
	: public material
{
	color_property Le;
	texture<vec3, uvec2, vec2>* normalmap;
	path_tracing_material(color_property e)
		: Le(e), normalmap(nullptr) {}

	virtual vec3 brdf(vec3 ki, vec3 ko, const hit_record& hr) = 0;
	//virtual float pdf(vec3 ki, vec3 ko, vec3 n) = 0;
	virtual vec3 random_ray(vec3 n, vec3 ki, float* pdf) = 0;
	vec3 shade(renderer* rndr, const ray& r, vec3 l, vec3 lc, const hit_record& hr, uint depth = 0)override;
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
