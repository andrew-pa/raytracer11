#include "renderer.h"

namespace raytracer11
{
	void renderer::render_tile(uvec2 pos)
	{
		ray r(vec3(0), vec3(0));
		vec2 fpos = (vec2)pos;
		for (float y = fpos.y; y < fpos.y + tile_size.y && y < render_target->size().y; ++y)
		{
			for (float x = fpos.x; x < fpos.x + tile_size.x && x < render_target->size().x; ++x)
			{
				r = cam.generate_ray(vec2(x, y));
				render_target->pixel(uvec2(x, y)) = raycolor(r);
			}
		}
	}

	void renderer::render_tile_aa(uvec2 pos)
	{
		const float smpl = (1.f / (float)samples);
		ray r(vec3(0), vec3(0));
		const vec2 fpos = (vec2)pos;
		for (float y = fpos.y; y < fpos.y + tile_size.y && y < render_target->size().y; ++y)
		{
			for (float x = fpos.x; x < fpos.x + tile_size.x && x < render_target->size().x; ++x)
			{
				vec3 fc = vec3(0);
				for (float p = 0; p < samples; ++p)
				{
					for (float q = 0; q < samples; ++q)
					{
						const vec2 aaof = vec2(p+rand_float(), q+rand_float()) * smpl;
						r = cam.generate_ray(vec2(x, y) + aaof);
						fc += raycolor(r);
					}
				}
				render_target->pixel(uvec2(x, y)) = fc * (smpl*smpl);
			}
		}
	}

	void renderer::render()
	{
		mutex tile_queue_mutex;
		queue<uvec2> tiles;

		vector<uvec2> start_tiles;

		for (uint y = 0; y < render_target->size().y; y += tile_size.y)
		{
			for (uint x = 0; x < render_target->size().x; x += tile_size.x)
			{
				start_tiles.push_back(uvec2(x, y));
			}
		}

		for (int i = 0; i < 20 + start_tiles.size(); ++i)
			swap(start_tiles[rand() % start_tiles.size()], start_tiles[rand() % start_tiles.size()]);

		for (const auto& t : start_tiles)
			tiles.push(t);

		vector<thread> threads;
		for (int ti = 0; ti < _numthreads; ++ti)
		{
			
			threads.push_back(thread(function<void()>([&] {
#ifdef WRITE_PER_THREAD_PERF_DATA
				auto start_time = chrono::system_clock::now();
				uint tiles_rendered = 0;
#endif
				while (true)
				{
					uvec2 t;
					{
						unique_lock<mutex> lm(tile_queue_mutex);
						if (tiles.empty()) { break; }
						t = tiles.front(); tiles.pop();
					}
					if (samples == 0) render_tile(t); 
					else render_tile_aa(t);
#ifdef WRITE_PER_THREAD_PERF_DATA
					tiles_rendered++;
#endif
				}
#ifdef WRITE_PER_THREAD_PERF_DATA
				auto end_time = chrono::system_clock::now();
				long long tm = abs(chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count());
				auto tmpt = (double)tm / (double)tiles_rendered;
				cout << "thread " << this_thread::get_id() << " rendered " << tiles_rendered << " tiles"
					<< " took " << (double)tm / 1000000.0 << "ms, " << tmpt / 1000000.0 << "ms/tile" << endl;
#endif
			})));
			
		}

		for (auto& t : threads)
		{
			t.join();
		}
	}

	vec3 renderer::raycolor(const ray & r, uint depth) {
		if (depth > max_depth) return env_map != nullptr ? env_map->texel(r.d)*env_luma : vec3(0);
		hit_record hr(1e9f);
		if (scene->hit(r, hr)) {
			auto mat = hr.hit_surface->mat();
			if (length2(mat->Le(hr)) > 0) return mat->Le(hr);
			vec3 v = normalize(-r.d);
			float pk = 0.f; material::ray_type rt;
			vec3 nrd = mat->random_ray(hr.norm, v, &pk, &rt);
			if (pk <= 0.f) return env_map != nullptr ? env_map->texel(r.d)*env_luma : vec3(0);
			return (
					mat->brdf(nrd, v, hr) *
					raycolor(ray(r(hr.t) + nrd*0.001f, nrd), depth + 1) *
					glm::max(0.f, dot(hr.norm * (rt == material::ray_type::transmission ? -1.f : 1.f), nrd) )
				) / pk;
		}
		else return env_map != nullptr ? env_map->texel(r.d)*env_luma : vec3(0);
	}
}