#pragma once
#include "cmmn.h"
#include "renderer.h"
#include "camera.h"
#include "surface.h"
#include "texture.h"

#define MINGW
#ifndef MINGW
#include <thread>
#include <mutex>
#else
#define EOWNERDEAD 2304234
#define EPROTO 234234234
#include <Windows.h>
#include <process.h>
#include <mutex>
#include <mingw.mutex.h>

class thread {
public:
	HANDLE winh;
	unsigned int id;
	unsigned int get_id() const { return id; }
	thread(function<void()> f) {
		function<void()>* pf = new function<void()>(f);
		winh = (HANDLE)_beginthreadex(NULL, 0, thrdfnc, (LPVOID)pf, 0, (unsigned*)&id);
	}

	static unsigned int __stdcall thrdfnc(void* arg) {
		function<void()>* f = (function<void()>*)arg;
		(*f)();
		return 0;
	}

	void join() {
		WaitForSingleObject(winh, INFINITE);
		CloseHandle(winh);
	}

	static unsigned int hardware_concurrency() noexcept { return 4; }
};

#endif


#include <queue>
using namespace std;

namespace raytracer11
{
	class parallel_tiles_renderer :
		public renderer
	{
	protected:
		uvec2 _tilesize;
		uint _numthreads;
		void render_tile(uvec2 pos);
		void render_tile_aa(uvec2 pos);//
		uint _samples;
	public:
		parallel_tiles_renderer(camera c, surface* s, texture<vec3, uvec2, vec2>* rt, int num_threads_ = -1, uvec2 tile_size_ = uvec2(32,32))
			: renderer(c, s, rt), _tilesize(tile_size_), _numthreads(num_threads_ == -1 ? thread::hardware_concurrency() : num_threads_),
			_samples(0)
		{}

		void render() override;
	
		//proprw(uint, aa_samples, { return _samples; });
		inline uint aa_samples() { return _samples*_samples; }
		inline void aa_samples(uint s) { _samples = sqrt(s); }
		proprw(uvec2, tile_size, { return _tilesize; });
		proprw(uint, number_of_threads, { return _numthreads; });
	};

}

