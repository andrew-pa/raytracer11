#pragma once
#ifdef ASSIMP
#include "assimp\Importer.hpp"
#include "assimp\postprocess.h"
#include "assimp\scene.h"
#include "assimp\material.h"

#include "surface.h"
#include "pt_materials.h"
#include "texture.h"
#include "triangle_mesh.h"

namespace raytracer11 {
	namespace assimp_loader {
		vector<surface*> load_model(const string& fn) {
			string base_path = fn.substr(0, fn.find_last_of('\\')+1);
			Assimp::Importer imp;
			auto scn = imp.ReadFile(fn.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);
			vector<material*> mats;
			for (int i = 0; i < scn->mNumMaterials; ++i) {
				auto mat = scn->mMaterials[i];
				
				aiColor3D diffuse_c, emit_c;
				mat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_c);
				mat->Get(AI_MATKEY_COLOR_EMISSIVE, emit_c);
				aiString diffuse_tx_path, normals_tx_path;
				texture2d *diffusemap = nullptr, *normalmap = nullptr;
				if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_tx_path) == aiReturn_SUCCESS) {
					diffusemap = new texture2d(base_path+diffuse_tx_path.C_Str());
				}
				if (mat->GetTexture(aiTextureType_NORMALS, 0, &normals_tx_path) == aiReturn_SUCCESS) {
					normalmap = new texture2d(base_path+normals_tx_path.C_Str());
				}

				if (!emit_c.IsBlack()) {
					mats.push_back(new emmisive_material(color_property(to_glm3(emit_c), diffusemap)));
				}
				else {
					auto m = new diffuse_material(color_property(to_glm3(diffuse_c), diffusemap));
					m->normalmap = normalmap;
					mats.push_back(m);
				}
			}
			vector<surface*> surfs;
			for (int i = 0; i < scn->mNumMeshes; ++i) {
				surfs.push_back(new triangle_mesh<bvh_node>(scn, scn->mMeshes[i], mats[scn->mMeshes[i]->mMaterialIndex]));
			}
			return surfs;
		}
	}
}

#endif