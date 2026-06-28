#pragma once
#include <bige-mesh.h>
#include <bige-shader.h>
#include <bige-model.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <vector>

namespace bige { namespace objects {

	struct object {
		int64_t id() const { return m_id; }
		enum class dispatch_call_id {
			prepare = 1,
			set_uniforms = 100,
			draw = 200,
			move = 300,
			animate = 400,
			teardown = 500
		};
		//
		virtual void dispatch(dispatch_call_id id, void* engine) = 0;
		virtual bool is_dead() { return false; };


		virtual void on_prepare(void* engine) = 0;
		virtual void on_draw(void* engine) = 0;
		virtual void on_move(void* engine) = 0;
		virtual void on_script(void* engine) = 0;
		virtual void on_teardown(void* engine) = 0;
		virtual void on_material(void* engine) = 0;
		virtual void destroy() = 0;

	private:
		int64_t m_id;
	};

	struct object_3d : public object {
		virtual void dispatch(dispatch_call_id id, void* engine) override;
		virtual void on_prepare(void* engine) override;
		virtual void on_draw(void* engine) override;
		virtual void on_move(void* engine) override;
		virtual void on_script(void* engine) override;
		virtual void on_teardown(void* engine) override;
		virtual void on_material(void* engine) override;
		virtual bool is_dead() { return m_dead; };

		virtual void destroy() override;
		//
		void attach_model(std::unique_ptr<model> model) { m_model = std::move(model); }


	protected:
		std::unique_ptr<bige::model> m_model;
		bool m_prepared = false;
		bool m_dead = false;
		std::vector<GLuint> m_vaos;
		std::vector<GLuint> m_vbos;
		std::vector<GLuint> m_ebos;
	};
}}