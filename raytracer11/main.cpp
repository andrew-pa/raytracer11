#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
using namespace raytracer11;

class test_renderer : public parallel_tiles_renderer
{
protected:
	vec3 raycolor(const ray& r, uint depth = 0) override
	{
		hit_record hr(10000.f);
		if(_scene->hit(r, hr))
		{
			return mix(mix(vec3(.5f, 1, 0), vec3(0, 1, .5f), glm::max(0.f, dot(hr.norm, vec3(0, .5f, .5f)))),
				vec3(0, .5f, 1), glm::max(0.f, dot(hr.norm, vec3(.5f, .5f, 0))));
		}
	}
public:

	test_renderer(camera c, surface* s, texture<vec3, uvec2, vec2>* rt)
		: parallel_tiles_renderer(c, s, rt){}
};

int main()
{
	texture2d* rt = new texture2d(uvec2(640, 480));

	camera cam(vec3(0, 2, -5), vec3(0), (vec2)rt->size(), 1.f);

	sphere* s = new sphere(vec3(0), 1.f);

	test_renderer rndr(cam, s, rt);
	
	rndr.render();

	ostringstream fss;
	fss << "img" << time(nullptr) << ".bmp";
	rt->write_bmp(fss.str());

	getchar();
}