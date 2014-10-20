#ifndef RenderCall_H
#define RenderCall_H

#include "GraphicsContext.h"



template <class T>
class Uniform
{
	T _value;
};


template <class _ShaderProgram>
class RenderCall
{
	_ShaderProgram* _shaderprogram;

public:
	typedef typename _ShaderProgram::VertexT VertexT;

	VertexBuffer<VertexT>* _vertices;

	RenderCall(GraphicsContext* gc) :
		_shaderprogram(gc->GetShaderProgram<_ShaderProgram>()),
		_vertices(nullptr)
	{
	}

	void Render()
	{
	}
};


#endif
