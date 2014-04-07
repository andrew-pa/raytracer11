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
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/noise.hpp>
#include <glm/gtx/random.hpp>
using namespace glm;

#define proprw(t, n, gc) inline t& n() gc
#define propr(t, n, gc) inline t n() const gc

template <typename T>
struct property_type
{
	T* _data;
	property_type(T* x)
		: _data(x)	{}

	inline virtual operator T()
	{
		return *_data;
	}

	inline virtual T& operator =(T n)
	{
		*_data = n;
		return *_data;
	}

	inline virtual T* operator ->()
	{
		return _data;
	}
	inline virtual T& operator *()
	{
		return *_data;
	}


	virtual ~property_type() {}
};

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

		inline vec3 center()
		{
			return (_min + _max) * 1.f/2.f;
		}
	};


}