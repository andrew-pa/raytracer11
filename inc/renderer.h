#pragma once
#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"

namespace raytracer11
{
	class renderer
	{
		uint _numthreads;
		void render_tile(uvec2 pos);
		void render_tile_aa(uvec2 pos);
	public:
		camera cam;
		shared_ptr<surface> scene;
		shared_ptr<texture<vec3, uvec2, vec2>> render_target;
		shared_ptr<texture<vec3, uvec3, vec3>> env_map;
		float env_luma;
		uint samples;
		uvec2 tile_size;

		void render();
	
		const uint max_depth = 4;

		vec3 raycolor(const ray& r, uint depth = 0);

		renderer(camera c, shared_ptr<surface> s, shared_ptr<texture<vec3, uvec2, vec2>> rt, uint aasmp = 0, uvec2 ts = uvec2(32), int numt = -1)
			: cam(c), scene(s), render_target(rt), tile_size(ts), 
				_numthreads(numt == -1 ? thread::hardware_concurrency() : numt), 
				samples(aasmp) {}
	};
}
