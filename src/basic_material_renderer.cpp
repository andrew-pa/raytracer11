#pragma once
#include "basic_material_renderer.h"

#ifndef p3
#define p3
vec3 pow3(vec3 v, float p)
{
	return vec3(pow(v.x, p), pow(v.y, p), pow(v.z, p));
}
#endif

namespace raytracer11
{
	vec3 basic_material::shade(renderer* rndr, const ray& r, vec3 l, vec3 lc, const hit_record& hr, uint depth)
	{
		vec3 v = normalize(-r.d);
		vec3 Ld = (diffuse*lc) * glm::max(0.f, dot(hr.norm, l));
		vec3 h = normalize(v + l);
		vec3 Ls = (specular*lc) * pow(glm::max(0.f,dot(hr.norm, h)), specular_exp);
		float shadow = 1.f;
		if (rndr->scene()->hit(ray(r(hr.t) + l*.001f, l), 10000.f) >= 0)
			shadow = 0.f;
		return (Ld + Ls) *shadow;
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
				col += hr.hit_surface->mat()->shade(this, r, normalize(li.pos - r(hr.t)), li.color, hr, depth);
			}
			return col;
		}
		else
		{
			return background(r.d);
		}
	}
}