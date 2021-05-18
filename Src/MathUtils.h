#ifndef _MATH_UTILS_H_
#define _MATH_UTILS_H_

#include <math.h>
#include <stdint.h>

#include <EngineAPI.h>

#define MATH_PI 3.1415926535897932384626433832795f

#define TO_COORD(c) (c - 1)

DECL_API float ToRadians(float degrees);

struct Vec2f
{
	float x, y;
};

DECL_API float Vec2f_Distance(const struct Vec2f* vec1, const struct Vec2f* vec2);
DECL_API float Vec2f_Length(const struct Vec2f* vec);
DECL_API void  Vec2f_Normalize(struct Vec2f* vec);

struct Vec2i
{
	int x, y;
};

#define DEFAULT_GAMMA 2.2f
/*
	Converts color from linear space to gamma space.
	Returns color in gamma space.
*/
DECL_API uint8_t ColorGammaEncode(uint8_t val, float gamma);
/*
	Converts color from gamma space to linear space 
*/
DECL_API uint8_t ColorGammaDecode(uint8_t val, float gamma);

#endif /* #ifndef _MATH_UTILS_H_ */