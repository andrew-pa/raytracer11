#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
#include "basic_renderer.h"
#include "path_tracing_renderer.h"
#include "bvh_node.h"
#include "triangle_mesh.h"
#include "grid.h"
using namespace raytracer11;

struct test_mat : public path_tracing_material
{
	vec3 R;
	float percent_cone;
	float cone_radius;

	test_mat(vec3 r, float pc = .8f, float cr = .2f)
		: R(r), percent_cone(.8f), cone_radius(cr), path_tracing_material(vec3(0)) {}

	vec3 brdf(vec3 ki, vec3 ko, const hit_record&)	override
	{
		return R;
	}

	float pdf(vec3 ki, vec3, vec3 n)
	{
		return 1.f;
	}

	vec3 random_ray(vec3 n, vec3 ki) override
	{
		if (linearRand(0.f, 1.f) > .8f)
			return cone_distribution(n, .16f);
		else
			return cosine_distribution(n);
	}
};

vec3 schlick(vec3 c, float dkih)
{
	return c + (vec3(1.f) - c)*pow(1.f - (dkih), 5.f);
}

struct rlm_mat : public path_tracing_material
{
	vec3 Rd;
	vec3 Rs;
	vec2 n;

	rlm_mat(vec3 rd, vec3 rs, vec2 _n = vec2(100,100))
		: Rd(rd), Rs(rs), n(_n), path_tracing_material(vec3(0)) {}

	vec3 brdf(vec3 ki, vec3 ko, const hit_record& hr)	override
	{
		vec3 n = hr.norm;
		float cof = glm::sqrt((n.x + 1.f) * (n.y + 1.f)) / (8.f * pi<float>());
		vec3 h = normalize(ki + ko);
		vec3 u = vec3(0);
		vec3 v = vec3(0);
		make_orthonormal(n, u, v);
		float num = (n.x*(dot(h, u)*dot(h, u))) + (n.y*(dot(h, v)*dot(h, v))) / (1-(dot(h,n)*dot(h,n)));
		float dom = dot(h, ki) * glm::max(dot(n, ki), dot(n, ko));
		vec3 spc = cof * pow(dot(n, h), (num / dom)) * schlick(Rs, dot(ki, h));
		return spc+Rd;
	}

	float pdf(vec3 ki, vec3 ko, vec3 n)
	{
		vec3 h = normalize(ki + ko);
		vec3 u = vec3(0);
		vec3 v = vec3(0);
		make_orthonormal(n, u, v);
		float num = (n.x*(dot(h, u)*dot(h, u))) + (n.y*(dot(h, v)*dot(h, v))) / (1-(dot(h,n)*dot(h,n)));
		float cof = glm::sqrt((n.x + 1.f) * (n.y + 1.f)) / (2.f * pi<float>());
		float ph = cof*pow(dot(n, h),num);
		return ph / (4.f*dot(ki,h));
	}

	inline float cos2(float x) { return cos(x)*cos(x); }
	inline float sin2(float x) { return sin(x)*sin(x); }
	vec3 make_h(vec3 n)
	{
		float e1 = float(std::rand()) / float(RAND_MAX);
		float e2 = float(std::rand()) / float(RAND_MAX);
		float phi = atan(glm::sqrt((n.x + 1) / (n.y + 1)) * tan((pi<float>()*e1) / 2));
		float theta = acos(pow(1 - e2, 1 / (n.x*cos2(phi) + n.y*sin2(phi + 1))));

		if (std::rand() % 10 > 5) phi = -phi;
		if (std::rand() % 10 > 5) theta = -theta;

		vec3 w = n;
		vec3 u, v;
		make_orthonormal(w, u, v);

		float x = sin(theta)*cos(phi);
		float y = sin(theta)*sin(phi);
		float z = cos(theta);
		return normalize(u * x + v * y + w * z);
	}

	vec3 random_ray(vec3 n, vec3 ki) override
	{
		vec3 h = make_h(n);
		
		return h;
	}
};

//template <typename T>
//struct prop
//{
//	typedef T type;
//
//	prop(T* x)
//		: _d(x){}
//
//	T* _d;
//
//	inline operator T&()
//	{
//		return *_d;
//	}
//
//	inline prop<T>& operator =(const T& x)
//	{
//		*_d = x;
//		return *this;
//	}
//
//	inline T& operator --() const
//	{
//		return *_d;
//	}
//};
//
//class noodle
//{
//	string* _name;
//	vec2 _len;
//public:
//	noodle(string& n, vec2 l)
//		: _name(&n), _len(l) { }
//
//	const prop<string*> namex = prop<string*>(&_name);
//
//	inline prop<string*> name()
//	{
//		return prop<string*>(&_name);
//	}
//
//	inline prop<vec2> length() { return prop<vec2>(&_len); }
//}

