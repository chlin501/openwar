// Copyright (C) 2014 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef Vertex_H
#define Vertex_H

#include "GraphicsContext.h"
#include <glm/glm.hpp>
#include <vector>


template <typename T> struct GetVertexAttributeSize;

template <> struct GetVertexAttributeSize<GLbyte> { static const GLint value {1}; };
template <> struct GetVertexAttributeSize<GLubyte> { static const GLint value {1}; };
template <> struct GetVertexAttributeSize<GLshort> { static const GLint value {1}; };
template <> struct GetVertexAttributeSize<GLushort> { static const GLint value {1}; };
template <> struct GetVertexAttributeSize<GLfloat> { static const GLint value {1}; };

template <typename T> struct GetVertexAttributeSize<glm::tvec1<T, glm::highp>> { static const GLint value {1}; };
template <typename T> struct GetVertexAttributeSize<glm::tvec2<T, glm::highp>> { static const GLint value {2}; };
template <typename T> struct GetVertexAttributeSize<glm::tvec3<T, glm::highp>> { static const GLint value {3}; };
template <typename T> struct GetVertexAttributeSize<glm::tvec4<T, glm::highp>> { static const GLint value {4}; };


template <typename T> struct GetVertexAttributeType;

template <> struct GetVertexAttributeType<GLbyte> { static const GLenum value {GL_BYTE}; };
template <> struct GetVertexAttributeType<GLubyte> { static const GLenum value {GL_UNSIGNED_BYTE}; };
template <> struct GetVertexAttributeType<GLshort> { static const GLenum value {GL_SHORT}; };
template <> struct GetVertexAttributeType<GLushort> { static const GLenum value {GL_UNSIGNED_SHORT}; };
template <> struct GetVertexAttributeType<GLfloat> { static const GLenum value {GL_FLOAT}; };

template <typename T> struct GetVertexAttributeType<glm::tvec1<T, glm::highp>> { static const GLenum value {GetVertexAttributeType<T>::value}; };
template <typename T> struct GetVertexAttributeType<glm::tvec2<T, glm::highp>> { static const GLenum value {GetVertexAttributeType<T>::value}; };
template <typename T> struct GetVertexAttributeType<glm::tvec3<T, glm::highp>> { static const GLenum value {GetVertexAttributeType<T>::value}; };
template <typename T> struct GetVertexAttributeType<glm::tvec4<T, glm::highp>> { static const GLenum value {GetVertexAttributeType<T>::value}; };



struct VertexAttributeTraits
{
	const GLchar* name;
	GLint size;
	GLenum type;
	GLintptr offset;
};


template <typename... T>
struct Vertex;


template <>
struct Vertex<>
{
	static const int count = 0;
	static std::vector<VertexAttributeTraits> GetVertexAttributeTraits()
	{
		return { };
	}

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
	static const int count = sizeof...(Ts) + 1;
	T _v {};

	Vertex() {}
	Vertex(T v, Ts... vs) : Vertex<Ts...> {vs...}, _v {v} {}

	template <typename... N>
	static std::vector<VertexAttributeTraits> GetVertexAttributeTraits(const GLchar* name, N... names)
	{
		auto result = Vertex<Ts...>::GetVertexAttributeTraits(names...);
		using Vertex_ptr = Vertex<T, Ts...>*;
		result.push_back({
			name,
			GetVertexAttributeSize<T>::value,
			GetVertexAttributeType<T>::value,
			reinterpret_cast<GLintptr>(&Vertex_ptr{}->_v)
		});

		return result;
	}
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


using Vertex_1f = Vertex<float>;
using Vertex_2f = Vertex<glm::vec2>;
using Vertex_3f = Vertex<glm::vec3>;
using Vertex_4f = Vertex<glm::vec4>;

using Vertex_2f_4f = Vertex<glm::vec2, glm::vec4>;
using Vertex_2f_2f = Vertex<glm::vec2, glm::vec2>;
using Vertex_3f_2f = Vertex<glm::vec3, glm::vec2>;
using Vertex_3f_1f = Vertex<glm::vec3, float>;
using Vertex_3f_3f = Vertex<glm::vec3, glm::vec3>;
using Vertex_3f_4f = Vertex<glm::vec3, glm::vec4>;

using Vertex_2f_2f_1f = Vertex<glm::vec2, glm::vec2, float>;
using Vertex_3f_4f_1f = Vertex<glm::vec3, glm::vec4, float>;
using Vertex_2f_2f_2f = Vertex<glm::vec2, glm::vec2, glm::vec2>;

using Vertex_2f_2f_4f_1f = Vertex<glm::vec2, glm::vec2, glm::vec4, float>;
using Vertex_3f_1f_2f_2f = Vertex<glm::vec3, float, glm::vec2, glm::vec2>;


#endif
