#include "TextureAtlas.h"
#include "GraphicsContext.h"
#import "TextureFont.h"



TextureAtlas::TextureAtlas(GraphicsContext* gc) : Texture(gc),
	_gc(gc),
	_image(nullptr),
	_currentX(0),
	_currentY(0),
	_nextY(0),
	_dirty(false)
{
}


GraphicsContext* TextureAtlas::GetGraphicsContext() const
{
	return _gc;
}


TextureFont* TextureAtlas::GetTextureFont(const FontDescriptor& fontDescriptor)
{
	for (TextureFont* textureFont : _textureFonts)
		if (textureFont->GetFontDescriptor() == fontDescriptor)
			return textureFont;

	TextureFont* result = new TextureFont(this, fontDescriptor);
	_textureFonts.push_back(result);
	return result;
}


void TextureAtlas::LoadAtlasFromResource(const resource& r)
{
	_image = new Image();
	_image->LoadFromResource(r);
	LoadTextureFromImage(*_image);
}


void TextureAtlas::LoadTextureFromImage(const Image& image)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.GetWidth(), image.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.GetPixels());
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();
}



#ifdef OPENWAR_IMAGE_USE_SDL
void TextureAtlas::LoadTextureFromSurface(SDL_Surface* surface)
{
	SDL_Surface* tmp = nullptr;
	if (surface->format->format != SDL_PIXELFORMAT_ABGR8888)
	{
		tmp = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ABGR8888, 0);
		surface = tmp;
	}

	SDL_LockSurface(surface);

	glBindTexture(GL_TEXTURE_2D, id);
	CHECK_ERROR_GL();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
	CHECK_ERROR_GL();
	glGenerateMipmap(GL_TEXTURE_2D);
	CHECK_ERROR_GL();

	SDL_UnlockSurface(surface);

	if (tmp != nullptr)
		SDL_FreeSurface(tmp);
}
#endif


void TextureAtlas::UpdateTexture()
{
	if (_dirty)
	{
		if (_image != nullptr)
			LoadTextureFromImage(*_image);
		_dirty = false;
	}
}


TextureImage* TextureAtlas::AddTextureImage(Image* image)
{
	if (_image == nullptr)
		_image = new Image(512, 512);

	int width = image->GetWidth();
	int height = image->GetHeight();

	if (_currentX + width > _image->GetWidth())
	{
		_currentX = 0;
		_currentY = _nextY;
	}

	_image->Copy(*image, _currentX, _currentY);

	bounds2f bounds(_currentX, _currentY, _currentX + width, _currentY + height);

	_currentX += width;
	_nextY = glm::max(_nextY, _currentY + height);

	_dirty = true;

	return GetTextureImage(bounds, bounds);
}


TextureImage* TextureAtlas::GetTextureImage(const bounds2f& inner, const bounds2f& outer)
{
	TextureImage* result = new TextureImage();

	result->_textureAtlas = this;
	result->_inner = inner;
	result->_outer = outer;

	_images.push_back(result);

	return result;
}


/***/


TextureImage::TextureImage() :
	_textureAtlas(nullptr)
{
}


TextureImage::~TextureImage()
{

}


bounds2f TextureImage::GetInnerBounds() const
{
	return _inner;
}


bounds2f TextureImage::GetOuterBounds() const
{
	return _outer;
}


bounds2f TextureImage::GetInnerUV() const
{
	return _inner / glm::vec2(_textureAtlas->_image->GetWidth(), _textureAtlas->_image->GetHeight());
}


bounds2f TextureImage::GetOuterUV() const
{
	return _outer / glm::vec2(_textureAtlas->_image->GetWidth(), _textureAtlas->_image->GetHeight());
}


/***/



TextureSheet::TextureSheet(TextureAtlas* textureAtlas, int size_u, int size_v) :
	_textureAtlas(textureAtlas),
	size(size_u, size_v)
{
}


TextureImage* TextureSheet::GetTextureImage(int u0, int v0, int size_u, int size_v)
{
	bounds2f bounds = bounds2f(u0, v0, u0 + size_u, v0 + size_v);// / glm::vec2(size.x, size.y);

	return _textureAtlas->GetTextureImage(bounds, bounds);
}


TextureImage* TextureSheet::GetTexturePatch(int u0, int v0, int size_u, int size_v, int inset_u, int inset_v)
{
	bounds2f outer = bounds2f(u0, v0, u0 + size_u, v0 + size_v);// / glm::vec2(size.x, size.y);
	bounds2f inner = outer.grow(-(float)inset_u /*/ size.x*/, -(float)inset_v /*/ size.y*/);

	return _textureAtlas->GetTextureImage(inner, outer);
}
