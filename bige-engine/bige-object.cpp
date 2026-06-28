#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <bige-engine.h>
#include <bige-state_cb.h>
#include <bige-object.h>
#include <bige-utils.h>
#include <bige-scene.h>
#include <bige-logging.h>

void bige::objects::object_3d::dispatch(dispatch_call_id id, void* engine) {
	switch (id)
	{
		case bige::objects::object::dispatch_call_id::prepare:
		{
			on_prepare(engine);
			break;
		}
		case bige::objects::object::dispatch_call_id::draw:
		{
			on_draw(engine);
			break;
		}
		case bige::objects::object::dispatch_call_id::set_uniforms:
		{
			on_material(engine);
			break;
		}
		case bige::objects::object::dispatch_call_id::animate:
		{
			on_script(engine);
			break;
		}
		case bige::objects::object::dispatch_call_id::teardown:
		{
			on_teardown(engine);
			break;
		}
		case bige::objects::object::dispatch_call_id::move:
		{
			on_move(engine);
			break;
		}
		default:
			break;
	}
}

void bige::objects::object_3d::on_prepare(void* engine) {
	if (m_prepared) { return; }

	m_vaos.resize(m_model->primitives_count());
	m_vbos.resize(m_model->primitives_count());
	m_ebos.resize(m_model->primitives_count());
	int counter = 0;

	for (auto& primitive : m_model->primitives_ref()) {
		BIGE_DEFER(counter++;);
		if (!primitive->mesh->vertices_count()) { continue; }
		glGenVertexArrays(1, &m_vaos[counter]);
		glGenBuffers(1, &m_vbos[counter]);
		glBindVertexArray(m_vaos[counter]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbos[counter]);
		glBufferData(GL_ARRAY_BUFFER, primitive->mesh->vertices_count() * mesh::vertex::size(), primitive->mesh->vertices(), GL_STATIC_DRAW);

		if (primitive->mesh->indices_count()) {
			glGenBuffers(1, &m_ebos[counter]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebos[counter]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, primitive->mesh->indices_count() * sizeof(uint32_t), primitive->mesh->indices(), GL_STATIC_DRAW);
		}

		// position — location 0
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, mesh::vertex::size(), (void*)offsetof(mesh::vertex, position));

		// normal — location 1
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, mesh::vertex::size(), (void*)offsetof(mesh::vertex, normal));

		// uv — location 2
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, mesh::vertex::size(), (void*)offsetof(mesh::vertex, uv));

		// tangent — location 3
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, mesh::vertex::size(), (void*)offsetof(mesh::vertex, tangent));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		if (!primitive->material->compile_shaders()) {
			_log("object prepare");
			_l("failed to compile material shaders for primitive {}", counter);
			return;
		}
		primitive->material->fill_uniform_locations();
	}
	m_prepared = true;
}

void bige::objects::object_3d::on_draw(void* engine) {
	for (int i = 0; i < m_model->primitives_count(); i++) {
		auto& primitive = m_model->primitives_ref()[i];
		primitive->material->set_uniforms(primitive->transform); // bind this primitive's program/texture/uModel right before its draw call

		glBindVertexArray(m_vaos[i]);
		if (primitive->mesh->indices_count()) {
			glDrawElements(GL_TRIANGLES, primitive->mesh->indices_count(), GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawArrays(GL_TRIANGLES, 0, primitive->mesh->vertices_count());
		}
		glBindVertexArray(0);
	}
}

void bige::objects::object_3d::on_move(void* engine) {}

void bige::objects::object_3d::on_script(void* engine) {}

void bige::objects::object_3d::on_teardown(void* engine) {
	if (is_dead()) { return; }
	glUseProgram(0);
	for (int i = 0; i < m_model->primitives_count(); i++) {
		glDeleteVertexArrays(1, &m_vaos[i]);
		glDeleteBuffers(1, &m_vbos[i]);
		if (m_model->primitives_ref()[i]->mesh->indices_count()) {
			glDeleteBuffers(1, &m_ebos[i]);
		}
	}
	m_prepared = false;
}

void bige::objects::object_3d::on_material(void* engine) {
	for (auto& primitive : m_model->primitives_ref()) {
		primitive->material->set_uniforms(primitive->transform);
	}
}

void bige::objects::object_3d::destroy() {
	m_dead = true;
	on_teardown(nullptr);
	m_model.reset();
}

