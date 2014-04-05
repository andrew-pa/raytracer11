#pragma once
#include "cmmn.h"	
#include "renderer.h"

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

	class renderer;

	struct material
	{
		virtual vec3 shade(renderer* rndr, const ray& r, vec3 l, vec3 lc, const hit_record& hr, uint depth = 0) = 0;
	};

	class surface
	{
	public:
		virtual bool hit(const ray& r, hit_record& hr) = 0;
		virtual float hit(const ray& r, float xt) = 0;
		propr(virtual aabb, bounds, const = 0);
		propr(virtual material*, mat, const = 0); //this is actually a read/write property because it returns a pointer
		virtual ~surface(){}
	};

	class sphere : public surface
	{
		vec3 _c;
		float _r;
		material* _mat;
	public:
		sphere(vec3 center, float radius, material* m = nullptr)
			: _c(center), _r(radius), _mat(m)
		{}

		bool hit(const ray& r, hit_record& hr) override;
		float hit(const ray& r, float xt) override;

		aabb bounds() const override
		{
			return aabb(_c - vec3(_r), _c + vec3(_r));
		}

		proprw(vec3, center, { return _c; });
		proprw(float, radius, { return _r; });

		inline material* mat() const override { return _mat; }
	};
}

