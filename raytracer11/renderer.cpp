#include "renderer.h"

namespace raytracer11
{
	renderer::renderer(camera c, surface* s, texture<vec3, uvec2, vec2>* _rt)
		: _c(c), _scene(s), rt(_rt)
	{

	}
	
	renderer::~renderer()
	{
		delete _scene;
		_scene = nullptr;
		delete rt;
		rt = nullptr;
	}

	void simple_renderer::render()
	{
		ray r(vec3(0),vec3(0));
		uvec2 px(0);
		for (px.y = 0; px.y < rt->size().y; ++px.y)
		{
			for (px.x = 0; px.x < rt->size().x; ++px.x)
			{
				r = _c.generate_ray((vec2)px);
				rt->pixel(px) = raycolor(r);
			}
		}
	}
}