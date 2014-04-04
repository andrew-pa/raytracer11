#pragma once 
#include "cmmn.h"	

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
			
			vec3 mins = glm::min(t1, t2);
			vec3 maxs = glm::max(t1, t2);

			float tmin = glm::max(mins.x, glm::max(mins.y, mins.z));
			float tmax = glm::min(mins.x, glm::min(mins.y, mins.z));

			return tmax >= tmin;
		}
	};
}