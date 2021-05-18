#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include <EngineAPI.h>

struct TCamera;
struct TMap;
struct Vec2f;

DECL_API void Physics_MoveCamera(struct TCamera* camera, const struct TMap* map, const struct Vec2f* movementVector);

#endif /* #ifndef _PHYSICS_H_ */