#pragma once
#include "path_tracing_renderer.h"

namespace raytracer11 {

	inline float squlen(vec3 v)
	{
		return dot(v, v);
	}


	struct emmisive_material : public path_tracing_material
	{
		emmisive_material(color_property e)
			: path_tracing_material(e) {}

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
		: public path_tracing_material
	{
		color_property R;

		diffuse_material(color_property r)
			: R(r), path_tracing_material(vec3(0)) {}

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

	struct perfect_reflection_material : public path_tracing_material
	{
		color_property R;

		perfect_reflection_material(color_property r)
			: R(r), path_tracing_material(vec3(0)) {}

		vec3 brdf(vec3 ki, vec3 ko, const hit_record& hr)	override
		{
			return R(hr);//vec3(pow(abs(dot(ki, ko)), shininess));
		}

		vec3 random_ray(vec3 n, vec3 ki, float* pdf) override
		{
			if (pdf) *pdf = 1.f;
			return reflect(-ki, n);
			//if (pdf) *pdf = dot(ki, n) / pi<float>();
			//return cosine_distribution(n);
		}
	};

	struct cone_fuzzy_reflection_material : public path_tracing_material {
		color_property R;
		float prob_ref, cone_size;
		
		cone_fuzzy_reflection_material(color_property r, float pr, float cs) : R(r), prob_ref(pr), cone_size(cs), path_tracing_material(vec3(0)) {}
		
		vec3 brdf(vec3 ki, vec3 ko, const hit_record& hr) override {
			return R(hr);
		}
		
		vec3 random_ray(vec3 n, vec3 ki, float* pdf) override {
			if(linearRand(0.f, 1.f) < prob_ref) {
				if(pdf) *pdf = prob_ref;
				return cone_distribution(n, cone_size);
			} else {
				if(pdf) *pdf = 1.f-prob_ref;
				return cosine_distribution(n);
			}
		}
	};
}