#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
#include "basic_material_renderer.h"
#include "bvh_node.h"
using namespace raytracer11;

int main()
{
	texture2d* rt = new texture2d(uvec2(640, 480));

	camera cam(vec3(0, 2, -5), vec3(0), (vec2)rt->size(), 1.f);

	sphere* s = new sphere(vec3(0), .75f, 
		new basic_material(vec3(0.1f,.8f,0), vec3(.2f,.8f,0), 500
		));

	vector<surface*> objs;
	objs.push_back(new sphere(vec3(0), .75f,
		new basic_material(vec3(0, .8f, 0), vec3(1), 500)));
	objs.push_back(new sphere(vec3(1,0,0), .75f,
		new basic_material(vec3(.8f, 0, 0), vec3(1), 200)));
	objs.push_back(new sphere(vec3(-1,0,0), .75f,
		new basic_material(vec3(0, 0, .8f), vec3(1), 50)));


	bvh_node* sc = new bvh_node(objs);

	basic_material_renderer rd(cam, sc, rt);
	
	rd.lights().push_back(point_light(vec3(0, 4, 0), vec3(2)));
	rd.lights().push_back(point_light(vec3(4, 4, -4), vec3(2)));
	rd.lights().push_back(point_light(vec3(6, -4, -3), vec3(2)));

	rd.render();


	ostringstream fss;
	fss << "img" << time(nullptr) << ".bmp";
	rt->write_bmp(fss.str());
}