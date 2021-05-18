#include "Texture.h"

#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include <libpng/png.h>

#include <MathUtils.h>

int TTexture_LoadFromFile(struct TTexture* texture, const char* filePath, int convertToLinear)
{
	int ret_val = 1;

	char*  buffer      = NULL;
	size_t buffer_size = 0;

	FILE* file_in = fopen(filePath, "rb");
	if (file_in)
	{
		fseek(file_in, 0, SEEK_END);
		size_t file_size = ftell(file_in);
		fseek(file_in, 0, SEEK_SET);

		buffer = (char*)malloc(file_size + 1);
		if (buffer)
		{
			if (fread(buffer, sizeof(char), file_size, file_in) == file_size)
			{
				buffer[file_size] = '\0';
				buffer_size = file_size + 1;
			}
			else
			{
				free(buffer);
				buffer = NULL;
			}
		}

		fclose(file_in);
	}

	if (buffer)
	{
		png_image png_img;
		memset(&png_img, 0, sizeof(png_img));
		png_img.version = PNG_IMAGE_VERSION;

		if (png_image_begin_read_from_memory(&png_img, buffer, buffer_size))
		{
			int good_to_load = 0;

			enum EnumPixelType pixel_type = pixelTypeUndefined;
			size_t        bytes_per_pixel = 0;

			if (png_img.format == PNG_FORMAT_RGB)
			{
				good_to_load    = 1;
				pixel_type      = pixelTypeRGB;
				bytes_per_pixel = 3;
			}
			else if (png_img.format == PNG_FORMAT_RGBA)
			{
				good_to_load    = 1;
				pixel_type      = pixelTypeRGBA;
				bytes_per_pixel = 4;
			}

			if (good_to_load)
			{
				texture->width = png_img.width;
				texture->height = png_img.height;

				texture->pixelType = pixel_type;
				texture->bytesPerPixel = bytes_per_pixel;

				texture->data = malloc(sizeof(uint8_t) * texture->width * texture->height * texture->bytesPerPixel);
				if (png_image_finish_read(&png_img, NULL, texture->data, 0, NULL))
				{
					if (convertToLinear)
					{
						if (pixelTypeRGB == pixel_type)
						{
							for (size_t pixel_i = 0; pixel_i < (size_t)texture->width * texture->height * texture->bytesPerPixel; ++pixel_i)
							{
								texture->data[pixel_i] = ColorGammaDecode(texture->data[pixel_i], DEFAULT_GAMMA);
							}
						}
						else if (pixelTypeRGBA == pixel_type)
						{
							for (size_t pixel_i = 0; pixel_i < (size_t)texture->width * texture->height * texture->bytesPerPixel; pixel_i += 4)
							{
								texture->data[pixel_i]     = ColorGammaDecode(texture->data[pixel_i], DEFAULT_GAMMA);
								texture->data[pixel_i + 1] = ColorGammaDecode(texture->data[pixel_i + 1], DEFAULT_GAMMA);
								texture->data[pixel_i + 2] = ColorGammaDecode(texture->data[pixel_i + 2], DEFAULT_GAMMA);
							}
						}
					}

					ret_val = 0;
				}
			}

			png_image_free(&png_img);
		}
	}

	return ret_val;
}

void TTexture_GetPixel(struct TTexture* texture, float x, float y, struct TRGBAColor* pixelColor)
{
	/*int tex_x = x * texture->width;
	int tex_y = y * texture->height;
	if (pixelTypeRGB == texture->pixelType)
	{
		pixelColor->r = texture->data[tex_y * (texture->width * 3) + tex_x * 3];
		pixelColor->g = texture->data[tex_y * (texture->width * 3) + tex_x * 3 + 1];
		pixelColor->b = texture->data[tex_y * (texture->width * 3) + tex_x * 3 + 2];
		pixelColor->a = 255;
	}
	else if (pixelTypeRGBA == texture->pixelType)
	{
		pixelColor->r = texture->data[tex_y * (texture->width * 3) + tex_x * 3];
		pixelColor->g = texture->data[tex_y * (texture->width * 3) + tex_x * 3 + 1];
		pixelColor->b = texture->data[tex_y * (texture->width * 3) + tex_x * 3 + 2];
		pixelColor->a = texture->data[tex_y * (texture->width * 3) + tex_x * 3 + 3];
	}*/

	int tex_x = (int)(x * texture->width);
	int tex_y = (int)(y * texture->height);

	size_t t = (size_t)tex_y * ((size_t)texture->width * 3) + (size_t)tex_x * 3;

	//pixelColor->r = texture->data[t];
	//pixelColor->g = texture->data[t + 1];
	//pixelColor->b = texture->data[t + 2];
	memcpy(&pixelColor->r, texture->data + t, 3);
}

void TTexture_InitZero(void* texture)
{
	if (texture)
	{
		struct TTexture* t_texture = texture;

		t_texture->width  = 0;
		t_texture->height = 0;
		t_texture->data   = NULL;
	}
}

void TTexture_Clear(void* texture)
{
	if (texture)
	{
		struct TTexture* t_texture = texture;

		t_texture->width  = 0;
		t_texture->height = 0;

		if (t_texture->data)
			free(t_texture->data);
	}
}