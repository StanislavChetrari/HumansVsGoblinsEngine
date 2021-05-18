#ifndef _WINDOW_SYSTEM_H_
#define _WINDOW_SYSTEM_H_

#include <SDL2/SDL.h>

#include <EngineAPI.h>

#include <MathUtils.h>
#include <Texture.h>

struct TWindow
{
	SDL_Window*   window;
	SDL_Renderer* renderer;
	SDL_Texture*  screenTexture;
	uint8_t*      screenTextureBuffer;
	int           width;
	int           height;

	int           fullscreen;
	int           framerate;

	uint8_t       currentRenderColour[3];
};

/* Inits SDL and SDL window.
   posX and posY can have -1 to center or other non negative value for position.
   Returns 0 on success and 1 on error */
DECL_API int TWindow_Create(struct TWindow* window, const char* title, int width, int height, int posX, int posY, int fullscreen, int framerate);

DECL_API void TWindow_InitZero(void* window);
/* Uninits window and SDL. */
DECL_API void TWindow_Clear(void* window);

DECL_API void TWindow_RenderStart(struct TWindow* window);
DECL_API void TWindow_RenderStop(struct TWindow* window);

DECL_API void TWindow_SetRenderDrawColor(struct TWindow* window, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
DECL_API void TWindow_RenderDrawLine(const struct TWindow* window, int x1, int y1, int x2, int y2);
DECL_API void TWindow_RenderDrawPoint(const struct TWindow* window, int x, int y);
DECL_API void TWindow_RenderDrawPointColor(const struct TWindow* window, int x, int y, uint8_t* color);
DECL_API void TWindow_RenderFillRect(const struct TWindow* window, int x, int y, int w, int h);
DECL_API void TWindow_RenderClear(const struct TWindow* window);
DECL_API void TWindow_RenderPresent(const struct TWindow* window);

#endif /* #ifndef _WINDOW_SYSTEM_H_ */