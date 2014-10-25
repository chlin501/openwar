#ifndef VertexShape_H
#define VertexShape_H

#include "VertexBuffer.h"


class VertexShape
{
public:
	VertexShape();
	~VertexShape();
};


typedef VertexBuffer<Vertex_2f> VertexShape_2f;
typedef VertexBuffer<Vertex_3f> VertexShape_3f;

typedef VertexBuffer<Vertex_2f_2f> VertexShape_2f_2f;
typedef VertexBuffer<Vertex_2f_4f> VertexShape_2f_4f;
typedef VertexBuffer<Vertex_3f_1f> VertexShape_3f_1f;
typedef VertexBuffer<Vertex_3f_2f> VertexShape_3f_2f;
typedef VertexBuffer<Vertex_3f_3f> VertexShape_3f_3f;
typedef VertexBuffer<Vertex_3f_4f> VertexShape_3f_4f;

typedef VertexBuffer<Vertex_2f_2f_1f> VertexShape_2f_2f_1f;
typedef VertexBuffer<Vertex_3f_4f_1f> VertexShape_3f_4f_1f;
typedef VertexBuffer<Vertex_2f_2f_2f> VertexShape_2f_2f_2f;

typedef VertexBuffer<Vertex_3f_1f_2f_2f> VertexShape_3f_1f_2f_2f;


#endif
