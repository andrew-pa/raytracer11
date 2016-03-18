#pragma once
#include "cmmn.h"	
#include "texture.h"

namespace raytracer11
{
	class surface;

	struct hit_record
	{
		surface* hit_surface;
		float t;
		vec3 norm;
		vec2 texcoord;
		hit_record(surface* s, float _t, vec3 n, vec2 t)
			: hit_surface(s), t(_t), norm(n), texcoord(t)
		{}
		hit_record(float _t)
			: hit_surface(nullptr), t(_t), norm(vec3(0)), texcoord(vec2(0))
		{}
	};

	struct color_property {
		vec3 col;
		texture<vec3, uvec2, vec2>* tex;
	public:
		color_property(vec3 c = vec3(0.f))
			: col(c), tex(nullptr) {}
		color_property(texture<vec3, uvec2, vec2>* t)
			: tex(t), col(0.f) {}

		void operator =(vec3 c) { col = c; tex = nullptr; }
		void operator =(texture<vec3, uvec2, vec2>* t) { tex = t; }

		inline vec3 operator()(const hit_record& hr) const {
			if (tex) return tex->texel(hr.texcoord);
			else return col;
		}
	};

	struct material {
		color_property Le;
		virtual vec3 random_ray(vec3 n, vec3 ko, float* pdf) {
			//this sampling distribution works for every BRDF but can be very noisy
			if (pdf) *pdf = 1.f / two_pi<float>();
			return hemi_distribution(n);
		}
		virtual vec3 brdf(vec3 ko, vec3 ki, const hit_record& hr) = 0;
	protected:
		material(const color_property& le) : Le(le) {}
	};

	class surface
	{
	public:
		virtual bool hit(const ray& r, hit_record& hr) = 0;
		virtual float hit(const ray& r, float xt) = 0;
		propr(virtual aabb, bounds, = 0);
		propr(virtual material*, mat, { return nullptr; }); //this is actually a read/write property because it returns a pointer

		virtual bool inside_of(const aabb& b) const { return bounds().inside_of(b); }

		virtual ~surface(){}
	};

	//template <typename T>
	//int INTERFACE_accl_struct()
	//{
	//	T* x = new T(vector<surface*>());
	//	delete x;
	//	return 0;
	//}

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

	class box : public surface
	{
		vec3 _center;
		vec3 _extents;
		material* _mat;

		vec3 get_normal(vec3 p);
	public:
		box(vec3 c, vec3 r, material* m)
			: _center(c), _extents(r), _mat(m)
		{
		}

		bool hit(const ray& r, hit_record& hr) override;
		float hit(const ray& r, float xt) override;

		aabb bounds() const override
		{
			return aabb(_center + _extents, _center - _extents);
		}

		proprw(vec3, center, { return _center; });
		proprw(vec3, extents, { return _extents; });


		inline material* mat() const override { return _mat; }
	};

	class group : public surface
	{
		vector<surface*> _objects;
		aabb _bounds;
	public:
		group(vector<surface*>& objects)
			: _objects(objects), _bounds(vec3(numeric_limits<float>::max()), vec3(numeric_limits<float>::min()))
		{
			for(const auto& s : _objects)
			{
				_bounds.add_aabb(s->bounds());
			}
		}

		bool hit(const ray& r, hit_record& hr) override
		{
			if (!_bounds.hit(r)) return false;
			hit_record rhr = hr;
			bool h = false;
			for(const auto& s : _objects)
			{
				hit_record thr = rhr;
				if(s->hit(r, thr))
				{
					h = true;
					rhr = thr;
				}
			}
			hr = rhr;
			return h;
		}

		float hit(const ray& r, float xt) override
		{
			if (!_bounds.hit(r)) return -1;
			float rt = xt;
			for(const auto& s : _objects)
			{
				float q = s->hit(r, rt);
				if(q < rt)
				{
					rt = q;
				}
			}
			return rt;
		}

		inline aabb bounds() const override { return _bounds; }

	};
}

