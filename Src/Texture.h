#ifndef _TEXTURE_HPP_
#define _TEXTURE_HPP_

#include <inttypes.h>

#include <EngineAPI.h>

enum EnumPixelType { pixelTypeUndefined, pixelTypeRGB, pixelTypeRGBA };

struct TTexture
{
	int                width, height;
	uint8_t*           data;
	enum EnumPixelType pixelType;
	size_t             bytesPerPixel;
};

struct TRGBAColor
{
	uint8_t r, g, b, a;
};

/*
	It will only load RGB or RGBA images with 1 byte per channel. 
	Source image must be in sRGB space, but after load in TTexture.data will be linear space values. 

	Returns 0 on success and non 0 on failure.
*/
DECL_API int  TTexture_LoadFromFile(struct TTexture* texture, const char* filePath, int convertToLinear);

/*
	Don't use! It is broken for now.
*/
DECL_API void TTexture_GetPixel(struct TTexture* texture, float x, float y, struct TRGBAColor* pixelColor);

DECL_API void TTexture_InitZero(void* texture);
DECL_API void TTexture_Clear(void* texture);

#endif /* #ifndef _TEXTURE_HPP_ */