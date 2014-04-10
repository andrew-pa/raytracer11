#pragma once
#include "cmmn.h"
#include "renderer.h"
#include "camera.h"
#include "surface.h"
#include "texture.h"
#include <thread>
#include <mutex>
#include <queue>
namespace raytracer11
{
	class parallel_tiles_renderer :
		public renderer
	{
	protected:
		uvec2 _tilesize;
		uint _numthreads;
		void render_tile(uvec2 pos);
		void render_tile_aa(uvec2 pos);
		uint _samples;
	public:
		parallel_tiles_renderer(camera c, surface* s, texture<vec3, uvec2, vec2>* rt, int num_threads_ = -1, uvec2 tile_size_ = uvec2(32,32))
			: renderer(c, s, rt), _tilesize(tile_size_), _numthreads(num_threads_ == -1 ? thread::hardware_concurrency() : num_threads_),
			_samples(0)
		{}

		void render() override;
	
		proprw(uint, aa_samples, { return _samples; });
		proprw(uvec2, tile_size, { return _tilesize; });
		proprw(uint, number_of_threads, { return _numthreads; });
	};

}

