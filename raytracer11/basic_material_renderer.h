#pragma once
#include "cmmn.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"

namespace raytracer11
{
	struct point_light
	{
		vec3 pos;
		vec3 color;
		point_light(vec3 p, vec3 c)
			: pos(p), color(c){}
	};

	struct basic_material : public material
	{
		vec3 diffuse;
		vec3 specular;
		float specular_exp;
		
		basic_material(vec3 d, vec3 s, float p)
			: diffuse(d), specular(s), specular_exp(p){}

		vec3 shade(renderer* rndr, const ray& r, vec3 l, vec3 lc, const hit_record& hr, uint depth = 0)override;
		
	};

	class basic_material_renderer :
		public parallel_tiles_renderer
	{
	protected:
		vec3 background(vec3 rd)
		{
			return mix(
					vec3(0, .3f, 1),
					vec3(0, .1f, .4f), 
					rd.y*.5f);
		}
		vector<point_light> _lights;
	public:
		const uint max_depth = 16;

		vec3 raycolor(const ray& r, uint depth = 0) override;

		basic_material_renderer(camera c, surface* s, texture<vec3, uvec2, vec2>* rt, uvec2 ts = uvec2(32), int numt = -1)
			: parallel_tiles_renderer(c, s, rt, numt, ts){}

		proprw(vector<point_light>, lights, { return _lights; })
	};
}

