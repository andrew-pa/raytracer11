#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "camera.h"
#include "renderer.h"
#include "parallel_tiles_renderer.h"
#include "basic_material_renderer.h"
#include "bvh_node.h"
using namespace raytracer11;



class noodle
{
public:
	string _name;
	int _age;
	noodle* _next;

	//struct name_property_type 
	//{
	//	string* itd;
	//	name_property_type(string* i)
	//		: itd(i)
	//	{}

	//	inline operator string()
	//	{
	//		return *itd;
	//	}

	//	inline string& operator =(string n)
	//	{
	//		*itd = n;
	//		return *itd;
	//	}
	//};

	//pproprw(string, name, _name);

	inline property_type<string> name()
	{
		return property_type<string>(&_name);
	}

	inline property_type<noodle*> next()
	{
		return property_type<noodle*>(&_next);
	}


	struct _____age_property_type : public property_type<int>
	{
		_____age_property_type(int* d)
			: property_type(d){}
		inline int& operator =(int n) override
		{
			if (n < 0 || n > 125) throw exception("too old!");
			*_data = n;
			return *_data;
		}
	};
	inline _____age_property_type age()
	{
		
		return _____age_property_type(&_age);
	}
};

int main()
{
	srand(time(nullptr));

	texture2d* rt = new texture2d(uvec2(640, 480));

	camera cam(vec3(0, 3, -10), vec3(0), (vec2)rt->size(), 1.f);

	sphere* s = new sphere(vec3(0), .75f, 
		new basic_material(vec3(0.1f,.8f,0), vec3(.2f,.8f,0), 500
		));

	vector<surface*> objs;
	/*objs.push_back(new sphere(vec3(0), .5f,
		new basic_material(vec3(.8f, .8f, .8f), vec3(1), 500)));
	objs.push_back(new sphere(vec3(1,0,0), .5f,
		new basic_material(vec3(.8f, 0, 0), vec3(1), 200)));
	objs.push_back(new sphere(vec3(-1,0,0), .5f,
		new basic_material(vec3(0, 0, .8f), vec3(1), 50)));
*/

	auto mat = new basic_material(vec3(.8f, .4f, 0), vec3(1, .7f, .3f), 200);

	for (float z = 0; z < 33; ++z)
	{
		objs.push_back(new sphere(glm::gaussRand(vec3(0), vec3(2.f)), 0.5f, mat));
	}

	auto vst = chrono::system_clock::now();
	bvh_node* sc = new bvh_node(objs);
	auto vet = chrono::system_clock::now();
	long vus = chrono::duration_cast<chrono::nanoseconds>(vet - vst).count();
	auto vms = (double)vus/1000000.0;
	cout << "bvh tree build took: " << vms << "ms" << endl;

	basic_material_renderer rd(cam, sc, rt);
	
	rd.lights().push_back(point_light(vec3(0, 4, 0), vec3(1)));
	rd.lights().push_back(point_light(vec3(4, 4, -4), vec3(1)));
	rd.lights().push_back(point_light(vec3(6, -4, -3), vec3(1)));

	auto start_time = chrono::system_clock::now();
	rd.render();
	auto end_time = chrono::system_clock::now();
	long tus = chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count();
	auto tms = (double)tus/1000000.0;
	cout << "render took: " << tms << "ms" << endl;


	ostringstream fss;
	fss << "img" << time(nullptr) << ".bmp";
	rt->write_bmp(fss.str());

	getchar();
}