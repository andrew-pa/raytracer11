#pragma once
#include "cmmn.h"
#include "surface.h"

namespace raytracer11 
{
	template<int Xdiv = 8, int Ydiv = 8, int Zdiv = 8>
	class grid : public surface
	{
		surface* _grid[Xdiv][Ydiv][Zdiv];
		aabb _bounds;
		vec3 voxel_size;
	public:
		grid(vector<surface*>& objects)
			: _bounds(vec3(numeric_limits<float>::max()), vec3(numeric_limits<float>::min()))
			
		{
			//calculate bounds
			for (const auto& s : objects)
			{
				_bounds.add_aabb(s->bounds());
			}

			vec3 delta = (_bounds._max - _bounds._min) / vec3(Xdiv, Ydiv, Zdiv);
			voxel_size = delta;

			for (int z = 0; z < Zdiv; ++z)
			{
				for (int y = 0; y < Ydiv; ++y)
				{
					for (int x = 0; x < Xdiv; ++x)
					{
						vec3 p = _bounds._min + delta*vec3(x, y, z);
						aabb box = aabb(p, p + delta);
						vector<surface*> items;
						for(const auto& s : objects)
						{
							if(s->inside_of(box))
								items.push_back(s);
						}
						if(items.size() == 0) 
						{
							_grid[x][y][z] = nullptr;
						}
						else if(items.size() > 1)
						{
							_grid[x][y][z] = new group(items);
						}
						else
						{
							_grid[x][y][z] = items[0];
						}
					}
				}
			}
		}

		bool hit(const ray& r, hit_record& hr) override
		{
			const uvec3 res = uvec3(Xdiv, Ydiv, Zdiv);

			uvec3 origin_voxel = uvec3(-1);

			if(_bounds.contains(r.e))
			{
				origin_voxel = uvec3((r.e - _bounds._min) / voxel_size);
			}
			else
			{
				auto h = _bounds.hit_retint(r);
				if(h.second >= h.first) //tmax >= tmin
				{
					vec3 p = r(h.first);
					origin_voxel = uvec3((p - _bounds._min) / voxel_size);
				}
				else return false;
			}

			origin_voxel = clamp(origin_voxel, uvec3(0), res-uvec3(1));

			ivec3 step = ivec3(sign(r.d));

			ivec3 vox = ivec3(origin_voxel);
			while (true)
			{
				if (vox.x < 0 || vox.x > res.x - 1) break;
				if (vox.y < 0 || vox.y > res.y - 1) break;
				if (vox.z < 0 || vox.z > res.z - 1) break;

				auto s = _grid[vox.x][vox.y][vox.z];
				if(s != nullptr)
				{
					if (s->hit(r, hr)) return true;
				}
				vox += step;
			}
			
			return false; 
		}
		float hit(const ray& r, float xt) override { return -1;  }

		inline aabb bounds() const override { return _bounds; }

		~grid(){}
	};
}

