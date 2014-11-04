#ifndef TextureResource_H
#define TextureResource_H

#include "Texture.h"
#include "resource.h"


class TextureResource : public Texture
{
public:
	TextureResource(GraphicsContext* gc, const resource& r);
	void LoadTextureFromResource(GraphicsContext* gc, const resource& r);

	virtual void UpdateTexture();
};


#endif
