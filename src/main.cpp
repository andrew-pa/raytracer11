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

	return 0;
}
