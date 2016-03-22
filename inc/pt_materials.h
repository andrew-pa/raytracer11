#pragma once
#include "surface.h"

namespace raytracer11 {

	inline float squlen(vec3 v)
	{
		return dot(v, v);
	}


	struct emmisive_material : public material
	{
		emmisive_material(color_property e)
			: material(e) {}

		vec3 brdf(vec3 ki, vec3 ko, const hit_record&)	override
		{
			return vec3(0);
		}

		vec3 random_ray(vec3 n, vec3 ki, float* pdf) override
		{
			if (pdf) *pdf = 0;
			return vec3(0);
		}
	};

	struct diffuse_material
		: public material
	{
		color_property R;

		diffuse_material(color_property r)
			: R(r), material(vec3(0)) {}

		vec3 brdf(vec3 ki, vec3 ko, const hit_record& hr)	override
		{
			return R(hr);
		}

		vec3 random_ray(vec3 n, vec3 ki, float* pdf) override
		{
			if (pdf) *pdf = dot(ki, n) / pi<float>();
			return cosine_distribution(n);
		}
	};

	struct perfect_reflection_material : public material
	{
		color_property R;

		perfect_reflection_material(color_property r)
			: R(r), material(vec3(0)) {}

		vec3 brdf(vec3 ki, vec3 ko, const hit_record& hr)	override
		{
			return R(hr);
		}

		vec3 random_ray(vec3 n, vec3 ki, float* pdf) override
		{
			if (pdf) *pdf = 1.f;
			return reflect(-ki, n);
		}
	};

	struct perfect_refraction_material : public material {
		color_property R;
		float eta;

		perfect_refraction_material(color_property r)
			: R(r), material(vec3(0)) {}

		vec3 brdf(vec3 ki, vec3 ko, const hit_record& hr)	override
		{
			return R(hr);
		}

		vec3 random_ray(vec3 n, vec3 ki, float* pdf) override
		{
			if (pdf) *pdf = 1.f;
			float ndki = dot(n, ki);
			float angle = 1.f - eta*eta * (1.f - ndki*ndki);
			if (angle < 0.f)
				return reflect(-ki, n);
			else {
				return (-eta * ndki - sqrt(angle)) * n + ki * eta;
			}
			return reflect(-ki, n);
		}
	};

	struct cook_torrance_material : public material {
		color_property R;
		float F0;

		cook_torrance_material(color_property r, float f0)
			: R(r), F0(f0), material(vec3(0)) {}

		vec3 brdf(vec3 ki, vec3 ko, const hit_record& hr)	override
		{
			vec3 h = normalize(ki + ko);
			float ndl = dot(hr.norm, ki);
			float ndv = dot(hr.norm, ko);
			float ndh = dot(hr.norm, h);
			float vdh = dot(ko, h);
			vec3 r = R(hr);
			return r * ( (D(ndh)*F(vdh)*G(ndl, ndv, ndh, vdh)) / 4.f*ndl*ndv );
		}
	protected:
		virtual float D(float ndh) const = 0;
		virtual float F(float vdh) const {
			//Schlick fresnel
			return F0 + (1 - F0)*pow(1 - vdh, 5);
		}
		virtual float G(float ndl, float ndv, float ndh, float vdh) const = 0;
	};
	
	struct GGX_material : public cook_torrance_material {
		float alpha2;

		GGX_material(color_property r, float a, float f0)
			: cook_torrance_material(r, f0), alpha2(a*a) {}

	protected:
		float D(float ndh) const override {
			float denom = ndh*ndh*(alpha2 - 1.f) + 1.f;
			denom *= denom * pi<float>();
			return alpha2 / denom;
		}
		inline float g(float ndx) const {
			float denom = ndx + sqrt(alpha2 + (1.f - alpha2)*ndx*ndx);
			return (2.f*ndx) / denom;
		}
		float G(float ndl, float ndv, float ndh, float vdh) const override {
			return g(ndl)*g(ndv);
		}
	};
}