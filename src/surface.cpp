#include "surface.h"

namespace raytracer11
{
	bool sphere::hit(const ray& r, hit_record& hr)
	{
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
		return xt;
	}
}