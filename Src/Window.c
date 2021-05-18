#include "Window.h"

#include <stdio.h>
#include <string.h>

int TWindow_Create(struct TWindow* window, const char* title, int width, int height, int posX, int posY, int fullscreen, int framerate)
{
	if (window)
	{
		if (0 != SDL_Init(SDL_INIT_VIDEO))
		{
			printf("Error! Failed to init SDL! : %s\n", SDL_GetError());
			return 1;
		}

		int pos_x = (-1 == posX) ? SDL_WINDOWPOS_CENTERED : posX;
		int pos_y = (-1 == posY) ? SDL_WINDOWPOS_CENTERED : posY;
		SDL_Window* sdl_window = SDL_CreateWindow(title, pos_x, pos_y, width, height, 0);
		if (NULL == window)
		{
			printf("Error! Failed to create SDL window! : %s\n", SDL_GetError());
			SDL_Quit();
			return 1;
		}

		SDL_Renderer* renderer = SDL_CreateRenderer(sdl_window, -1, 0);
		if (NULL == renderer)
		{
			printf("Error! Failed to create SDL renderer! : %s\n", SDL_GetError());
			SDL_DestroyWindow(sdl_window);
			SDL_Quit();
			return 1;
		}

		SDL_Texture* screen_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, width, height);
		if (!screen_texture)
		{
			printf("Error! Failed to create SDL screen texture! : %s\n", SDL_GetError());
			SDL_DestroyRenderer(renderer);
			SDL_DestroyWindow(sdl_window);
			SDL_Quit();
			return 1;
		}

		window->width               = width;
		window->height              = height;
		window->window              = sdl_window;
		window->renderer            = renderer;
		window->fullscreen          = fullscreen;
		window->framerate           = framerate;
		window->screenTexture       = screen_texture;
		window->screenTextureBuffer = NULL;
		window->currentRenderColour[0] = 0;
		window->currentRenderColour[1] = 0;
		window->currentRenderColour[2] = 0;

		printf("Init SDL with success.\n");
		printf("Created window : width = %d, height = %d, fullscreen = %s, framerate = %d\n", 
			window->width, 
			window->height, 
			((window->fullscreen) ? "true" : "false"), 
			window->framerate);

		return 0;
	}
	else
	{
		return 1;
	}
}

void TWindow_InitZero(void* window)
{
	if (window)
	{
		struct TWindow* t_window = window;

		t_window->window              = NULL;
		t_window->renderer            = NULL;
		t_window->screenTexture       = NULL;
		t_window->screenTextureBuffer = NULL;
		t_window->width               = 0;
		t_window->height              = 0;

		t_window->fullscreen = 0;
		t_window->framerate  = 60;

		t_window->currentRenderColour[0] = 0;
		t_window->currentRenderColour[1] = 0;
		t_window->currentRenderColour[2] = 0;
	}
}

void TWindow_Clear(void* window)
{
	if (window)
	{
		struct TWindow* t_window = window;

		if (t_window->renderer)
		{
			SDL_DestroyRenderer(t_window->renderer);
			t_window->renderer = NULL;
		}

		if (t_window->window)
		{
			SDL_DestroyWindow(t_window->window);
			t_window->window = NULL;
		}

		if (t_window->screenTexture)
		{
			SDL_DestroyTexture(t_window->screenTexture);
			t_window->screenTexture = NULL;
		}

		t_window->screenTextureBuffer = NULL;

		t_window->width  = 0;
		t_window->height = 0;

		SDL_Quit();
	}
}

void TWindow_RenderStart(struct TWindow* window)
{
	int pitch = 0;
	SDL_LockTexture(window->screenTexture, NULL, &window->screenTextureBuffer, &pitch);
}

void TWindow_RenderStop(struct TWindow* window)
{
	SDL_UnlockTexture(window->screenTexture);
}

void TWindow_SetRenderDrawColor(struct TWindow* window, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	window->currentRenderColour[0] = ColorGammaEncode(r, DEFAULT_GAMMA);
	window->currentRenderColour[1] = ColorGammaEncode(g, DEFAULT_GAMMA);
	window->currentRenderColour[2] = ColorGammaEncode(b, DEFAULT_GAMMA);
}

