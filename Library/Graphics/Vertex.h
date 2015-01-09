// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Vertex_H
#define Vertex_H

#include <glm/glm.hpp>
#include <vector>


template <typename... T>
struct Vertex
{
	static std::vector<std::size_t> GetSizes()
	{
		return { };
	}

	static std::vector<std::ptrdiff_t> GetOffsets()
	{
		return { };
	}
};

template <typename T, typename... Ts>
struct Vertex<T, Ts...> : Vertex<Ts...>
{
	T _v {};

	Vertex() {}
	Vertex(T v, Ts... vs) : Vertex<Ts...> {vs...}, _v {v} {}


	static std::vector<std::size_t> GetSizes()
	{
		std::vector<std::size_t> s = Vertex<Ts...>::GetSizes();
		s.insert(s.begin(), sizeof(T));
		return s;
	}

	static std::vector<std::ptrdiff_t> GetOffsets()
	{
		auto s = Vertex<Ts...>::GetOffsets();
		Vertex<T, Ts...>* p {nullptr};
		s.insert(s.begin(), reinterpret_cast<char*>(&p->_v) - reinterpret_cast<char*>(p));
		return s;
	};

};


template <int k, class... Ts>
struct VertexAttributeType;

template <class T, class... Ts>
struct VertexAttributeType<0, T, Ts...>
{
	typedef T type;
	typedef const T const_type;
};

template <int k, class T, class... Ts>
struct VertexAttributeType<k, T, Ts...>
{
	typedef typename std::enable_if<k != 0, typename VertexAttributeType<k - 1, Ts...>::type>::type type;
};


template <int k, class... Ts>
typename std::enable_if<k == 0, typename VertexAttributeType<0, Ts...>::type&>::type GetVertexAttribute(Vertex<Ts...>& vertex)
{
	return vertex._v;
}

template <int k, class... Ts>
typename std::enable_if<k == 0, typename VertexAttributeType<0, Ts...>::const_type&>::type GetVertexAttribute(const Vertex<Ts...>& vertex)
{
	return vertex._v;
}

template <int k, class T, class... Ts>
typename std::enable_if<k != 0, typename VertexAttributeType<k, T, Ts...>::type&>::type GetVertexAttribute(Vertex<T, Ts...>& vertex)
{
	Vertex<Ts...>& base = vertex;
	return GetVertexAttribute<k - 1, Ts...>(base);
}

template <int k, class T, class... Ts>
typename std::enable_if<k != 0, typename VertexAttributeType<k, T, Ts...>::const_type&>::type GetVertexAttribute(const Vertex<T, Ts...>& vertex)
{
	Vertex<Ts...>& base = vertex;
	return GetVertexAttribute<k - 1, Ts...>(base);
}


typedef Vertex<float> Vertex_1f;
typedef Vertex<glm::vec2> Vertex_2f;
typedef Vertex<glm::vec3> Vertex_3f;
typedef Vertex<glm::vec4> Vertex_4f;

typedef Vertex<glm::vec2, glm::vec4> Vertex_2f_4f;
typedef Vertex<glm::vec2, glm::vec2> Vertex_2f_2f;
typedef Vertex<glm::vec3, glm::vec2> Vertex_3f_2f;
typedef Vertex<glm::vec3, float> Vertex_3f_1f;
typedef Vertex<glm::vec3, glm::vec3> Vertex_3f_3f;
typedef Vertex<glm::vec3, glm::vec4> Vertex_3f_4f;

typedef Vertex<glm::vec2, glm::vec2, float> Vertex_2f_2f_1f;
typedef Vertex<glm::vec3, glm::vec4, float> Vertex_3f_4f_1f;
typedef Vertex<glm::vec2, glm::vec2, glm::vec2> Vertex_2f_2f_2f;

typedef Vertex<glm::vec2, glm::vec2, glm::vec4, float> Vertex_2f_2f_4f_1f;
typedef Vertex<glm::vec3, float, glm::vec2, glm::vec2> Vertex_3f_1f_2f_2f;


#endif
