#ifndef _CAMERA_HPP_
#define _CAMERA_HPP_

#include <EngineAPI.h>

#include <MathUtils.h>

enum EnumTCameraError
{
	tcNoError = 0,

	tcErrorCameraArgIsNULL  = 1,
	tcErrorPosArgIsNULL     = 2,
	tcErrorForwardArgIsNULL = 3
};

struct TCamera
{
	// Horizontal field of view in radians
	float horizontalFOV;
	// Aspect ratio width over height
	float aspectRatio;
	// It is safe to set position directly in the struct.
	struct Vec2f position;
	struct Vec2f forward;
	struct Vec2f right;
	// tan(0.5 * horizontalFOV)
	float halfWidth;
	//
	float halfHeight;
};

/*
	Arguments:
		horizontalFOV   : Horizontal field of view in radians.
		aspectRatio     : Aspect ratio width over height.
		position        : Position of the camera.
		forwardDirection: Direction vector for forward.

	Return: 
		tcNoError
		tcErrorCameraArgIsNULL
		tcErrorPositionArgIsNULL
		tcErrorForwardDirectionArgIsNULL
*/
DECL_API enum EnumTCameraError TCamera_Create(struct TCamera* camera, float horizontalFOV, float aspectRatio, const struct Vec2f* position, const struct Vec2f* forwardDirection);

/*
	Set forward vector. It also computes the right vector.
	Doesn't handle errors!
*/
DECL_API void TCamera_SetForward(struct TCamera* camera, const struct Vec2f* forwardDirection);

/*
	Generates a 2D ray from camera using normalized coordinate. Make shure to send coordinates in interval 0..1 they will not be checked.
	Doesn't handle errors!
*/
DECL_API void TCamera_GenerateRay(const struct TCamera* camera, float normalizedCoordinate, struct Vec2f* ray);

/*
	Set gorizontal field of view. It also computes halfWidth and halfHeight.
	Doesn't handle errors!
*/
DECL_API void TCamera_SetFOV(struct TCamera* camera, float horizontalFOV);

/*
	Set aspect ratio width over height. Is also computes halfHeight.
	Doesn't handle errors!
*/
DECL_API void TCamera_SetAspectRatio(struct TCamera* camera, float aspectRatio);

/*
	Return:
		tcNoError
		tcErrorCameraArgIsNULL
*/
DECL_API enum EnumTCameraError TCamera_InitZero(void* camera);

/*
	Return:
		tcNoError
		tcErrorCameraArgIsNULL
*/
DECL_API enum EnumTCameraError TCamera_Clear(void* camera);

/*
	Appends to errorStr the string text that coresponds to error.
	Make shure there is enough space left!
	If errorStr is NULL the function will not do anything.
*/
DECL_API void TCamera_GetErrorStr(enum EnumTCameraError error, char* errorStr);

/*
	Returns size in bytes that is needed to store error string with TCamera_GetErrorStr(). It includes NULL terminator.
*/
DECL_API size_t TCamera_GetErrorStrSize(enum EnumTCameraError error);

#endif /* #ifndef _CAMERA_HPP_ */