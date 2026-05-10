#pragma once
#include <bige-model.h>
#include <bige-texture.h>
#include <tiny_gltf.h>
#include <memory>
#include <functional>

namespace bige {
	using material_factory = std::function<std::unique_ptr<material>(const tinygltf::Material* material, 
		std::shared_ptr<bige::texture> base_color_tex, std::shared_ptr<bige::texture> normal_tex, std::shared_ptr<bige::texture> rough_met)>;
	struct gltf_loader {
		
		gltf_loader() = default;
		std::unique_ptr<mesh> build_mesh(const tinygltf::Model& gltf, const tinygltf::Primitive& prim);
		std::unique_ptr<model> load(const std::string& path, const material_factory& make_material);
	private:
		void traverse_nodes(const tinygltf::Model& gltf, int node_idx, const glm::mat4& parent_transform, model& result, 
			const bige::material_factory& make_material, 
			const std::vector<std::shared_ptr<bige::texture>>& loaded_textures);

	};
}