int path_main()
{
	//noodle n(string("bob"), vec2(20));
	//noodle x(string("joe"), vec2(25));
	//n.name() = new string("noo");
	//string* ffa = n.name();
	//auto s = n.name()--->size();
	//s = n.namex--->size();
	//n.namex-- = new string("boo");

	//auto l = n.length()-- + vec2(1);
	//n.length()--.length();

	srand(time(nullptr));
	texture2d* rt = new texture2d(uvec2(640, 480));
	camera cam(vec3(0, 5, -14)/*vec3(2, 3, 8)*/, vec3(0), (vec2)rt->size(), 1.f);

	vector<surface*> objects;
	objects.push_back(new box(vec3(0, 4, 0), vec3(1.f, .05f, 1.f),
		new emmisive_material(vec3(5))));
	//objects.push_back(new sphere(vec3(1.5f, 1, -1.6f), .2f,
	//	new emmisive_material(vec3(8, 4.f, 0))));
	objects.push_back(new box(vec3(0, 0.f, 0), vec3(6, .1f, 6),
		new diffuse_material(vec3(.3f))));
	//objects.push_back(new sphere(vec3(0, 1, 0), .75f,
	//	new test_mat(vec3(.6f, .3f, .1f))));
	//objects.push_back(new sphere(vec3(-2.2f, 1.f, 4.f), .75f,
	//	new diffuse_material(vec3(.3f, 0, 0))));
	objects.push_back(new sphere(vec3(-1.6f, 1.f, -1.5f), .8f,
                               new rlm_mat(vec3(0.,.5,1.), vec3(.2, .4, .5)) ));

	//triangle_mesh<bvh_node>* t = new triangle_mesh<bvh_node>("teapot.obj",
	//	new diffuse_material(vec3(.1f, .2f, .6f)));
	//objects.push_back(t);

	/*vector<triangle_mesh<bvh_node>::vertex> v;
	vector<uint> i;
	v.push_back(triangle_mesh<bvh_node>::vertex(vec3(0, 0, 0), vec3(0, 0.5, -1), vec2(0, 0)));
	v.push_back(triangle_mesh<bvh_node>::vertex(vec3(10, 0, 0), vec3(0, 0.5, -1), vec2(1, 0)));
	v.push_back(triangle_mesh<bvh_node>::vertex(vec3(0, 0, 10), vec3(0, 0.5, -1), vec2(0, 1)));
	v.push_back(triangle_mesh<bvh_node>::vertex(vec3(10, 0, 10), vec3(0, 0.5, -1), vec2(1, 1)));
	i.push_back(0); i.push_back(1); i.push_back(2);
	i.push_back(1); i.push_back(2); i.push_back(3);
	triangle_mesh<bvh_node>* t = new triangle_mesh<bvh_node>(v, i, new diffuse_material(vec3(0,0,.6f)));
	objects.push_back(t);*/

	//objects.push_back(new sphere(vec3(3.f, 1.f, -2.3f), .75f,
	//	new rlm_mat(vec3(0.f, 5.f, 1.f), vec3(.8f), vec2(100000))));

	bvh_node* sc = new bvh_node(objects);
	//group* sc = new group(objects);
	//grid<8, 8, 8>* sc = new grid<8,8,8>(objects);

	path_tracing_renderer rd(cam, sc, rt, vec2(32));

	rd.aa_samples(60);
  
  cout << "render starting: [AA: " << rd.aa_samples() << ", tile size: " << rd.tile_size() << ", object count: " << objects.size() << "]" << endl;

#ifdef WRITE_WP_PERF_DATA
	auto start_time = chrono::system_clock::now();
#endif
	rd.render();
#ifdef WRITE_WP_PERF_DATA
	auto end_time = chrono::system_clock::now();

	long long tus = (chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count());
	auto tms = (double)tus / 1000000.0;
	cout << "render took: " << tms << "ms" << endl;
#endif
	ostringstream otxt;
	otxt << "RENDERED IN " << tms << "MS" << endl;
	otxt << "SAMPLES: " << rd.aa_samples() << endl;
	otxt << "TILE SIZE: [" << rd.tile_size().x << ", " << rd.tile_size().y << "]" << endl;
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
	bvh_node* sc = new bvh_node(objs, 3);
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
	auto tms = tus/1000000.0;
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
