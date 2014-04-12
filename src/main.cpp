#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
#include "basic_renderer.h"
#include "path_tracing_renderer.h"
#include "bvh_node.h"
using namespace raytracer11;

struct test_mat : public path_tracing_material
{
	vec3 R;

	test_mat(vec3 r)
		: R(r), path_tracing_material(vec3(0)) {}

	vec3 brdf(vec3 ki, vec3 ko)	override
	{
		return R;
	}

	vec3 random_ray(vec3 n, vec3 ki) override
	{
		if (linearRand(0.f, 1.f) > .8f)
			return cone_distribution(n, .25f);
		else
			return cosine_distribution(n);
	}
};

int path_main()
{
	srand(time(nullptr));
	texture2d* rt = new texture2d(uvec2(1280, 960));
	camera cam(vec3(0, 5, -14), vec3(0), (vec2)rt->size(), 1.f);

	vector<surface*> objects;
	objects.push_back(new box(vec3(0, 4, 0), vec3(1.f, .05f, 1.f),
		new emmisive_material(vec3(15))));
	objects.push_back(new box(vec3(3, 1, -3), vec3(.3f, 1.f, .3f),
		new emmisive_material(vec3(5, 2.5f, 0))));
	objects.push_back(new box(vec3(0, 0, 0), vec3(6, .1f, 6),
		new diffuse_material(vec3(.4f))));
	objects.push_back(new sphere(vec3(0, 1, 0), .75f,
		new test_mat(vec3(.6f, .3f, .1f))));
	objects.push_back(new sphere(vec3(-1.6f, 1.f, -1.5f), .75f,
		new diffuse_material(vec3(.8f, 0, 0))));

	bvh_node* sc = new bvh_node(objects);

	path_tracing_renderer rd(cam, sc, rt);

	rd.aa_samples(7000);

#ifdef WRITE_WP_PERF_DATA
	auto start_time = chrono::system_clock::now();
#endif
	rd.render();
#ifdef WRITE_WP_PERF_DATA
	auto end_time = chrono::system_clock::now();
	long tus = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count();
	auto tms = (double)tus / 1000000.0;
	cout << "render took: " << tms << "ms" << endl;
#endif
	ostringstream otxt;
	otxt << "RENDERED IN " << tms << "MS" << endl;
	otxt << "SAMPLES: " << rd.aa_samples() << endl;
	rt->draw_text(otxt.str(), uvec2(3), vec3(.2f));
	rt->draw_text(otxt.str(), uvec2(1), vec3(1,1,0));

	ostringstream fss;
	fss << "img" << time(nullptr) << ".bmp";
	rt->write_bmp(fss.str());

	getchar();
	return 0;
}

int basic_main()
{
	srand(time(nullptr));

	texture2d* rt = new texture2d(uvec2(1280, 960));

	camera cam(vec3(0, 5, -12), vec3(0), (vec2)rt->size(), 1.f);

	vector<surface*> objs;
	objs.push_back(new sphere(vec3(0,1,0), .75f,
		new basic_material(vec3(.4f, .4f, .5f), vec3(1), 700, vec3(.1f))));
	objs.push_back(new sphere(vec3(1.8f, 3, 1.2f), .75f,
		new basic_material(vec3(.8f, .4f, 0), vec3(1), 200)));
	objs.push_back(new sphere(vec3(1.5f,1,-2), .5f,
		new basic_material(vec3(.8f, 0, 0), vec3(1), 200)));
	objs.push_back(new sphere(vec3(-1.5f, 1, -2), .75f,
		new basic_material(vec3(.8f), vec3(1), 50, vec3(0),
		new checker_texture(vec3(1,1,0),vec3(0,1,0), 10.f))));

	objs.push_back(new box(vec3(0), vec3(6, .16f, 6),
		new basic_material(vec3(.2f), vec3(.2f), 10, vec3(0))));

#ifdef WRITE_WP_PERF_DATA
	auto vst = chrono::system_clock::now();
#endif
	bvh_node* sc = new bvh_node(objs);
#ifdef WRITE_WP_PERF_DATA
	auto vet = chrono::system_clock::now();
	long vus = chrono::duration_cast<chrono::nanoseconds>(vet - vst).count();
	auto vms = (double)vus/1000000.0;
	cout << "bvh tree build took: " << vms << "ms" << endl;
#endif

	basic_renderer rd(cam, sc, rt);
	
	rd.lights().push_back(point_light(vec3(0, 4, 0), vec3(1)));
	rd.lights().push_back(point_light(vec3(4, 4, -4), vec3(1,1,.7f)));
	rd.lights().push_back(point_light(vec3(-6, 4, -3), vec3(1,.7f,1)));

	rd.aa_samples(4);

#ifdef WRITE_WP_PERF_DATA
	auto start_time = chrono::system_clock::now();
#endif
	rd.render();
#ifdef WRITE_WP_PERF_DATA
	auto end_time = chrono::system_clock::now();
	long tus = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count();
	auto tms = (double)tus/1000000.0;
	cout << "render took: " << tms << "ms" << endl;
#endif
	ostringstream otxt;
	otxt << "RENDERED IN " << tms << "MS" << endl;
	otxt << "SAMPLES: " << rd.aa_samples() << endl;
	rt->draw_text(otxt.str(), uvec2(3), vec3(.2f));
	rt->draw_text(otxt.str(), uvec2(1), vec3(1));

	ostringstream fss;
	fss << "img" << time(nullptr) << ".bmp";
	rt->write_bmp(fss.str());

	getchar();
	return 0;
}

int main()
{
	return path_main();
}