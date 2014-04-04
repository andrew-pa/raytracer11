#include "parallel_tiles_renderer.h"

namespace raytracer11
{

	void parallel_tiles_renderer::render_tile(uvec2 pos)
	{
		ray r(vec3(0), vec3(0));
		hit_record hr(10000.f);
		for (float y = pos.y; y < pos.y + _tilesize.y && y < rt->size().y; ++y)
		{
			for (float x = pos.x; x < pos.x + _tilesize.x && x < rt->size().x; ++x)
			{
				r = _c.generate_ray(vec2(x, y));
				hr.t = 100000.f;
				rt->pixel(uvec2(x, y)) = raycolor(r);
			}
		}
	}

	void parallel_tiles_renderer::render()
	{
		mutex tile_queue_mutex;
		queue<uvec2> tiles;

		for (uint y = 0; y < rt->size().y; y += _tilesize.y)
		{
			for (uint x = 0; x < rt->size().x; x += _tilesize.x)
			{
				tiles.push(uvec2(x, y));
			}
		}

		vector<thread> threads;
		for (int ti = 0; ti < _numthreads; ++ti)
		{
			threads.push_back(thread([&] {
				uint tiles_rendered = 0;
				bool done = false;
				while(!done)
				{
					uvec2 t;
					{
						unique_lock<mutex> lm(tile_queue_mutex);
						if (tiles.empty()) { done = true; break; }
						t = tiles.front(); tiles.pop();
						done = tiles.empty();
					}
					render_tile(t);
					tiles_rendered++;
				}
				cout << "thread " << this_thread::get_id() << " rendered " << tiles_rendered << " chunks" << endl;
			}));
		}

		for(auto& t : threads)
		{
			t.join();
			this_thread::sleep_for(std::chrono::microseconds(20));
		}
	}
}