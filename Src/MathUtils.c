#include "MathUtils.h"

float ToRadians(float degrees)
{
	return degrees * MATH_PI / 180.0f;
}

float Vec2f_Distance(const struct Vec2f* vec1, const struct Vec2f* vec2)
{
	return sqrtf(powf(vec1->x - vec2->x, 2.0f) + powf(vec1->y - vec2->y, 2.0f));
}

float Vec2f_Length(const struct Vec2f* vec)
{
	return sqrtf(vec->x * vec->x + vec->y * vec->y);
}

void  Vec2f_Normalize(struct Vec2f* vec)
{
	float length = sqrtf(vec->x * vec->x + vec->y * vec->y);
	vec->x /= length;
	vec->y /= length;
}

uint8_t ColorGammaEncode(uint8_t val, float gamma)
{
	//float c = (float)val / 255.0f;
	//c = powf(c, 1.0f / gamma);
	//return (uint8_t)(c * 255);
	return val;
}

uint8_t ColorGammaDecode(uint8_t val, float gamma)
{
	//float c = (float)val / 255.0f;
	//c = powf(c, gamma);
	//return (uint8_t)(c * 255);
	return val;
}