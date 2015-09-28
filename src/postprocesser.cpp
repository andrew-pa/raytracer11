#pragma once
#include "postprocesser.h"
#include <glm/gtx/color_space.hpp>

namespace raytracer11 {
	void postprocessor::render(texture2d * in, texture2d * out) {
		mutex tile_queue_mutex;
		queue<uvec2> tiles;

		vector<uvec2> start_tiles;

		for (uint y = 0; y < in->size().y; y += _tilesize.y)
		{
			for (uint x = 0; x < in->size().x; x += _tilesize.x)
			{
				start_tiles.push_back(uvec2(x, y));
			}
		}

		//for (int i = 0; i < 20 + start_tiles.size(); ++i)
		//	swap(start_tiles[rand() % start_tiles.size()], start_tiles[rand() % start_tiles.size()]);
		shuffle(start_tiles.begin(), start_tiles.end(), rnd_generator);

		for (const auto& t : start_tiles)
			tiles.push(t);

		vector<thread> threads;
		for (int ti = 0; ti < _numthreads; ++ti)
		{
			threads.push_back(thread(function<void()>([&] {
				bool notdone = true;
#ifdef WRITE_PER_THREAD_PERF_DATA
				auto start_time = chrono::system_clock::now();
				uint tiles_rendered = 0;
#endif
				while (notdone)
				{
					uvec2 t;
					{
						unique_lock<mutex> lm(tile_queue_mutex);
						if (tiles.empty()) { notdone = false; break; }
						t = tiles.front(); tiles.pop();
					}
					postprocess_tile(t, in, out);
#ifdef WRITE_PER_THREAD_PERF_DATA
					tiles_rendered++;
#endif
				}
#ifdef WRITE_PER_THREAD_PERF_DATA
				auto end_time = chrono::system_clock::now();
				long long tm = abs(chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count());
				if (tm < 0)cout << "X";
				auto tmpt = (double)tm / (double)tiles_rendered;
				cout << "thread " << this_thread::get_id() << " postprocessed " << tiles_rendered << " tiles"
					<< " took " << (double)tm / 1000000.0 << "ms, " << tmpt / 1000000.0 << "ms/tile" << endl;
#endif
			})));
			
		}

		for (auto& t : threads)
			t.join();
	
	}

	void postprocessor::postprocess_tile(uvec2 fpos, texture2d * in, texture2d * out) {
		for (uint y = fpos.y; y < fpos.y + _tilesize.y && y < in->size().y; ++y) {
			for (uint x = fpos.x; x < fpos.x + _tilesize.x && x < in->size().x; ++x) {
				vec3 vin = in->pixel(uvec2(x, y));
				vec3 vout = vin;
				if (tonemap) {
					float luma = luminosity(vin);
					float tmluma = (luma / (1.f + luma)) * (1.f + (luma / tonemap_white));
					vout *= tmluma / luma;
				}
				vout = pow(vout, vec3(inv_gamma));
				out->pixel(uvec2(x, y)) = vout;
			}
		}
	}
}