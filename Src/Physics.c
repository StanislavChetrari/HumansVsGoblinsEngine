#include "Physics.h"

#include <Camera.h>
#include <Map.h>
#include <MathUtils.h>
#include <RayCasting.h>

void Physics_MoveCamera(struct TCamera* camera, const struct TMap* map, const struct Vec2f* movementVector)
{
	struct TIntersectionPointData intersection_point_data;
	TIntersectionPointData_InitZero(&intersection_point_data);

	struct Vec2f t_current_movement_vector = *movementVector;

	int movement_intersects = 0;
	do
	{
		movement_intersects = 0;

		struct Vec2f move_direction = t_current_movement_vector;
		Vec2f_Normalize(&move_direction);

		if (RayMapIntersection(&camera->position, &move_direction, map, &intersection_point_data, 1))
		{
			float dist_next_position      = Vec2f_Length(&t_current_movement_vector);
			float dist_intersection_point = Vec2f_Distance(&camera->position, &intersection_point_data.intersectionPoint);

			if (dist_intersection_point < dist_next_position)
			{
				movement_intersects = 1;

				t_current_movement_vector.x += fabsf(t_current_movement_vector.x) * intersection_point_data.intersectionNormal.x;
				t_current_movement_vector.y += fabsf(t_current_movement_vector.y) * intersection_point_data.intersectionNormal.y;
			}
		}

	} while (movement_intersects);

	camera->position.x += t_current_movement_vector.x;
	camera->position.y += t_current_movement_vector.y;
}