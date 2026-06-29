#pragma once
#include <bige-model.h>
#include <bige-texture.h>
#include <tiny_gltf.h>
#include <memory>
#include <functional>

namespace bige {
	struct gltf_loader {
		
		gltf_loader() = default;
		std::unique_ptr<mesh> build_mesh(const tinygltf::Model& gltf, const tinygltf::Primitive& prim);
		std::unique_ptr<model> load(const std::string& path);
	private:
		void traverse_nodes(const tinygltf::Model& gltf, int node_idx, const glm::mat4& parent_transform, model& result,
			const std::vector<std::shared_ptr<bige::texture>>& loaded_textures);

	};
}