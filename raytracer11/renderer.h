#pragma once
#include "cmmn.h"
#include "camera.h"
#include "surface.h"
#include "texture.h"

namespace raytracer11
{
	class renderer
	{
	protected:
		virtual vec3 raycolor(const ray& r, uint depth = 0) = 0;
		camera _c;
		surface* _scene;
		texture<vec3,uvec2,vec2>* rt;
	public:
		renderer(camera c, surface* s, texture<vec3,uvec2,vec2>* _rt);
		~renderer();

		virtual void render() = 0;

		proprw(camera, cam, { return _c; });
		proprw(surface*, scene, { return _scene; });
		inline texture<vec3, uvec2, vec2>* render_target() { return rt; }
	};

	class simple_renderer : public renderer
	{
	public:
		simple_renderer(camera c, surface* s, texture<vec3, uvec2, vec2>* rt)
			: renderer(c,s,rt){}
		void render() override;
	};
}

