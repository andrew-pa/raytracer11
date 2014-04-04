#pragma once
#include "basic_material_renderer.h"

namespace raytracer11
{
	vec3 basic_material::shade(renderer* rndr, const ray& r, vec3 l, vec3 lc, const hit_record& hr, uint depth)
	{
		vec3 Ld = (diffuse*lc) * dot(hr.norm, l);
		vec3 h = normalize(r.d + l);
		vec3 Ls = pow3((specular*lc) * dot(hr.norm, h), specular_exp);
		float shadow = 1.f;
		if (rndr->scene()->hit(ray(r(hr.t) + l*.001f, l), 10000.f) > 0)
			shadow = .2f;
		return (Ld + Ls)*shadow;
	}
	vec3 basic_material_renderer::raycolor(const ray& r, uint depth)
	{
		if (depth > max_depth) return vec3(0);
		hit_record hr(10000.f);
		if (_scene->hit(r, hr))
		{
			vec3 col = vec3(0);
			for (const auto& li : _lights)
			{
				col += hr.hit_surface->mat()->shade(this, r, li.pos - r(hr.t), li.color, hr, depth);
			}
			return col;
		}
		else
		{
			return background(r.d);
		}
	}
}