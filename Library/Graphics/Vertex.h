#ifndef Vertex_H
#define Vertex_H

#include <glm/glm.hpp>


template <class T1>
struct Vertex1
{
	T1 _1;
	Vertex1() { }
	Vertex1(const T1& __1) : _1(__1) { }
};
typedef Vertex1<float> Vertex_1f;
typedef Vertex1<glm::vec2> Vertex_2f;
typedef Vertex1<glm::vec3> Vertex_3f;
typedef Vertex1<glm::vec4> Vertex_4f;


template <class T1, class T2>
struct Vertex2
{
	T1 _1;
	T2 _2;
	Vertex2() { }
	Vertex2(const T1& v1, const T2& v2) : _1(v1), _2(v2) { }
};
typedef Vertex2<glm::vec2, glm::vec4> Vertex_2f_4f;
typedef Vertex2<glm::vec2, glm::vec2> Vertex_2f_2f;
typedef Vertex2<glm::vec3, glm::vec2> Vertex_3f_2f;
typedef Vertex2<glm::vec3, float> Vertex_3f_1f;
typedef Vertex2<glm::vec3, glm::vec3> Vertex_3f_3f;
typedef Vertex2<glm::vec3, glm::vec4> Vertex_3f_4f;


template <class T1, class T2, class T3>
struct Vertex3
{
	T1 _1;
	T2 _2;
	T3 _3;
	Vertex3() { }
	Vertex3(const T1& v1, const T2& v2, const T3& v3) : _1(v1), _2(v2), _3(v3) { }
};
typedef Vertex3<glm::vec2, glm::vec2, float> Vertex_2f_2f_1f;
typedef Vertex3<glm::vec3, glm::vec4, float> Vertex_3f_4f_1f;
typedef Vertex3<glm::vec2, glm::vec2, glm::vec2> Vertex_2f_2f_2f;


template <class T1, class T2, class T3, class T4>
struct Vertex4
{
	T1 _1;
	T2 _2;
	T3 _3;
	T4 _4;
	Vertex4() { }
	Vertex4(const T1& v1, const T2& v2, const T3& v3, const T4& v4) : _1(v1), _2(v2), _3(v3), _4(v4) { }
};
typedef Vertex4<glm::vec3, float, glm::vec2, glm::vec2> Vertex_3f_1f_2f_2f;



#endif
