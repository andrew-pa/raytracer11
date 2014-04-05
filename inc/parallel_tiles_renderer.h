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
	public:
		parallel_tiles_renderer(camera c, surface* s, texture<vec3, uvec2, vec2>* rt, int num_threads_ = -1, uvec2 tile_size_ = uvec2(32,32))
			: renderer(c, s, rt), _tilesize(tile_size_), _numthreads(num_threads_ == -1 ? thread::hardware_concurrency() : num_threads_)
		{}

		void render() override;
	
		proprw(uvec2, tile_size, { return _tilesize; });
		proprw(uint, number_of_threads, { return _numthreads; });
	};
}

