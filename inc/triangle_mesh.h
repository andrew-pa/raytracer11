#pragma once
#include "cmmn.h"
#include "surface.h"
#include "bvh_node.h"

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
		};
	protected:
		accl_struct* _acs;
		mat4 _world;
		mat4 _invworld;

		vector<vertex> _vertices;

		material* m;
	public:
		class triangle : public surface
		{
		protected:
			triangle_mesh* _mesh;
			uint i[3];
			aabb _b;
			//vec3 _cntr;
		public:
			triangle(uint _0, uint _1, uint _2, triangle_mesh* m)
				: _mesh(m)
			{
				i[0] = _0; i[1] = _1; i[2] = _2;
				
				_b = aabb();
				for (int q = 0; q < 3; ++q) _b.add_point(_mesh->_vertices[i[q]].pos);

			//	_cntr = third<float>() * (mesh->_vertices[i[0]] + 
			//		mesh->_vertices[i[1]] + mesh->_vertices[i[2]]);
			}

			inline aabb bounds() const override { return _b; }
			//inline vec3 center() const override { return _cntr; }
			inline material* mat() const { return _mesh->m; }
		
			inline bool hit(const ray& r, hit_record& hr) override
			{
				vec3 v0 = _mesh->_vertices[i[0]].pos;
				vec3 v1 = _mesh->_vertices[i[1]].pos;
				vec3 v2 = _mesh->_vertices[i[2]].pos;

				float u, v;
				vec3 e1 = v1 - v0;
				vec3 e2 = v2 - v0;
				vec3 pv = cross(r.d, e2);
				float det = dot(e1,pv);
				if (det == 0)
					return false;
				float idet = 1.f / det;
				vec3 tv = r.e - v0;
				u = dot(tv,pv)*idet;
				if (u < 0 || u > 1.f)
					return false;
				vec3 qv = cross(tv,e1);
				v = dot(r.d, qv) * idet;
				if (v < 0 || u + v > 1)
					return false;
				float nt = dot(e2, qv)*idet;
				if(nt > 0 && nt < hr.t)
				{
					hr.t = nt;
					float w = 1 - (u + v);
					hr.norm = _mesh->_vertices[i[0]].norm*w +
						_mesh->_vertices[i[0]].norm*u + _mesh->_vertices[i[0]].norm*v;
					hr.texcord = _mesh->_vertices[i[0]].texc*w + _mesh->_vertices[i[0]].texc*u +
						_mesh->_vertices[i[0]].texc*v;
					hr.hit_surface = _mesh;
					return true;
				}
				return false;
			}

			inline float hit(const ray& r, float xt) override
			{
				vec3 v0 = _mesh->_vertices[i[0]].pos;
				vec3 v1 = _mesh->_vertices[i[1]].pos;
				vec3 v2 = _mesh->_vertices[i[2]].pos;

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

		}

		~triangle_mesh()
		{
			delete _acs;
		}

		inline bool hit(const ray& r, hit_record& hr) override
		{
			ray tr = ray(vec3(_invworld*vec4(r.e,1)), vec3(_invworld*vec4(r.d,1)));
			if(_acs->hit(tr, hr))
			{
				hr.norm = vec3(transpose(_invworld)*vec4(hr.norm, 0));
				return true;
			}
			return false;
		}

		inline float hit(const ray& r, float xt) override
		{
			ray tr = ray(vec3(_invworld*vec4(r.e, 1)), vec3(_invworld*vec4(r.d, 1)));
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