#include "TerrainViewport.h"


TerrainViewport::TerrainViewport(GraphicsContext* gc) : Viewport(gc)
{
}


TerrainViewport::~TerrainViewport()
{
}


glm::mat4 TerrainViewport::GetTransform() const
{
	return glm::mat4();
}
