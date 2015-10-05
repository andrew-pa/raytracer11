#pragma once
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <array>
#include <stack>
#include <map>
#include <iostream>
#include <fstream>
#include <functional>
#include <exception>
#include <algorithm>
#include <chrono>
#include <random>
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

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/io.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>
using namespace glm;


#define proprw(t, n, gc) inline t& n() gc
#define propr(t, n, gc) inline t n() const gc

#ifndef MINGW
#define WRITE_PER_THREAD_PERF_DATA
#endif
#define WRITE_WP_PERF_DATA

//#define implements(current_class, interface) const int IMPLEMENT_CHECK_##interface = INTERFACE_##interface##<current_class>();


namespace raytracer11
{
	struct ray
	{
	public:
		vec3 e, d;
		ray(vec3 _e, vec3 _d)
			: e(_e), d(_d){}

		inline vec3 operator ()(float t) const
		{
			return e + d*t;
		}
	};

	struct aabb
	{
	public:
		vec3 _min;
		vec3 _max;

		aabb()
			: _min(0), _max(0)
		{}

		aabb(vec3 m, vec3 x)
			: _min(m), _max(x)
		{}

		aabb(const aabb& a, const aabb& b)
			: _min(), _max()
		{
			add_point(a._min);
			add_point(a._max);
			add_point(b._min);
			add_point(b._max);
		}

		inline void add_point(vec3 p)
		{
			if (p.x > _max.x) _max.x = p.x;
			if (p.y > _max.y) _max.y = p.y;
			if (p.z > _max.z) _max.z = p.z;

			if (p.x < _min.x) _min.x = p.x;
			if (p.y < _min.y) _min.y = p.y;
			if (p.z < _min.z) _min.z = p.z;
		}

		inline bool contains(vec3 p) const
		{
			if (p.x >= _min.x && p.x <= _max.x &&
				p.y >= _min.y && p.y <= _max.y &&
				p.z >= _min.z && p.z <= _max.z)
				return true;
			return false;
		}

		inline bool inside_of(const aabb& b)
		{
			return b.contains(vec3(_min.x, _min.y, _min.z)) ||

				b.contains(vec3(_max.x, _min.y, _min.z)) ||
				b.contains(vec3(_min.x, _max.y, _min.z)) ||
				b.contains(vec3(_min.x, _min.y, _max.z)) ||

				b.contains(vec3(_min.x, _max.y, _max.z)) ||
				b.contains(vec3(_max.x, _min.y, _max.z)) ||
				b.contains(vec3(_max.x, _max.y, _min.z)) ||

				b.contains(vec3(_max.x, _max.y, _max.z));
		}

		inline aabb transform(const mat4& m) const
		{
			vec3 min, max;
			min = vec3(m[3][0], m[3][1], m[3][2]);
			max = min;

			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
				{
					if(m[i][j] > 0)
					{
						min[i] += m[i][j] * _min[j];
						max[i] += m[i][j] * _max[j];
					}
					else
					{
						min[i] += m[i][j] * _max[j];
						max[i] += m[i][j] * _min[j];
					}
				}
			return aabb(min, max);
		}

		inline bool hit(const ray& r) const
		{
			if (contains(r.e)) return true;

			vec3 rrd = 1.f / r.d;

			vec3 t1 = (_min - r.e) * rrd;
			vec3 t2 = (_max - r.e) * rrd;

			vec3 m12 = glm::min(t1, t2);
			vec3 x12 = glm::max(t1, t2);

			float tmin = m12.x;
			tmin = glm::max(tmin, m12.y);
			tmin = glm::max(tmin, m12.z);

			float tmax = x12.x;
			tmax = glm::min(tmax, x12.y);
			tmax = glm::min(tmax, x12.z);


			return tmax >= tmin;
		}

		inline pair<float,float> hit_retint(const ray& r) const
		{
			vec3 rrd = 1.f / r.d;

			vec3 t1 = (_min - r.e) * rrd;
			vec3 t2 = (_max - r.e) * rrd;

			vec3 m12 = glm::min(t1, t2);
			vec3 x12 = glm::max(t1, t2);

			float tmin = m12.x;
			tmin = glm::max(tmin, m12.y);
			tmin = glm::max(tmin, m12.z);

			float tmax = x12.x;
			tmax = glm::min(tmax, x12.y);
			tmax = glm::min(tmax, x12.z);


			return pair<float,float>(tmin, tmax);
		}

		inline vec3 center()
		{
			return (_min + _max) * 1.f/2.f;
		}

		inline void add_aabb(const aabb& b)
		{
			add_point(b._min);
			add_point(b._max);
		}
	};

	static mt19937 rnd_generator;

	inline void init_random() {
		random_device rd;
		rnd_generator = mt19937(rd());
	}
	inline void init_random(uint seed) {
		rnd_generator = mt19937(seed);
	}


	inline void make_orthonormal(vec3& w, vec3& u, vec3& v)
	{
		w = normalize(w);
		vec3 t = fabsf(w.x) > .1f ?
			vec3(0, 1, 0) : vec3(1, 0, 0);
		u = normalize(cross(w, t));
		v = cross(w, u);
	}

	inline float rand_float() {
		uniform_real_distribution<float> dist;
		return dist(rnd_generator);
	}

//#define CRANDOM
	inline vec3 cosine_distribution(vec3 n)
	{
		vec3 w = n;
		vec3 u, v;
		make_orthonormal(w, u, v);

#ifdef CRANDOM
		float e1 = linearRand(0.f, 1.f);
		float e2 = linearRand(0.f, 1.f);
#else 
		
		float e1 = rand_float(), e2 = rand_float();
#endif
		float se2 = sqrtf(e2);
		float t2e = pi<float>() * 2 * e1;
		vec3 d =
			(cos(t2e)*se2*u) +
			(sin(t2e)*se2*v) +
			(sqrtf(1 - e2)*w);
		return normalize(d);
	}

	inline vec3 cone_distribution(vec3 n, float r, float h = 1)
	{
		vec2 p = diskRand(r);

		vec3 w = n;
		vec3 u, v;
		make_orthonormal(w, u, v);

		vec3 x = p.x*u + p.y*v + h*w;

		return normalize(x);
	}

	/*
	vec3 rand_hemi( const vec3 n, const vec2 p ) {
		vec2 r = hash22(p)*6.2831;
		vec3 dr=vec3(sin(r.x)*vec2(sin(r.y),cos(r.y)),cos(r.x));
		return dot(dr,n) * dr;
	}
	*/

	inline vec3 hemi_distribution(vec3 n) {
		vec2 r = vec2(rand_float(), rand_float())*pi<float>()*2.f;
		vec3 dr = vec3(sin(r.x)*vec2(sin(r.y), cos(r.y)), cos(r.x));
		return dot(dr, n) * dr;
	}



#ifdef _MSC_VER
#define NOEXCEPT throw()
#else
#define NOEXCEPT noexcept
#endif
	class rexception : public std::exception
	{
		string msg;
	public:
		rexception(const string& m)
			: msg(m) {}
		const char* what() const NOEXCEPT override { return msg.c_str(); }
	};
}
