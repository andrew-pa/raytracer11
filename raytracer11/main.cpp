#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
#include "basic_material_renderer.h"
using namespace raytracer11;

int main()
{
	texture2d* rt = new texture2d(uvec2(640, 480));

	camera cam(vec3(0, 2, -5), vec3(0), (vec2)rt->size(), 1.f);

	sphere* s = new sphere(vec3(0), .75f, new basic_material(vec3(0.1f,.8f,0), vec3(.2f,.8f,0), 64));

	basic_material_renderer rd(cam, s, rt);
	
	rd.lights().push_back(point_light(vec3(0, 4, -4), vec3(1)));

	rd.render();


	ostringstream fss;
	fss << "img" << time(nullptr) << ".bmp";
	rt->write_bmp(fss.str());
}