#pragma once
#include "cmmn.h"
#include "texture.h"

namespace raytracer11 {
	struct postprocessor {
		float inv_gamma;
		bool tonemap;
		float tonemap_white;
		uint _numthreads;
		uvec2 _tilesize;
		postprocessor(float gma, bool tm, float white, int numt, uvec2 ts)
			: inv_gamma(1.f/gma), tonemap(tm), tonemap_white(white), _numthreads(numt > 1 ? numt : thread::hardware_concurrency()), _tilesize(ts) {}

		void render(texture2d* in, texture2d* out);
	private:
		void postprocess_tile(uvec2 t, texture2d* in, texture2d* out);
	};
}