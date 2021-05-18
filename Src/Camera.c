#include "Camera.h"

#include <string.h>
#include <math.h>

enum EnumTCameraError TCamera_InitZero(void* camera)
{
	if (!camera)
		return tcErrorCameraArgIsNULL;

	struct TCamera* t_camera = camera;

	t_camera->horizontalFOV = 0.0f;
	t_camera->aspectRatio   = 0.0f;

	t_camera->position.x = 0.0f;
	t_camera->position.y = 0.0f;

	t_camera->forward.x = 0.0f;
	t_camera->forward.y = 1.0f;

	t_camera->right.x = 1.0f;
	t_camera->right.y = 0.0f;

	t_camera->halfWidth  = 0.0f;
	t_camera->halfHeight = 0.0f;

	return tcNoError;
}

enum EnumTCameraError TCamera_Clear(void* camera)
{
	return TCamera_InitZero(camera);
}

enum EnumTCameraError TCamera_Create(struct TCamera* camera, float horizontalFOV, float aspectRatio, const struct Vec2f* position, const struct Vec2f* forwardDirection)
{
	if (!camera)
		return tcErrorCameraArgIsNULL;
	if (!position)
		return tcErrorPosArgIsNULL;
	if (!forwardDirection)
		return tcErrorForwardArgIsNULL;

	TCamera_SetFOV(camera, horizontalFOV);
	TCamera_SetForward(camera, forwardDirection);
	TCamera_SetAspectRatio(camera, aspectRatio);

	camera->position = *position;

	return tcNoError;
}

void TCamera_SetForward(struct TCamera* camera, const struct Vec2f* forwardDirection)
{
	camera->forward = *forwardDirection;

	// Find Right vector by computing cross product with (0, 0, 1)
	camera->right.x = forwardDirection->y;
	camera->right.y = -forwardDirection->x;
}

void TCamera_GenerateRay(const struct TCamera* camera, float normalizedCoordinate, struct Vec2f* ray)
{
	ray->x = camera->forward.x + camera->halfWidth * normalizedCoordinate * camera->right.x;
	ray->y = camera->forward.y + camera->halfWidth * normalizedCoordinate * camera->right.y;

	Vec2f_Normalize(ray);
}

void TCamera_SetFOV(struct TCamera* camera, float horizontalFOV)
{
	camera->horizontalFOV = horizontalFOV;

	// Compute halfWidth
	camera->halfWidth = tanf(0.5f * horizontalFOV);
	// Compute halfHeight
	camera->halfHeight = camera->halfWidth / camera->aspectRatio;
}

DECL_API void TCamera_SetAspectRatio(struct TCamera* camera, float aspectRatio)
{
	camera->aspectRatio = aspectRatio;

	// Compute halfHeight
	camera->halfHeight = camera->halfWidth / aspectRatio;
}

static const char* tcErrors[] =
{
	/* tcNoError */                        "No error.",
	/* tcErrorCameraArgIsNULL */           "Argument 'camera' is NULL.",
	/* tcErrorPositionArgIsNULL */         "Argument 'position' is NULL",
	/* tcErrorForwardDirectionArgIsNULL */ "Argument 'forwardDirection' is NULL"
};

static const char* errorNotFound = "No string for this error.";

void TCamera_GetErrorStr(enum EnumTCameraError error, char* errorStr)
{
	if (NULL == errorStr)
		return;

	size_t errors_count = sizeof(tcErrors) / sizeof(const char*);
	if ((size_t)error < errors_count)
		strcpy(errorStr, tcErrors[(size_t)error]);
	else
		strcpy(errorStr, errorNotFound);
}

size_t TCamera_GetErrorStrSize(enum EnumTCameraError error)
{
	size_t errors_count = sizeof(tcErrors) / sizeof(const char*);

	if (error < errors_count)
		return (strlen(tcErrors[error]) + 1);
	else
		return (strlen(errorNotFound) + 1);
}