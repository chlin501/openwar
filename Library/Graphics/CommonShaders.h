#ifndef CommonShaders_H
#define CommonShaders_H

#include "ShaderProgram.h"


struct renderers
{
	static renderers* singleton;

	ShaderProgram2<glm::vec2, glm::vec2>* _distance_renderer;
	ShaderProgram2<glm::vec2, glm::vec4>* _gradient_renderer;
	ShaderProgram2<glm::vec3, glm::vec4>* _gradient_renderer3;
	ShaderProgram2<glm::vec2, glm::vec2>* _ground_renderer;
	ShaderProgram1<glm::vec2>* _plain_renderer;
	ShaderProgram1<glm::vec3>* _plain_renderer3;
	ShaderProgram2<glm::vec2, glm::vec2>* _texture_renderer;
	ShaderProgram2<glm::vec3, glm::vec2>* _texture_renderer3;
	ShaderProgram2<glm::vec2, glm::vec2>* _opaque_texture_renderer;
	ShaderProgram2<glm::vec2, glm::vec2>* _alpha_texture_renderer;

	renderers();
};


#endif
