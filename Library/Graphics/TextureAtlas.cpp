#include "TextureAtlas.h"
#include "Image.h"
#import "GraphicsContext.h"



TextureAtlas::TextureAtlas(GraphicsContext* gc) : Texture(gc),
	_image(nullptr),
	_currentX(0),
	_currentY(0),
	_nextY(0)
{
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


TextureImage* TextureAtlas::AddTextureImage(Image* image)
{
	if (_image == nullptr)
	{
		image = new Image(1024, 1204);
	}

	if (_currentX + image->GetWidth() > _image->GetWidth())
	{
		_currentX = 0;
		_currentY = _nextY;
	}

	_image->Copy(*image, _currentX, _currentY);

	TextureImage* result = new TextureImage();
	result->_textureAtlas = this;
	result->_x = _currentX;
	result->_y = _currentY;
	result->_w = image->GetWidth();
	result->_h = image->GetHeight();

	_images.push_back(result);

	_currentX += result->_w;
	_nextY = glm::max(_nextY, _currentY + result->_h);

	return result;
}


/***/


TextureImage::TextureImage()
{

}


TextureImage::~TextureImage()
{

}


bounds2f TextureImage::GetBoundsUV() const
{
	glm::vec2 s = glm::vec2(_textureAtlas->_image->GetWidth(), _textureAtlas->_image->GetHeight());
	return bounds2f(_x, _y, _x + _w, _y + _h) / s;
}
