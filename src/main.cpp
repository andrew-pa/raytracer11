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

color_property load_color(const picojson::value& v) {
	if (v.is<picojson::array>()) return loadv3(v);
	else if(v.is<picojson::object>()) {
		auto ov = v.get<picojson::object>();
		auto ty = ov["type"].get<string>();
		if(ty == "checker") {
			return color_property(new checker_texture(loadv3(ov["color0"]), loadv3(ov["color1"]), 
				(float)ov["size"].get<double>()));
		} else if(ty == "bmp") {
			return color_property(new texture2d(ov["path"].get<string>()));
		}
	}
}

material* load_material(const picojson::value& v) {
	auto mj = v.get<picojson::value::object>();
	if(mj["type"].get<string>() == "diffuse") {
		return new diffuse_material(load_color(mj["color"]));
	} else if(mj["type"].get<string>() == "emission") {
		return new emmisive_material(load_color(mj["color"]));
	}
}

int main(int argc, char* argv[]) {
	srand(time(nullptr));
	vector<string> args; for(int i = 1; i < argc; ++i) args.push_back(argv[i]);
	
	if(args.size() == 0) {
		cout << "Need at least a scene file";
		return -1;
	}

	uint samples_override = 0;
	uvec2 res_override = uvec2(0);

	string scene_filename = args[0];

	if (args.size() > 1) {
		for (uint i = 1; i < args.size(); ++i) {
			if (args[i] == "-s") {
				samples_override = atoi(args[++i].c_str());
			}
			else if (args[i] == "-r") {
				res_override.x = atoi(args[++i].c_str());
				res_override.y = atoi(args[++i].c_str());
			}
		}
	}

	ifstream scenef(scene_filename);
	picojson::value vscenej;
	string err;
	istream_iterator<char> scenefi{scenef};
	picojson::parse(vscenej, scenefi, istream_iterator<char>(),  &err);
	if(!err.empty()) {
		cerr << "Scene parse error: " << err << endl;

	}
	auto scenej = vscenej.get<picojson::value::object>();


	texture2d* rdt = new texture2d(res_override.x > 0 ? res_override : uvec2(loadv2(scenej["resolution"])));
	
	cout << "Rendering " << scene_filename << " @ [" << rdt->size().x << ", " << rdt->size().y << "]" << endl; 
	
	auto camj = scenej["camera"].get<picojson::value::object>();
	camera cam(loadv3(camj["position"]), loadv3(camj["target"]), (vec2)rdt->size(), 1.f);



	vector<surface*> objects;
	for(const auto& vobjj : scenej["objects"].get<picojson::value::array>()) {
		auto objj = vobjj.get<picojson::value::object>();
		auto type = objj["type"].get<string>();
		auto mat = load_material(objj["material"]);
		if(type == "sphere") {
			objects.push_back(new sphere(loadv3(objj["center"]), objj["radius"].get<double>(), mat));
		} else if(type == "box") {
			objects.push_back(new box(loadv3(objj["center"]), loadv3(objj["extent"]), mat));
		} else if (type == "mesh") {
			mat4 w = mat4(1);
			if (objj["scale"].is<picojson::array>()) w = scale(w, loadv3(objj["scale"]));
			if (objj["rotation-axis"].is<picojson::array>()) w = rotate(w, (float)objj["rotation-angle"].get<double>(), loadv3(objj["rotation-axis"]));
			if (objj["position"].is<picojson::array>()) w = translate(w, loadv3(objj["position"]));
			objects.push_back(new triangle_mesh<bvh_node>(objj["path"].get<string>(), mat, w));
		}

	}
	bvh_node* sc = new bvh_node(objects);
	
	vec2 tilesize = vec2(32);
	if(!scenej["tile-size"].is<picojson::null>()) tilesize = loadv2(scenej["tile-size"]);

	path_tracing_renderer rd(cam, sc, rdt, tilesize); 
	
	rd.aa_samples(samples_override > 0 ? samples_override : 
			scenej["samples"].is<double>() ? scenej["samples"].get<double>() : 64);

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
		otxt << "RENDERED " << scene_filename << " IN " << tms << "MS" << endl;
		otxt << "TILE SIZE [" << rd.tile_size().x << ", " << rd.tile_size().y << "] @ " << rd.aa_samples() << " SAMPLES" << endl;
		rdt->draw_text(otxt.str(), uvec2(4), vec3(.2f));
		rdt->draw_text(otxt.str(), uvec2(1), vec3(1.f, 1.f, 0.f));
	}
	ostringstream fss;
	if(scenej["filename"].is<string>()) fss << scenej["filename"].get<string>() << time(nullptr) << ".bmp";
	else fss << "img" << time(nullptr) << ".bmp";
	rdt->write_bmp(fss.str());

	return 0;
}
