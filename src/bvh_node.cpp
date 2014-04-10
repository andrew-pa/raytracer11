#include "bvh_node.h"


namespace raytracer11
{
	bvh_node::bvh_node(vector<surface*>& objects, uint axi)
	{
		if(objects.size() == 0)
		{
			_left = nullptr;
			_right = nullptr;
			_bounds = aabb(vec3(), vec3());
		}
		else if(objects.size() == 1)
		{
			_left = objects[0]; _right = nullptr;
			_bounds = _left->bounds();
		}
		else if (objects.size() == 2)
		{
			_left = objects[0]; _right = objects[1];
			_bounds = aabb(_left->bounds(), _right->bounds());
		}
		else
		{
			sort(objects.begin(), objects.end(), [&](surface* a, surface* b)
			{
				float pac = a->bounds().center()[axi];
				float pbc = b->bounds().center()[axi];
				return pac > pbc;
			});
			auto half = objects.size() / 2;
			auto left_half = vector<surface*>(objects.begin(), objects.begin() + half);
			auto right_half = vector<surface*>(objects.begin() + half, objects.end());
			//optomize case where left node should only be a pntr to a primitive because left_half.size() == 1
			
			_left = new bvh_node(left_half, (axi + 1) % 3);
			_right = new bvh_node(right_half, (axi + 1) % 3);
			_bounds = aabb(_left->bounds(), _right->bounds());
		}
	}

	bool bvh_node::hit(const ray& r, hit_record& hr)
	{
		if (!_bounds.hit(r)) return false;
		if(_left != nullptr && _right == nullptr)
		{
			return _left->hit(r, hr);
		}
		hit_record lhr(hr), rhr(hr);
		bool lh = (_left != nullptr ?
			_left->hit(r, lhr) : false);
		bool rh = (_right != nullptr ?
			_right->hit(r, rhr) : false);
		if(lh&&rh)
		{
			if(lhr.t < rhr.t)
			{
				hr = lhr;
				return true;
			}
			else
			{
				hr = rhr;
				return true;
			}
		}
		else if(lh)
		{
			hr = lhr;
			return true;
		}
		else if(rh)
		{
			hr = rhr;
			return true;
		}
		return false;
	}

	float bvh_node::hit(const ray& r, float xt)
	{
		if (!_bounds.hit(r)) return -1;
		float lt = (_left != nullptr ?
			_left->hit(r, xt) : -1);
		float rt = (_right != nullptr ?
			_right->hit(r, xt) : -1);
		bool lh = lt >= 0;
		bool rh = rt >= 0;
		if (lh&&rh)
		{
			if (lt < rt)
			{
				return lt;
			}
			else
			{
				return rt;
			}
		}
		else if (lh)
		{
			return lt;
		}
		else if (rh)
		{
			return rt;
		}
		return -1;
	}
}
