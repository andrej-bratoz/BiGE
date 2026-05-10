#pragma once
#include <bige-transform.h>
#include <glm/common.hpp>
#include <glm/glm.hpp>

#include <cstdint>
#include <cstring>
#include <array>
#include <type_traits>

namespace {
	template<typename T>
	std::array<uint8_t, sizeof(T)> to_little_endian_bytes(T value) {
		static_assert(std::is_arithmetic_v<T>, "T must be numeric");

		using UInt = std::conditional_t<sizeof(T) == 1, uint8_t,
			std::conditional_t<sizeof(T) == 2, uint16_t,
			std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>>>;

		UInt bits;
		std::memcpy(&bits, &value, sizeof(T));

		std::array<uint8_t, sizeof(T)> bytes;
		for (size_t i = 0; i < sizeof(T); ++i)
			bytes[i] = static_cast<uint8_t>(bits >> (8 * i));
		return bytes;
	}

	template<typename T>
	T from_little_endian_bytes(const std::array<uint8_t, sizeof(T)>& bytes) {
		static_assert(std::is_arithmetic_v<T>, "T must be numeric");

		using UInt = std::conditional_t<sizeof(T) == 1, uint8_t,
			std::conditional_t<sizeof(T) == 2, uint16_t,
			std::conditional_t<sizeof(T) == 4, uint32_t, uint64_t>>>;

		UInt bits = 0;
		for (size_t i = 0; i < sizeof(T); ++i)
			bits |= static_cast<UInt>(bytes[i]) << (8 * i);

		T value;
		std::memcpy(&value, &bits, sizeof(T));
		return value;
	}
}

namespace bige {

	struct buffer {
		uint32_t count = 0;     // number of bytes currently stored (clearer than a wraparound index)
		uint32_t capacity = 0;
		uint8_t* data = nullptr;

		buffer() = default;
		buffer(const buffer&) = delete;
		buffer& operator=(const buffer&) = delete;

		void push(uint8_t b) {
			if (count >= capacity) {
				uint32_t new_capacity = capacity == 0 ? 16 : capacity * 2;
				void* tmp = realloc(data, new_capacity);
				if (!tmp) return; // original 'data'/'capacity' untouched, allocation untouched
				data = (uint8_t*)tmp;
				capacity = new_capacity;
			}
			data[count++] = b;
		}

		// Returns pointer to the start of the last `n` bytes and removes them.
		// Caller must use the pointer before any subsequent push() (it may be invalidated by realloc).
		uint8_t* pop(uint32_t n) {
			if (n > count) return nullptr; // not enough data
			count -= n;
			return data + count;
		}

		~buffer() {
			free(data);
		}
	};

	template<typename T>
	void push(const T&, buffer& b) { static_assert(false); }

	template<typename T>
	void pop(T&, buffer& b) { static_assert(false); }

	template<>
	void push(const uint32_t& u, buffer& b) {
		auto arr = to_little_endian_bytes(u);
		for (auto byte : arr) { b.push(byte); }
	}

	template<>
	void pop(uint32_t& u, buffer& b) {
		auto* bytes = b.pop(sizeof(uint32_t));
		std::array<uint8_t, sizeof(uint32_t)> arr;
		for (size_t i = 0; i < sizeof(uint32_t); ++i) {
			arr[i] = static_cast<uint8_t>(bytes[i]);
		}
		u = from_little_endian_bytes<uint32_t>(arr);
	}

	template<>
	void push(const int& i, buffer& b) {
		auto arr = to_little_endian_bytes(i);
		for (auto byte : arr) { b.push(byte); }
	}

	template<>
	void pop(int& i, buffer& b) {
		auto* bytes = b.pop(sizeof(int));
		std::array<uint8_t, sizeof(int)> arr;
		for (size_t i = 0; i < sizeof(int); ++i) {
			arr[i] = static_cast<uint8_t>(bytes[i]);
		}
		i = from_little_endian_bytes<int>(arr);
	}

	template<>
	void push(const float& f, buffer& b) {
		auto arr = to_little_endian_bytes(f);
		for (auto byte : arr) { b.push(byte); }
	}

	template<>
	void pop(float& f, buffer& b) {
		auto* bytes = b.pop(sizeof(float));
		std::array<uint8_t, sizeof(float)> arr;
		for (size_t i = 0; i < sizeof(float); ++i) {
			arr[i] = static_cast<uint8_t>(bytes[i]);
		}
		f = from_little_endian_bytes<float>(arr);
	}

	template<>
	void push(const glm::vec2& v, buffer& b) {
		push(v.x, b);
		push(v.y, b);
	}

	template<>
	void pop(glm::vec2& v, buffer& b) {
		pop(v.y, b);
		pop(v.x, b);
	}

	template<>
	void push(const glm::vec3& v, buffer& b) {
		push(v.x, b);
		push(v.y, b);
		push(v.z, b);
	}

	template<>
	void pop(glm::vec3& v, buffer& b) {
		pop(v.z, b);
		pop(v.y, b);
		pop(v.x, b);
	}

	template<>
	void push(const glm::vec4& v, buffer& b) {
		push(v.x, b);
		push(v.y, b);
		push(v.z, b);
		push(v.w, b);
	}

	template<>
	void pop(glm::vec4& v, buffer& b) {
		pop(v.w, b);
		pop(v.z, b);
		pop(v.y, b);
		pop(v.x, b);
	}

	template<>
	void push(const glm::quat& q, buffer& b) {
		push(q.x, b);
		push(q.y, b);
		push(q.z, b);
		push(q.w, b);
	}

	template<>
	void pop(glm::quat& q, buffer& b) {
		pop(q.w, b);
		pop(q.z, b);
		pop(q.y, b);
		pop(q.x, b);
	}

	template<>
	void push(const glm::mat4& m, buffer& b) {
		push(m[0], b);
		push(m[1], b);
		push(m[2], b);
		push(m[3], b);
	}

	template<>
	void pop(glm::mat4& m, buffer& b) {
		pop(m[3], b);
		pop(m[2], b);
		pop(m[1], b);
		pop(m[0], b);
	}

	template<>
	void push(const std::string& s, buffer& b) {
		push(static_cast<uint32_t>(s.size()), b);
		for (char c : s) {
			b.push(static_cast<uint8_t>(c));
		}
		uint32_t size = s.size();
		push(size, b);
	}

	template<>
	void pop(std::string& s, buffer& b) {
		uint32_t size;
		pop(size, b);
		s.resize(size);
		for (uint32_t i = 0; i < size; ++i) {
			uint8_t byte = b.pop(1)[0];
			s[size - 1 - i] = static_cast<char>(byte);
		}
	}

	template<>
	void push(const transform& t, buffer& b) {
		push(t.world_matrix, b);
	}

	template<>
	void pop(transform& t, buffer& b) {
		pop(t.world_matrix, b);
	}

	struct serializer {
		
	};
}