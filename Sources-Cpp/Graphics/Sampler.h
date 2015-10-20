#ifndef Sampler_H
#define Sampler_H

#include "GraphicsContext.h"


enum class SamplerMinMagFilter : GLint
{
	Nearest = GL_NEAREST,
	Linear = GL_LINEAR
};


enum class SamplerAddressMode : GLint
{
	Clamp = 0x812F, // GL_CLAMP or GL_CLAMP_TO_EDGE
	Repeat = GL_REPEAT
};


struct Sampler
{
	SamplerMinMagFilter minFilter{SamplerMinMagFilter::Linear};
	SamplerMinMagFilter magFilter{SamplerMinMagFilter::Linear};
	SamplerAddressMode sAddressMode{SamplerAddressMode::Repeat};
	SamplerAddressMode tAddressMode{SamplerAddressMode::Repeat};

	Sampler()
	{}

	Sampler(SamplerMinMagFilter filter, SamplerAddressMode addressMode) :
		minFilter{filter},
		magFilter{filter},
		sAddressMode{addressMode},
		tAddressMode{addressMode}
	{}
};


#endif
