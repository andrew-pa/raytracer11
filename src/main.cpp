#include "cmmn.h"
#include "texture.h"
#include "surface.h"
#include "renderer.h"
#include "pt_materials.h"
#include "bvh_node.h"
#include "triangle_mesh.h"
#include "postprocesser.h"

#include "picojson.h"
#include "picolisp.h"
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

material* load_material(const picojson::value& v, map<string, material*>& named_materials) {
	if (v.is<string>()) {
		return named_materials[v.get<string>()];
	}
	auto mj = v.get<picojson::value::object>();
	string type = mj["type"].get<string>();
	if(type == "diffuse") {
		return new diffuse_material(load_color(mj["color"]));
	} else if(type == "emission") {
		return new emmisive_material(load_color(mj["color"]));
	} else if (type == "perfect-reflection") {
		return new perfect_reflection_material(load_color(mj["color"]));
	} else if (type == "perfect-refraction") {
		return new perfect_refraction_material(load_color(mj["color"]), (float)mj["eta"].get<double>());
	}
	else if (type == "GGX") {
		return new GGX_material(load_color(mj["color"]),
			(float)mj["alpha"].get<double>(),
			(float)mj["fresnel"].get<double>());
	}
}

int main(int argc, char* argv[]) {

	picojson::context cx;
	picojson::init_cmmn_lisp(cx);
	while (true) {
		picojson::value val;
		string s; getline(cin, s);
		picojson::parse(val, s); //R"({ "d1": (defvar x 3), "d2": (defun square (x) (* x x)), "math": (+ x (square 6)) })"
		auto nv = resolve(val, cx);
		cout << nv.serialize(true) << endl;
	}

	srand(time(nullptr));
	vector<string> args; for (int i = 1; i < argc; ++i) args.push_back(argv[i]);

	if (args.size() == 0) {
		cout << "Need at least a scene file";
		return -1;
	}

	uint samples_override = 0;
	uvec2 res_override = uvec2(0);
	int numt = -1;

	bool ovride_postprocess = false, tonemap_override = false;
	float gamma_override = 1.f, tonemap_white_override = 0.f;

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
			else if (args[i] == "-t") {
				numt = atoi(args[++i].c_str());
			}
			else if (args[i] == "-pp") {
				ovride_postprocess = true;
				for (uint j = i; j < args.size() && args[j] != ";"; ++j) {
					if (args[j] == "-tm") {
						tonemap_override = true;
						tonemap_white_override = atof(args[++j].c_str());
					}
					else if (args[j] == "-g") {
						gamma_override = atof(args[++j].c_str());
					}
				}
			}
		}
	}

	ifstream scenef(scene_filename);
	picojson::value vscenej;
	string err;
	istream_iterator<char> scenefi{ scenef };
	picojson::parse(vscenej, scenefi, istream_iterator<char>(), &err);
	if (!err.empty()) {
		cerr << "Scene parse error: " << err << endl;

	}
	auto scenej = vscenej.get<picojson::value::object>();


	shared_ptr<texture2d> rdt = make_shared<texture2d>(res_override.x > 0 ? res_override : uvec2(loadv2(scenej["resolution"])));

	cout << "Rendering " << scene_filename << " @ [" << rdt->size().x << ", " << rdt->size().y << "]" << endl;

	auto camj = scenej["camera"].get<picojson::value::object>();
	camera cam(loadv3(camj["position"]), loadv3(camj["target"]), (vec2)rdt->size(), 1.f);

	map<string, material*> named_materials;
	if (!scenej["materials"].is<picojson::null>()) {
		for (const auto& mt : scenej["materials"].get<picojson::value::object>()) {
			named_materials[mt.first] = load_material(mt.second, named_materials);
		}
	}

	vector<surface*> objects;
	for (const auto& vobjj : scenej["objects"].get<picojson::value::array>()) {
		auto objj = vobjj.get<picojson::value::object>();
		auto type = objj["type"].get<string>();
		auto mat = load_material(objj["material"], named_materials);
		if (type == "sphere") {
			objects.push_back(new sphere(loadv3(objj["center"]), objj["radius"].get<double>(), mat));
		}
		else if (type == "box") {
			objects.push_back(new box(loadv3(objj["center"]), loadv3(objj["extent"]), mat));
		}
		else if (type == "mesh") {
			mat4 w = mat4(1);
			if (objj["scale"].is<picojson::array>()) w = scale(w, loadv3(objj["scale"]));
			if (objj["rotation-axis"].is<picojson::array>()) w = rotate(w, (float)objj["rotation-angle"].get<double>(), loadv3(objj["rotation-axis"]));
			if (objj["position"].is<picojson::array>()) w = translate(w, loadv3(objj["position"]));
			objects.push_back(new triangle_mesh<bvh_node>(objj["path"].get<string>(), mat, w));
		}

	}
	shared_ptr<bvh_node> sc = make_shared<bvh_node>(objects);

	vec2 tilesize = vec2(32);
	if (!scenej["tile-size"].is<picojson::null>()) tilesize = loadv2(scenej["tile-size"]);

	renderer rd(cam, sc, rdt, samples_override > 0 ? samples_override :
		scenej["samples"].is<double>() ? scenej["samples"].get<double>() : 64, tilesize, numt);

	if (!scenej["env-map"].is<picojson::null>()) {
		auto envmp = scenej["env-map"].get<picojson::object>();
		rd.env_luma = (float)envmp["luma"].get<double>();
		rd.env_map = make_shared<textureCube>(vector<shared_ptr<texture<vec3, uvec2, vec2>>>{
			make_shared<texture2d>(envmp["x-"].get<string>()),
			make_shared<texture2d>(envmp["x+"].get<string>()),
			make_shared<texture2d>(envmp["y-"].get<string>()),
			make_shared<texture2d>(envmp["y+"].get<string>()),
			make_shared<texture2d>(envmp["z-"].get<string>()), 
			make_shared<texture2d>(envmp["z+"].get<string>()),
		});
	}

	cout << "render starting: [AA: " << rd.samples*rd.samples << ", tile size: " << rd.tile_size << ", object count: " << objects.size() << "]" << endl;


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
	
	if (scenej["postprocess"].is<picojson::object>() || ovride_postprocess) {
		::postprocessor ppr{ gamma_override, tonemap_override, tonemap_white_override, numt, tilesize };
		if (!ovride_postprocess) {
			auto ppj = scenej["postprocess"].get<picojson::object>();
			if (ppj["gamma"].is<double>()) ppr.inv_gamma = 1.f / ppj["gamma"].get<double>();
			if (ppj["tonemap"].is <bool>() && ppj["tonemap"].get<bool>()) {
				ppr.tonemap = true;
				ppr.tonemap_white = ppj["tonemap-white"].get<double>();
			}
		}
		
#ifdef WRITE_WP_PERF_DATA
		start_time = chrono::system_clock::now();
#endif
		ppr.render(rdt.get(), rdt.get());
#ifdef WRITE_WP_PERF_DATA
		end_time = chrono::system_clock::now();
		long long pps_tus = (chrono::duration_cast<chrono::nanoseconds>(end_time - start_time).count());
		auto pps_tms = (double)pps_tus / 1000000.0;
		cout << "Postprocess took: " << pps_tms << "ms" << endl;
#endif
	}

	if(!scenej["watermark"].is<picojson::null>() && scenej["watermark"].get<bool>()) {
		ostringstream otxt;
		otxt << "RENDERED " << 
			scene_filename.substr(scene_filename.size() > 64 ? scene_filename.length()-64 : 0) 
			<< " IN " << tms << "MS" << endl;
		otxt << "TILE SIZE [" << rd.tile_size.x << ", " << rd.tile_size.y << "] @ " << rd.samples*rd.samples << " SAMPLES" << endl;
		rdt->draw_text(otxt.str(), uvec2(4), vec3(.2f));
		rdt->draw_text(otxt.str(), uvec2(1), vec3(1.f, 1.f, 0.f));
	}
	ostringstream fss;
	if(scenej["filename"].is<string>()) fss << scenej["filename"].get<string>() << time(nullptr) << ".bmp";
	else fss << "img" << time(nullptr) << ".bmp";
	rdt->write_bmp(fss.str());

#ifdef WIN32
	getchar();
#endif

	return 0;
}
