#pragma once
#include "cmmn.h"	

namespace raytracer11
{
	class surface;

	struct hit_record
	{
		surface* hit_surface;
		float t;
		vec3 norm;
		vec2 texcord;
		hit_record(surface* s, float _t, vec3 n, vec2 t)
			: hit_surface(s), t(_t), norm(n), texcord(t)
		{}
		hit_record(float _t)
			: hit_surface(nullptr), t(_t), norm(vec3(0)), texcord(vec2(0))
		{}
	};

	class surface
	{
	public:
		virtual bool hit(const ray& r, hit_record& hr) = 0;
		virtual float hit(const ray& r, float xt) = 0;
		propr(virtual aabb, bounds, const = 0);
		virtual ~surface(){}
	};

	class sphere : public surface
	{
		vec3 _c;
		float _r;
	public:
		sphere(vec3 center, float radius)
			: _c(center), _r(radius)
		{}

		bool hit(const ray& r, hit_record& hr) override;
		float hit(const ray& r, float xt) override;

		aabb bounds() const override
		{
			return aabb(_c - vec3(_r), _c + vec3(_r));
		}

		proprw(vec3, center, { return _c; });
		proprw(float, radius, { return _r; });
	};
}

