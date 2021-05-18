#ifndef _RENDER_MAP_2_5_D_H_
#define _RENDER_MAP_2_5_D_H_

#include <EngineAPI.h>

struct TMap;
struct TCamera;
struct TWindow;

DECL_API void RenderMap2_5D(const struct TMap* map, const struct TCamera* camera, struct TWindow* window);

#endif /* #ifndef _RENDER_MAP_2_5_D_H_ */