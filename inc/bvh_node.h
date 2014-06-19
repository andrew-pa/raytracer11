#pragma once
#include "surface.h"

namespace raytracer11
{
	class bvh_node :
		public surface
	{
		surface* _left, *_right;
		aabb _bounds;
	public:
		bvh_node(surface* l, surface* r)
			: _left(l), _right(r), _bounds(l->bounds(), r->bounds()) {}

		bvh_node(vector<surface*>& objects, uint axi = 0);

		bool hit(const ray& r, hit_record& hr) override;
		float hit(const ray& r, float xt) override;

		inline aabb bounds() const override { return _bounds; }

		proprw(surface*, left, { return _left; });
		proprw(surface*, right, { return _right; });


		~bvh_node()
		{
			delete _left;
			_left = nullptr;
			delete _right;
			_right = nullptr;
		}
	};
}