void TWindow_RenderDrawLine(const struct TWindow* window, int x1, int y1, int x2, int y2)
{
	int steep = 0;
	if (abs(x1 - x2) < abs(y1 - y2))
	{
		int t = x1;
		x1 = y1;
		y1 = t;

		t = x2;
		x2 = y2;
		y2 = t;

		steep = 1;
	}
	if (x1 > x2)
	{
		int t = x1;
		x1 = x2;
		x2 = t;

		t = y1;
		y1 = y2;
		y2 = t;
	}
	int dx = x2 - x1;
	int dy = y2 - y1;
	float derror = fabsf(dy / (float)dx);
	float error = 0;
	int y = y1;
	for (int x = x1; x <= x2; x++)
	{
		if (steep)
		{
			if (x >= 0 && x < window->height && y >= 0 && y < window->width)
			{
				window->screenTextureBuffer[(size_t)(window->height - 1 - x) * window->width * 4 + (size_t)y * 4]     = window->currentRenderColour[0];
				window->screenTextureBuffer[(size_t)(window->height - 1 - x) * window->width * 4 + (size_t)y * 4 + 1] = window->currentRenderColour[1];
				window->screenTextureBuffer[(size_t)(window->height - 1 - x) * window->width * 4 + (size_t)y * 4 + 2] = window->currentRenderColour[2];
			}
		}
		else
		{
			if (x >= 0 && x < window->width && y >= 0 && y < window->height)
			{
				window->screenTextureBuffer[(size_t)(window->height - 1 - y) * window->width * 4 + (size_t)x * 4]     = window->currentRenderColour[0];
				window->screenTextureBuffer[(size_t)(window->height - 1 - y) * window->width * 4 + (size_t)x * 4 + 1] = window->currentRenderColour[1];
				window->screenTextureBuffer[(size_t)(window->height - 1 - y) * window->width * 4 + (size_t)x * 4 + 2] = window->currentRenderColour[2];
			}
		}
		error += derror;
		if (error > 0.5f)
		{
			y += (y2 > y1 ? 1 : -1);
			error -= 1.;
		}
	}
}

void TWindow_RenderDrawPoint(const struct TWindow* window, int x, int y)
{
	if (x >= 0 && x < window->width && y >= 0 && y < window->height)
	{
		window->screenTextureBuffer[(size_t)(window->height - 1 - y) * window->width * 4 + (size_t)x * 4]     = window->currentRenderColour[0];
		window->screenTextureBuffer[(size_t)(window->height - 1 - y) * window->width * 4 + (size_t)x * 4 + 1] = window->currentRenderColour[1];
		window->screenTextureBuffer[(size_t)(window->height - 1 - y) * window->width * 4 + (size_t)x * 4 + 2] = window->currentRenderColour[2];
	}
}

void TWindow_RenderDrawPointColor(const struct TWindow* window, int x, int y, uint8_t* color)
{
	if (x >= 0 && x < window->width && y >= 0 && y < window->height)
	{
		//memcpy(window->screenTextureBuffer + ((size_t)(window->height - 1 - y) * window->width + x) * (size_t)4, color, 3);

		window->screenTextureBuffer[((size_t)(window->height - 1 - y) * window->width + x) * (size_t)4 + 0] = ColorGammaEncode(color[0], DEFAULT_GAMMA);
		window->screenTextureBuffer[((size_t)(window->height - 1 - y) * window->width + x) * (size_t)4 + 1] = ColorGammaEncode(color[1], DEFAULT_GAMMA);
		window->screenTextureBuffer[((size_t)(window->height - 1 - y) * window->width + x) * (size_t)4 + 2] = ColorGammaEncode(color[2], DEFAULT_GAMMA);
	}
}

void TWindow_RenderFillRect(const struct TWindow* window, int x, int y, int w, int h)
{
	int x1 = (x < 0) ? 0 : x;
	int y1 = (y < 0) ? 0 : y;
	int x2 = (x + w >= window->width) ? window->width : x + w;
	int y2 = (y + h >= window->height) ? window->height : y + h;

	for (int t_y = y1; t_y < y2; ++t_y)
	{
		for (int t_x = x1; t_x < x2; ++t_x)
		{
			window->screenTextureBuffer[(size_t)(window->height - 1 - t_y) * window->width * 4 + (size_t)t_x * 4]     = window->currentRenderColour[0];
			window->screenTextureBuffer[(size_t)(window->height - 1 - t_y) * window->width * 4 + (size_t)t_x * 4 + 1] = window->currentRenderColour[1];
			window->screenTextureBuffer[(size_t)(window->height - 1 - t_y) * window->width * 4 + (size_t)t_x * 4 + 2] = window->currentRenderColour[2];
		}
	}
}

void TWindow_RenderClear(const struct TWindow* window)
{
	for (int y = 0; y < window->height; ++y)
		for (int x = 0; x < window->width; ++x)
		{
			window->screenTextureBuffer[(size_t)y * window->width * 4 + (size_t)x * 4]     = window->currentRenderColour[0];
			window->screenTextureBuffer[(size_t)y * window->width * 4 + (size_t)x * 4 + 1] = window->currentRenderColour[1];
			window->screenTextureBuffer[(size_t)y * window->width * 4 + (size_t)x * 4 + 2] = window->currentRenderColour[2];
		}
}

void TWindow_RenderPresent(const struct TWindow* window)
{
	SDL_RenderCopy(window->renderer, window->screenTexture, NULL, NULL);
	SDL_RenderPresent(window->renderer);
}