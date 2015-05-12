#ifndef MapEditor_H
#define MapEditor_H

#include "Algebra/bounds.h"
class Image;

enum class TerrainFeature { Hills, Water, Trees, Fords };


class MapEditor
{
public:
	virtual ~MapEditor();

	virtual void Extract(glm::vec2 position, Image& brush) = 0;
	virtual bounds2f Paint(TerrainFeature feature, glm::vec2 position, float pressure, const Image& brush) = 0;
	virtual bounds2f Paint(TerrainFeature feature, glm::vec2 position, float pressure, float radius) = 0;

};



#endif
