#ifndef TerrainViewport_H
#define TerrainViewport_H

#include "Viewport.h"


class TerrainViewport : public Viewport
{
public:
	TerrainViewport(GraphicsContext* gc);
	virtual ~TerrainViewport();

	virtual glm::mat4 GetTransform() const;

};



#endif
