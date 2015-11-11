#pragma once
#include "cmmn.h"
#include "surface.h"
#include "bvh_node.h"
#include <cstring>

#ifdef ASSIMP
#include "assimp\scene.h"
		inline vec3 to_glm3(const aiVector3D& v) { return vec3(v.x, v.y, v.z); }
		inline vec2 to_glm2(const aiVector3D& v) { return vec2(v.x, v.y); }
		inline vec3 to_glm3(const aiColor3D& v) { return vec3(v.r, v.g, v.b); }
		inline vec4 to_glm4(const aiColor4D& v) { return vec4(v.r, v.g, v.b, v.a); }
#endif

namespace raytracer11 
{

	template <typename accl_struct>
	class triangle_mesh : public surface
	{
	public:
		struct vertex
		{
			vec3 pos, norm;
			vec2 texc;
			vertex() {}
			vertex(vec3 p, vec3 n, vec2 t)
				: pos(p), norm(n), texc(t){}
			
			bool equals(const vertex& b)
			{
				return pos.x == b.pos.x && pos.y == b.pos.y && pos.z == b.pos.z &&
					norm.x == b.norm.x && norm.y == b.norm.y && norm.z == b.norm.z &&
					texc.x == b.texc.x && texc.y == b.texc.y;
			}
		};

		
	protected:
		accl_struct* _acs;
		mat4 _world;
		mat4 _invworld;

		vector<vertex> _vertices;

		material* m;

		vec3 readvec3(ifstream& i)
		{
			float x, y, z;
			i >> x >> y >> z;
			return vec3(x, y, z);
		}
	public:
		class triangle : public surface
		{
		protected:
			triangle_mesh* _mesh;
			uint i[3];
			vec3 p[3];
			aabb _b;
			//vec3 _cntr;
		public:
			triangle(uint _0, uint _1, uint _2, triangle_mesh* m)
				: _mesh(m)
			{
				i[0] = _0; i[1] = _1; i[2] = _2;
				

				_b = aabb();
				for (int q = 0; q < 3; ++q) {
					p[q] = _mesh->_vertices[i[q]].pos;
					_b.add_point(p[q]);
				}

			//	_cntr = third<float>() * (mesh->_vertices[i[0]] + 
			//		mesh->_vertices[i[1]] + mesh->_vertices[i[2]]);
			}

			inline aabb bounds() const override { return _b; }
			//inline vec3 center() const override { return _cntr; }
			inline material* mat() const { return _mesh->m; }

			inline bool inside_of(const aabb& b) const override { return b.contains(_mesh->_vertices[i[0]].pos) || b.contains(_mesh->_vertices[i[1]].pos) || b.contains(_mesh->_vertices[i[1]].pos); }
		
			inline bool hit(const ray& r, hit_record& hr) override
			{
				const vec3 v0 = p[0];//_mesh->_vertices[i[0]].pos;
				const vec3 v1 = p[1];//_mesh->_vertices[i[1]].pos;
				const vec3 v2 = p[2];//_mesh->_vertices[i[2]].pos;

				float u, v;
				const vec3 e1 = v1 - v0;
				const vec3 e2 = v2 - v0;
				const vec3 pv = cross(r.d, e2);
				const float det = dot(e1,pv);
				if (det == 0)
					return false;
				const float idet = 1.f / det;
				const vec3 tv = r.e - v0;
				u = dot(tv,pv)*idet;
				if (u < 0 || u > 1.f)
					return false;
				const vec3 qv = cross(tv,e1);
				v = dot(r.d, qv) * idet;
				if (v < 0 || u + v > 1)
					return false;
				const float nt = dot(e2, qv)*idet;
				if(nt > 0 && nt < hr.t)
				{
					hr.t = nt;
					float w = 1 - (u + v);
					hr.norm = _mesh->_vertices[i[0]].norm*w +
						_mesh->_vertices[i[0]].norm*u + _mesh->_vertices[i[0]].norm*v;
					hr.texcoord = _mesh->_vertices[i[0]].texc*w + _mesh->_vertices[i[0]].texc*u +
						_mesh->_vertices[i[0]].texc*v;
					hr.hit_surface = _mesh;
					return true;
				}
				return false;
			}

			inline float hit(const ray& r, float xt) override
			{
				const vec3 v0 = p[0];//_mesh->_vertices[i[0]].pos;
				const vec3 v1 = p[1];//_mesh->_vertices[i[1]].pos;
				const vec3 v2 = p[2];//_mesh->_vertices[i[2]].pos;

				float u, v;
				vec3 e1 = v1 - v0;
				vec3 e2 = v2 - v0;
				vec3 pv = cross(r.d, e2);
				float det = dot(e1, pv);
				if (det == 0)
					return false;
				float idet = 1.f / det;
				vec3 tv = r.e - v0;
				u = dot(tv, pv)*idet;
				if (u < 0 || u > 1.f)
					return false;
				vec3 qv = cross(tv, e1);
				v = dot(r.d, qv) * idet;
				if (v < 0 || u + v > 1)
					return false;
				float nt = dot(e2, qv)*idet;
				if (nt > 0 && nt < xt)
				{
					return nt;
				}
				return -1;
			}
		};
		
