// Copyright (C) 2013 Felix Ungman
//
// This file is part of the openwar platform (GPL v3 or later), see LICENSE.txt

#ifndef VERTEX_H
#define VERTEX_H

#ifdef OPENWAR_USE_XCODE_FRAMEWORKS
#if !TARGET_OS_IPHONE
#define glGenVertexArraysOES glGenVertexArraysAPPLE
#define glBindVertexArrayOES glBindVertexArrayAPPLE
#define glDeleteVertexArraysOES glDeleteVertexArraysAPPLE
#endif
#endif


template <class T1>
struct vertex1
{
	T1 _1;

	vertex1() { }
	vertex1(const T1& __1) : _1(__1) { }
};

template <class T1, class T2>
struct vertex2
{
	T1 _1;
	T2 _2;

	vertex2() { }
	vertex2(const T1& v1, const T2& v2) : _1(v1), _2(v2) { }
};

template <class T1, class T2, class T3>
struct vertex3
{
	T1 _1;
	T2 _2;
	T3 _3;

	vertex3() { }
	vertex3(const T1& v1, const T2& v2, const T3& v3) : _1(v1), _2(v2), _3(v3) { }
};

template <class T1, class T2, class T3, class T4>
struct vertex4
{
	T1 _1;
	T2 _2;
	T3 _3;
	T4 _4;

	vertex4() { }
	vertex4(const T1& v1, const T2& v2, const T3& v3, const T4& v4) : _1(v1), _2(v2), _3(v3), _4(v4) { }
};






struct plain_vertex
{
	glm::vec2 _position;

	plain_vertex() {}
	plain_vertex(glm::vec2 p) : _position(p) {}
};


struct plain_vertex3
{
	glm::vec3 _position;

	plain_vertex3() {}
	plain_vertex3(glm::vec3 p) : _position(p) {}
};


struct color_vertex
{
	glm::vec2 _position;
	glm::vec4 _color;

	color_vertex() {}
	color_vertex(glm::vec2 p, glm::vec4 c) : _position(p), _color(c) {}
};


struct color_vertex3
{
	glm::vec3 _position;
	glm::vec4 _color;

	color_vertex3() {}
	color_vertex3(glm::vec3 p, glm::vec4 c) : _position(p), _color(c) {}
};


struct texture_vertex
{
	glm::vec2 _position;
	glm::vec2 _texcoord;

	texture_vertex() {}
	texture_vertex(glm::vec2 p, glm::vec2 t) : _position(p), _texcoord(t) {}
};


struct texture_vertex3
{
	glm::vec3 _position;
	glm::vec2 _texcoord;

	texture_vertex3() {}
	texture_vertex3(glm::vec3 p, glm::vec2 t) : _position(p), _texcoord(t) {}
};


struct texture_alpha_vertex
{
	glm::vec2 _position;
	glm::vec2 _texcoord;
	float _alpha;

	texture_alpha_vertex() {}
	texture_alpha_vertex(glm::vec2 p, glm::vec2 t, float a) : _position(p), _texcoord(t), _alpha(a) {}
};


#endif

