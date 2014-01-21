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


typedef vertex1<glm::vec2> plain_vertex;
typedef vertex1<glm::vec3> plain_vertex3;
typedef vertex2<glm::vec2, glm::vec4> color_vertex;
typedef vertex2<glm::vec3, glm::vec4> color_vertex3;
typedef vertex2<glm::vec2, glm::vec2> texture_vertex;
typedef vertex2<glm::vec3, glm::vec2> texture_vertex3;
typedef vertex3<glm::vec2, glm::vec2, float> texture_alpha_vertex;


#endif