		triangle_mesh(const vector<vertex>& v, const vector<uint>& i, material* _m,
			const mat4& w = mat4(1))
			: _world(w), _invworld(inverse(w)), _vertices(v), m(_m)
		{
			vector<surface*> tris;
			for (int q = 0; q < i.size(); q += 3)
			{
				tris.push_back(new triangle(i[q], i[q + 1], i[q + 2], this));
			}
			_acs = new accl_struct(tris);
		}

		triangle_mesh(const string& from_obj_file, material* _m, const mat4& w = mat4(1))
			 : _world(w), _invworld(inverse(w)), m(_m)
		{
			vector<vec3> poss;
			vector<vec3> norms;
			vector<vec2> texcoords;
			vector<uint> indices;

			ifstream inf(from_obj_file);
			char comm[256] = { 0 };

			while (inf)
			{
				inf >> comm;
				if (!inf) break;
				if (strcmp(comm, "#") == 0) continue;
				else if (strcmp(comm, "v") == 0)
					poss.push_back(readvec3(inf));
				else if (strcmp(comm, "vn") == 0)
					norms.push_back(readvec3(inf));
				else if (strcmp(comm, "vt") == 0)
				{
					float u, v;
					inf >> u >> v;
					texcoords.push_back(vec2(u, v));
				}
				else if (strcmp(comm, "f") == 0)
				{
					for (uint ifa = 0; ifa < 3; ++ifa)
					{
						vertex v;
						uint ip, in, it;
						inf >> ip;
						v.pos = poss[ip - 1];
						if ('/' == inf.peek())
						{
							inf.ignore();
							if ('/' != inf.peek())
							{
								inf >> it;
								v.texc = texcoords[it - 1];
							}
							if ('/' == inf.peek())
							{
								inf.ignore();
								inf >> in;
								v.norm = norms[in - 1];
							}
						}

						auto iv = find_if(_vertices.begin(), _vertices.end(), 
							[&](const vertex& a) { return v.equals(a); });
						if (iv == _vertices.end())
						{
							indices.push_back(_vertices.size());
							_vertices.push_back(v);
						}
						else
						{
							indices.push_back(std::distance(_vertices.begin(), iv));
						}
					}
				}
			}
			aabb bnds;
			vector<surface*> ts;
			for (auto iv = _vertices.begin(); iv != _vertices.end(); ++iv)
				bnds.add_point(iv->pos);
			for (uint ix = 0; ix < indices.size(); ix += 3)
			{
				ts.push_back(new triangle(indices[ix], indices[ix + 1], indices[ix + 2], this));
			}
			_acs = new accl_struct(ts);
		}

#ifdef ASSIMP
		inline mat4 to_glm4x4(const aiMatrix4x4& v)
		{
			return transpose(mat4(
				v.a1, v.a2, v.a3, v.a4,
				v.b1, v.b2, v.b3, v.b4,
				v.c1, v.c2, v.c3, v.c4,
				v.d1, v.d2, v.d3, v.d4));
		}
		triangle_mesh(const aiScene* scn, aiMesh* m, material* mat) : m(mat) {
			using namespace assimp_loader;
			auto node = scn->mRootNode->FindNode(m->mName.C_Str());
			_world = to_glm4x4(node->mTransformation);
			_invworld = inverse(_world);
			for (int i = 0; i < m->mNumVertices; ++i)
				_vertices.push_back(vertex(
					to_glm3(m->mVertices[i]),
					to_glm3(m->mNormals[i]),
					to_glm2(m->mTextureCoords[0][i])));
			vector<surface*> tris;
			for (int q = 0; q < m->mNumFaces; ++q) {
				auto f = m->mFaces[q];
				tris.push_back(new triangle(f.mIndices[0], f.mIndices[1], f.mIndices[2], this));
			}
			_acs = new accl_struct(tris);
		}
#endif

		~triangle_mesh()
		{
			delete _acs;
		}

		inline bool hit(const ray& r, hit_record& hr) override
		{
			ray tr = ray(vec3(_invworld*vec4(r.e,1)), vec3(_invworld*vec4(r.d,0)));
			if(_acs->hit(tr, hr))
			{
				hr.norm = vec3(transpose(_invworld)*vec4(hr.norm, 0));
				return true;
			}
			return false;
		}

		inline float hit(const ray& r, float xt) override
		{
			ray tr = ray(vec3(_invworld*vec4(r.e, 1)), vec3(_invworld*vec4(r.d, 0)));
			return _acs->hit(tr, xt);
		}

		inline aabb bounds() const override
		{
			aabb b = _acs->bounds();
			return b.transform(_world);
		}

		inline material* mat() const override { return m; }
	};
}
