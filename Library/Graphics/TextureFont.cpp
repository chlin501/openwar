#include "TextureFont.h"
#include "Image.h"
#import "TextureAtlas.h"


TextureFont::TextureFont(TextureAtlas* textureAtlas, NSFont* font) :
	_textureAtlas(textureAtlas),
	_font(nil),
	_attributes(nil)

{
	_font = [font retain];
	_attributes = [@{
		NSFontAttributeName: _font,
		NSForegroundColorAttributeName: [NSColor whiteColor]
	} retain];
}


TextureFont::~TextureFont()
{
	[_attributes release];
	[_font release];
}


TextureChar* TextureFont::GetTextureChar(const std::string& character)
{
	NSString* string = [NSString stringWithUTF8String:character.c_str()];

	CGSize size = [string sizeWithAttributes:_attributes];

	int width = (int)size.width + 2;
	int height = (int)size.height + 2;


	Image* image = new Image(width, height);
	image->Fill(glm::vec4(0.2f, 0.2f, 0.2f, 1), bounds2f(0, 0, width, height));
	image->Fill(glm::vec4(0.3f, 0.3f, 0.3f, 1), bounds2f(1, 1, width - 1, height - 1));

	CGContextRef context = image->GetCGContext();

	NSGraphicsContext* gc = [NSGraphicsContext graphicsContextWithGraphicsPort:context flipped:NO];
	[NSGraphicsContext saveGraphicsState];
	[NSGraphicsContext setCurrentContext:gc];

	CGContextSetRGBFillColor(context, 1, 1, 1, 1);
	CGContextSetRGBStrokeColor(context, 1, 1, 1, 1);

	CGPoint p = CGPointMake(1, 1);
	[string drawAtPoint:p withAttributes:_attributes];

	[NSGraphicsContext restoreGraphicsState];

	TextureImage* textureImage = _textureAtlas->AddTextureImage(image);
	textureImage->_inner.min = textureImage->_outer.min + glm::vec2(1, 1);
	textureImage->_inner.max = textureImage->_inner.min + glm::vec2(size.width, size.height);

	delete image;

	return new TextureChar(textureImage);
}


/***/


TextureChar::TextureChar(TextureImage* textureImage) :
	_textureImage(textureImage)
{

}


glm::vec2 TextureChar::GetInnerSize() const
{
	return _textureImage->GetInnerBounds().size();
}


bounds2f TextureChar::GetOuterUV() const
{
	return _textureImage->GetOuterUV();
}


bounds2f TextureChar::GetOuterXY(glm::vec2 position) const
{
	bounds2f inner_xy = _textureImage->GetInnerBounds();
	bounds2f outer_xy = _textureImage->GetOuterBounds();
	return outer_xy + position - inner_xy.min;
}


TextureImage* TextureChar::GetTextureImage() const
{
	return _textureImage;
}
