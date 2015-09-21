#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "parallel_tiles_renderer.h"
#include "path_tracing_renderer.h"
#include "bvh_node.h"
#include "triangle_mesh.h"
#include "picojson.h"
#include <iterator>
using namespace raytracer11;

vec2 loadv2(const picojson::value& v) {
	auto ar = v.get<picojson::value::array>();
	return vec2(ar[0].get<double>(), ar[1].get<double>());
}
vec3 loadv3(const picojson::value& v) {
	auto ar = v.get<picojson::value::array>();
	return vec3(ar[0].get<double>(), ar[1].get<double>(), ar[2].get<double>());
}

material* load_material(const picojson::value& v) {
	auto mj = v.get<picojson::value::object>();
	if(mj["type"].get<string>() == "diffuse") {
		return new diffuse_material(loadv3(mj["color"]));
	} else if(mj["type"].get<string>() == "emission") {
		return new emmisive_material(loadv3(mj["color"]));
	}
}

int main(int argc, char* argv[]) {
	srand(time(nullptr));
	vector<string> args; for(int i = 1; i < argc; ++i) args.push_back(argv[i]);
	
	if(args.size() == 0) {
		cout << "Need at least a scene file";
		return -1;
	}

	ifstream scenef(args[0]);
	picojson::value vscenej;
	string err;
	istream_iterator<char> scenefi{scenef};
	picojson::parse(vscenej, scenefi, istream_iterator<char>(),  &err);
	if(!err.empty()) {
		cerr << "Scene parse error: " << err << endl;

	}
	auto scenej = vscenej.get<picojson::value::object>();


	texture2d* rdt = new texture2d(uvec2(loadv2(scenej["resolution"])));
	
	cout << "Rendering " << args[0] << " @ [" << rdt->size().x << ", " << rdt->size().y << "]" << endl; 
	
	auto camj = scenej["camera"].get<picojson::value::object>();
	camera cam(loadv3(camj["position"]), loadv3(camj["target"]), (vec2)rdt->size(), 1.f);



	vector<surface*> objects;
	for(const auto& vobjj : scenej["objects"].get<picojson::value::array>()) {
		auto objj = vobjj.get<picojson::value::object>();
		auto type = objj["type"].get<string>();
		if(type == "sphere") {
			objects.push_back(new sphere(loadv3(objj["center"]), objj["radius"].get<double>(), 
				load_material(objj["material"])));
		} else if(type == "box") {
			objects.push_back(new box(loadv3(objj["center"]), loadv3(objj["extent"]), load_material(objj["material"])));
		}

	}

	bvh_node* sc = new bvh_node(objects);
	
	vec2 tilesize = vec2(32);
	if(!scenej["tile-size"].is<picojson::null>()) tilesize = loadv2(scenej["tile-size"]);

	path_tracing_renderer rd(cam, sc, rdt, tilesize); 
	
	rd.aa_samples(scenej["samples"].get<double>());

	cout << "render starting: [AA: " << rd.aa_samples() << ", tile size: " << rd.tile_size() << ", object count: " << objects.size() << "]" << endl;


	#ifdef WRITE_WP_PERF_DATA
	auto start_time = chrono::system_clock::now();
	#endif
	rd.render();
	#ifdef WRITE_WP_PERF_DATA
	auto end_time = chrono::system_clock::now();
	long long tus = (chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count());
	auto tms = (double)tus / 1000000.0;
	cout << "Render took: " << tms << "ms" << endl;
	#endif

	if(!scenej["watermark"].is<picojson::null>() && scenej["watermark"].get<bool>()) {
		ostringstream otxt;
		otxt << "RENDERED " << args[0] << " IN " << tms << "MS" << endl;
		otxt << "TILE [" << rd.tile_size().x << ", " << rd.tile_size().y << "] @ " << rd.aa_samples() << " SAMPLES" << endl;
		rdt->draw_text(otxt.str(), uvec2(3), vec3(.2f));
		rdt->draw_text(otxt.str(), uvec2(1), vec3(1.f, 1.f, 0.f));
	}
	ostringstream fss;
	if(scenej["filename"].is<string>()) fss << scenej["filename"].get<string>() << time(nullptr) << ".bmp";
	else fss << "img" << time(nullptr) << ".bmp";
	rdt->write_bmp(fss.str());

	return 0;
}
