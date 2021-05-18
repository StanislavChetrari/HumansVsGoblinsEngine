#ifndef _RAY_CASTING_H_
#define _RAY_CASTING_H_

#include <EngineAPI.h>

#include <MathUtils.h>

struct TMap;

struct TIntersectionPointData
{
	struct TCellInfo* cellInfo;
	struct Vec2i      cellPosition;
	struct Vec2f      intersectionPoint;
	struct Vec2f      intersectionNormal;
	float             intersectionTextureCoordinate;
	float             intersectionPointDistance;
};

DECL_API void TIntersectionPointData_InitZero(void* intersectionPointData);

DECL_API int RayMapIntersection(const struct Vec2f* rayOrigin, const struct Vec2f* rayDirection, const struct TMap* map, struct TIntersectionPointData* intersectionPointData, int nthIntersection);

#endif /* #ifndef _RAY_CASTING_H_ */