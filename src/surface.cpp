#include "surface.h"

namespace raytracer11
{
	bool sphere::hit(const ray& r, hit_record& hr)
	{
		if (!bounds().hit(r)) return false;
		vec3 v = r.e - _c;
		float b = -dot(v, r.d);
		float det = (b*b) - dot(v, v) + _r*_r;
		if(det > 0)
		{
			det = glm::sqrt(det);
			float i1 = b - det;
			float i2 = b + det;
			if(i2 > 0)
			{
				if(i1 < 0)
				{
					if(i2 < hr.t)
					{
						hr.t = i2;//what?!
						return false;
					}
				}
				else
				{
					if(i1 < hr.t)
					{
						hr.t = i1;
						hr.norm = normalize(r(i1) - _c);

						{
							float phi = acosf(-dot(hr.norm, vec3(0, 1, 0)));
							hr.texcord.y = phi * one_over_pi<float>();
							float theta = acosf(dot(vec3(0, 0, -1), hr.norm) / sinf(phi))
								* two_over_pi<float>();
							if (dot(vec3(1, 0, 0), hr.norm) >= 0) hr.texcord.x = (1 - theta);
							else hr.texcord.x = theta;
						}
						
						hr.hit_surface = this;
						return true;
					}
				}
			}
		}
		return false;
	}

	float sphere::hit(const ray& r, float xt)
	{
		vec3 v = r.e - _c;
		float b = -dot(v, r.d);
		float det = (b*b) - dot(v, v) + _r*_r;
		if (det > 0)
		{
			det = glm::sqrt(det);
			float i1 = b - det;
			float i2 = b + det;
			if (i2 > 0)
			{
				if (i1 < 0)
				{
					if (i2 < xt)
					{
						return i2;
					}
				}
				else
				{
					if (i1 < xt)
					{
						return i1;
					}
				}
			}
		}
		return -1;
	}

	vec3 box::get_normal(vec3 p)
	{
		static const vec3 axises[] =
		{
			vec3(1,0,0),
			vec3(0,1,0),
			vec3(0,0,1),
		};
		vec3 n = vec3(0);
		float m = FLT_MAX;
		float dist;
		vec3 np = p - _center;
		for (int i = 0; i < 3; ++i)
		{
			dist = fabsf(_extents[i] - fabsf(np[i]));
			if(dist < m)
			{
				m = dist;
				n = sign(np[i])*axises[i];
			}
		}
		return n;
	}

	bool box::hit(const ray& r, hit_record& hr)
	{
		vec3 _min = _center - _extents;
		vec3 _max = _center + _extents;

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

		if (tmax < tmin || tmin < 0 || tmin > hr.t) return false;
		hr.hit_surface = this;
		hr.t = tmin;
		hr.norm = get_normal(r(tmin));
		hr.texcord = vec2(0);
		return true;
	}

	float box::hit(const ray& r, float xt)
	{
		vec3 _min = _center - _extents;
		vec3 _max = _center + _extents;

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

		if (tmax < tmin || tmin < 0 || tmin > xt) return -1;
		return tmin;
	}
}