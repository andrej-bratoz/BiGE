#include <bige-engine.h>
#include <bige-serializer.h>

#include <catch2/catch_all.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

TEST_CASE("BiGE Buffer int", "[bige]") {
	bige::buffer b;
	bige::push((int)5, b);
	//
	int f;
	bige::pop(f,b);
	REQUIRE(f == 5);
}

TEST_CASE("BiGE Buffer float", "[bige]") {
	bige::buffer b;
	int repr = 28732123;
	float f = *reinterpret_cast<float*>(&repr);
	bige::push(f, b);
	//
	float f2;
	bige::pop(f2, b);
	REQUIRE(f2 == f);
}

TEST_CASE("BiGE Buffer vec2", "[bige]") {
	bige::buffer b;
	glm::vec2 v(1.5f, -2.3f);
	bige::push(v, b);
	//
	glm::vec2 v2;
	bige::pop(v2, b);
	REQUIRE(v2 == v);
}

TEST_CASE("BiGE Buffer vec3", "[bige]") {
	bige::buffer b;
	glm::vec3 v(1.5f, -2.3f, 1.6f);
	bige::push(v, b);
	//
	glm::vec3 v2;
	bige::pop(v2, b);
	REQUIRE(v2 == v);
}

TEST_CASE("BiGE Buffer vec4", "[bige]") {
	bige::buffer b;
	glm::vec4 v(1.5f, -2.3f, 0.3f, 5.3f);
	bige::push(v, b);
	//
	glm::vec4 v2;
	bige::pop(v2, b);
	REQUIRE(v2 == v);
}

TEST_CASE("BiGE Buffer quat", "[bige]") {
	bige::buffer b;
	glm::quat q(1.0f, 1.4f, 1.6f, -3.9f);
	bige::push(q, b);
	//
	glm::quat q2;
	bige::pop(q2, b);
	REQUIRE(q2 == q);
}

TEST_CASE("BiGE Buffer mat4", "[bige]") {
	bige::buffer b;
	glm::mat4 m;
	m[0] = glm::vec4(1.1f, 2.2f, 3.6f, 4.1f);
	m[1] = glm::vec4(5.2f, 6.3f, 7.7f, 8.2f);
	m[2] = glm::vec4(9.3f, 10.4f, 11.8f, 12.3f);
	m[3] = glm::vec4(13.4f, 14.5f, 15.9f, 16.4f);
	bige::push(m, b);
	//
	glm::mat4 m2;
	bige::pop(m2, b);
	REQUIRE(m2 == m);
}

TEST_CASE("BiGE Buffer transform", "[bige]") {
	bige::buffer b;
	
	glm::mat4 m;
	m[0] = glm::vec4(1.1f, 2.2f, 3.6f, 4.1f);
	m[1] = glm::vec4(5.2f, 6.3f, 7.7f, 8.2f);
	m[2] = glm::vec4(9.3f, 10.4f, 11.8f, 12.3f);
	m[3] = glm::vec4(13.4f, 14.5f, 15.9f, 16.4f);
	bige::transform t(m);

	bige::push(t, b);
	//
	bige::transform t2;
	bige::pop(t2, b);
	REQUIRE(t2.world_matrix == t.world_matrix);
}

TEST_CASE("BiGE Buffer string", "[bige]") {
	bige::buffer b;
	std::string s = "Hello, BiGE! 123456678879091283098";
	bige::push(s, b);
	//
	std::string s2;
	bige::pop(s2, b);
	REQUIRE(s2 == s);
}
