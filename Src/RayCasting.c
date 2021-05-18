#include "RayCasting.h"

#include <Map.h>
#include <Texture.h>

void TIntersectionPointData_InitZero(void* intersectionPointData)
{
	if (intersectionPointData)
	{
		struct TIntersectionPointData* t_intersectionPointData = intersectionPointData;

		t_intersectionPointData->cellInfo = NULL;
		t_intersectionPointData->cellPosition.x = 0;
		t_intersectionPointData->cellPosition.y = 0;
		t_intersectionPointData->intersectionPoint.x = 0.0f;
		t_intersectionPointData->intersectionPoint.y = 0.0f;
	}
}

int RayMapIntersection(const struct Vec2f* rayOrigin, const struct Vec2f* rayDirection, const struct TMap* map, struct TIntersectionPointData* intersectionPointData, int nthIntersection)
{
	float check_cell_x = (float)(int)(rayOrigin->x);
	float check_cell_y = (float)(int)(rayOrigin->y);

	// 1.0f because length of rayDir is assumed to be 1
	float delta_dist_x = fabsf(1.0f / rayDirection->x);
	float delta_dist_y = fabsf(1.0f / rayDirection->y);

	float step_dist_x;
	float step_dist_y;

	float ray_quadrant_x;
	float ray_quadrant_y;

	if (rayDirection->x < 0)
	{
		ray_quadrant_x = -1.0f;
		step_dist_x = fabsf(rayOrigin->x - check_cell_x) * delta_dist_x;
	}
	else
	{
		ray_quadrant_x = 1.0f;
		step_dist_x = (1.0f - fabsf(rayOrigin->x - check_cell_x)) * delta_dist_x;
	}

	if (rayDirection->y < 0)
	{
		ray_quadrant_y = -1.0f;
		step_dist_y = fabsf(rayOrigin->y - check_cell_y) * delta_dist_y;
	}
	else
	{
		ray_quadrant_y = 1.0f;
		step_dist_y = (1.0f - fabsf(rayOrigin->y - check_cell_y)) * delta_dist_y;
	}

	// 0 for x axis, 1 for y axis
	int intersection_axis = (step_dist_x < step_dist_y) ? 0 : 1;

	int finish_search                 = 0;
	int intersected                   = 0;
	int cur_intersection_order_number = 0;
	while (!finish_search)
	{
		if (0 <= check_cell_x && check_cell_x < map->numCellsX &&
			0 <= check_cell_y && check_cell_y < map->numCellsY)
		{
			// Check current cell
			size_t cell_top    = map->data[(int)(((map->numCellsY - 1) - check_cell_y) * map->numCellsX + check_cell_x)].top;
			size_t cell_bottom = map->data[(int)(((map->numCellsY - 1) - check_cell_y) * map->numCellsX + check_cell_x)].bottom;
			size_t cell_left   = map->data[(int)(((map->numCellsY - 1) - check_cell_y) * map->numCellsX + check_cell_x)].left;
			size_t cell_right  = map->data[(int)(((map->numCellsY - 1) - check_cell_y) * map->numCellsX + check_cell_x)].right;

			int is_wall = 0;

			if (intersection_axis == 0)
			{
				if (rayDirection->x < 0)
				{
					if (cell_left)
					{
						cur_intersection_order_number++;

						if (cur_intersection_order_number == nthIntersection)
						{
							intersectionPointData->intersectionPoint.x = rayOrigin->x + (step_dist_x)*rayDirection->x;
							intersectionPointData->intersectionPoint.y = rayOrigin->y + (step_dist_x)*rayDirection->y;

							intersectionPointData->intersectionNormal.x = 1.0f;
							intersectionPointData->intersectionNormal.y = 0.0f;

							intersectionPointData->intersectionPointDistance = step_dist_x;

							intersectionPointData->cellInfo = &map->cellsInfo[cell_left];

							intersectionPointData->cellPosition.x = (int)check_cell_x;
							intersectionPointData->cellPosition.y = (int)check_cell_y;

							float t_texture_coordinate_x = intersectionPointData->intersectionPoint.x - (int)intersectionPointData->intersectionPoint.x;
							float t_texture_coordinate_y = intersectionPointData->intersectionPoint.y - (int)intersectionPointData->intersectionPoint.y;
							if (t_texture_coordinate_x > t_texture_coordinate_y)
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_x < 1.0f - 0.0001f) ? t_texture_coordinate_x : t_texture_coordinate_y;
							else
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_y < 1.0f - 0.0001f) ? t_texture_coordinate_y : t_texture_coordinate_x;

							finish_search = 1;
							intersected = 1;
							continue;
						}
					}
				}
				else
				{
					if (cell_right)
					{
						cur_intersection_order_number++;

						if (cur_intersection_order_number == nthIntersection)
						{
							intersectionPointData->intersectionPoint.x = rayOrigin->x + (step_dist_x)*rayDirection->x;
							intersectionPointData->intersectionPoint.y = rayOrigin->y + (step_dist_x)*rayDirection->y;

							intersectionPointData->intersectionNormal.x = -1.0f;
							intersectionPointData->intersectionNormal.y = 0.0f;

							intersectionPointData->intersectionPointDistance = step_dist_x;

							intersectionPointData->cellInfo = &map->cellsInfo[cell_right];

							intersectionPointData->cellPosition.x = (int)check_cell_x;
							intersectionPointData->cellPosition.y = (int)check_cell_y;

							float t_texture_coordinate_x = intersectionPointData->intersectionPoint.x - (int)intersectionPointData->intersectionPoint.x;
							float t_texture_coordinate_y = intersectionPointData->intersectionPoint.y - (int)intersectionPointData->intersectionPoint.y;
							if (t_texture_coordinate_x > t_texture_coordinate_y)
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_x < 1.0f - 0.0001f) ? t_texture_coordinate_x : t_texture_coordinate_y;
							else
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_y < 1.0f - 0.0001f) ? t_texture_coordinate_y : t_texture_coordinate_x;

							finish_search = 1;
							intersected = 1;
							continue;
						}
					}
				}
			}
			else
			{
				if (rayDirection->y < 0)
				{
					if (cell_bottom)
					{
						cur_intersection_order_number++;

						if (cur_intersection_order_number == nthIntersection)
						{
							intersectionPointData->intersectionPoint.x = rayOrigin->x + (step_dist_y)*rayDirection->x;
							intersectionPointData->intersectionPoint.y = rayOrigin->y + (step_dist_y)*rayDirection->y;

							intersectionPointData->intersectionNormal.x = 0.0f;
							intersectionPointData->intersectionNormal.y = 1.0f;

							intersectionPointData->intersectionPointDistance = step_dist_y;

							intersectionPointData->cellInfo = &map->cellsInfo[cell_bottom];

							intersectionPointData->cellPosition.x = (int)check_cell_x;
							intersectionPointData->cellPosition.y = (int)check_cell_y;

							float t_texture_coordinate_x = intersectionPointData->intersectionPoint.x - (int)intersectionPointData->intersectionPoint.x;
							float t_texture_coordinate_y = intersectionPointData->intersectionPoint.y - (int)intersectionPointData->intersectionPoint.y;
							if (t_texture_coordinate_x > t_texture_coordinate_y)
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_x < 1.0f - 0.0001f) ? t_texture_coordinate_x : t_texture_coordinate_y;
							else
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_y < 1.0f - 0.0001f) ? t_texture_coordinate_y : t_texture_coordinate_x;

							finish_search = 1;
							intersected = 1;
							continue;
						}
					}
				}
				else
				{
					if (cell_top)
					{
						cur_intersection_order_number++;

						if (cur_intersection_order_number == nthIntersection)
						{
							intersectionPointData->intersectionPoint.x = rayOrigin->x + (step_dist_y)*rayDirection->x;
							intersectionPointData->intersectionPoint.y = rayOrigin->y + (step_dist_y)*rayDirection->y;

							intersectionPointData->intersectionNormal.x = 0.0f;
							intersectionPointData->intersectionNormal.y = -1.0f;

							intersectionPointData->intersectionPointDistance = step_dist_y;

							intersectionPointData->cellInfo = &map->cellsInfo[cell_top];

							intersectionPointData->cellPosition.x = (int)check_cell_x;
							intersectionPointData->cellPosition.y = (int)check_cell_y;

							float t_texture_coordinate_x = intersectionPointData->intersectionPoint.x - (int)intersectionPointData->intersectionPoint.x;
							float t_texture_coordinate_y = intersectionPointData->intersectionPoint.y - (int)intersectionPointData->intersectionPoint.y;
							if (t_texture_coordinate_x > t_texture_coordinate_y)
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_x < 1.0f - 0.0001f) ? t_texture_coordinate_x : t_texture_coordinate_y;
							else
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_y < 1.0f - 0.0001f) ? t_texture_coordinate_y : t_texture_coordinate_x;

							finish_search = 1;
							intersected = 1;
							continue;
						}
					}
				}
			}
		}
		else
		{
			finish_search = 1;
			continue;
		}

		int t_check_cell_x = (int)check_cell_x;
		int t_check_cell_y = (int)check_cell_y;
		int t_intersection_axis = 0;

		// Make the other cell that touches this point
		if (step_dist_x < step_dist_y)
		{
			// step in direction x
			t_check_cell_x += (int)ray_quadrant_x;
			t_intersection_axis = 0;
		}
		else
		{
			// step in direction y
			t_check_cell_y += (int)ray_quadrant_y;
			t_intersection_axis = 1;
		}

		if (0 <= t_check_cell_x && t_check_cell_x < map->numCellsX &&
			0 <= t_check_cell_y && t_check_cell_y < map->numCellsY)
		{
			// Check current cell
			int cell_top    = map->data[(int)(((map->numCellsY - 1) - t_check_cell_y) * map->numCellsX + t_check_cell_x)].top;
			int cell_bottom = map->data[(int)(((map->numCellsY - 1) - t_check_cell_y) * map->numCellsX + t_check_cell_x)].bottom;
			int cell_left   = map->data[(int)(((map->numCellsY - 1) - t_check_cell_y) * map->numCellsX + t_check_cell_x)].left;
			int cell_right  = map->data[(int)(((map->numCellsY - 1) - t_check_cell_y) * map->numCellsX + t_check_cell_x)].right;

			if (t_intersection_axis == 0)
			{
				if (rayDirection->x < 0)
				{
					if (cell_right)
					{
						cur_intersection_order_number++;

						if (cur_intersection_order_number == nthIntersection)
						{
							finish_search = 1;
							intersected = 1;

							intersectionPointData->intersectionPoint.x = rayOrigin->x + (step_dist_x)*rayDirection->x;
							intersectionPointData->intersectionPoint.y = rayOrigin->y + (step_dist_x)*rayDirection->y;

							intersectionPointData->intersectionNormal.x = 1.0f;
							intersectionPointData->intersectionNormal.y = 0.0f;

							intersectionPointData->intersectionPointDistance = step_dist_x;

							intersectionPointData->cellInfo = &map->cellsInfo[cell_right];

							intersectionPointData->cellPosition.x = t_check_cell_x;
							intersectionPointData->cellPosition.y = t_check_cell_y;

							float t_texture_coordinate_x = intersectionPointData->intersectionPoint.x - (int)intersectionPointData->intersectionPoint.x;
							float t_texture_coordinate_y = intersectionPointData->intersectionPoint.y - (int)intersectionPointData->intersectionPoint.y;
							if (t_texture_coordinate_x > t_texture_coordinate_y)
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_x < 1.0f - 0.0001f) ? t_texture_coordinate_x : t_texture_coordinate_y;
							else
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_y < 1.0f - 0.0001f) ? t_texture_coordinate_y : t_texture_coordinate_x;
						}
					}
				}
				else
				{
					if (cell_left)
					{
						cur_intersection_order_number++;

						if (cur_intersection_order_number == nthIntersection)
						{
							finish_search = 1;
							intersected = 1;

							intersectionPointData->intersectionPoint.x = rayOrigin->x + (step_dist_x)*rayDirection->x;
							intersectionPointData->intersectionPoint.y = rayOrigin->y + (step_dist_x)*rayDirection->y;

							intersectionPointData->intersectionNormal.x = -1.0f;
							intersectionPointData->intersectionNormal.y = 0.0f;

							intersectionPointData->intersectionPointDistance = step_dist_x;

							intersectionPointData->cellInfo = &map->cellsInfo[cell_left];

							intersectionPointData->cellPosition.x = t_check_cell_x;
							intersectionPointData->cellPosition.y = t_check_cell_y;

							float t_texture_coordinate_x = intersectionPointData->intersectionPoint.x - (int)intersectionPointData->intersectionPoint.x;
							float t_texture_coordinate_y = intersectionPointData->intersectionPoint.y - (int)intersectionPointData->intersectionPoint.y;
							if (t_texture_coordinate_x > t_texture_coordinate_y)
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_x < 1.0f - 0.0001f) ? t_texture_coordinate_x : t_texture_coordinate_y;
							else
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_y < 1.0f - 0.0001f) ? t_texture_coordinate_y : t_texture_coordinate_x;
						}
					}
				}
			}
			else
			{
				if (rayDirection->y < 0)
				{
					if (cell_top)
					{
						cur_intersection_order_number++;

						if (cur_intersection_order_number == nthIntersection)
						{
							finish_search = 1;
							intersected = 1;

							intersectionPointData->intersectionPoint.x = rayOrigin->x + (step_dist_y)*rayDirection->x;
							intersectionPointData->intersectionPoint.y = rayOrigin->y + (step_dist_y)*rayDirection->y;

							intersectionPointData->intersectionNormal.x = 0.0f;
							intersectionPointData->intersectionNormal.y = 1.0f;

							intersectionPointData->intersectionPointDistance = step_dist_y;

							intersectionPointData->cellInfo = &map->cellsInfo[cell_top];

							intersectionPointData->cellPosition.x = t_check_cell_x;
							intersectionPointData->cellPosition.y = t_check_cell_y;

							float t_texture_coordinate_x = intersectionPointData->intersectionPoint.x - (int)intersectionPointData->intersectionPoint.x;
							float t_texture_coordinate_y = intersectionPointData->intersectionPoint.y - (int)intersectionPointData->intersectionPoint.y;
							if (t_texture_coordinate_x > t_texture_coordinate_y)
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_x < 1.0f - 0.0001f) ? t_texture_coordinate_x : t_texture_coordinate_y;
							else
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_y < 1.0f - 0.0001f) ? t_texture_coordinate_y : t_texture_coordinate_x;
						}
					}
				}
				else
				{
					if (cell_bottom)
					{
						cur_intersection_order_number++;

						if (cell_bottom && cur_intersection_order_number == nthIntersection)
						{
							finish_search = 1;
							intersected = 1;

							intersectionPointData->intersectionPoint.x = rayOrigin->x + (step_dist_y)*rayDirection->x;
							intersectionPointData->intersectionPoint.y = rayOrigin->y + (step_dist_y)*rayDirection->y;

							intersectionPointData->intersectionNormal.x = 0.0f;
							intersectionPointData->intersectionNormal.y = -1.0f;

							intersectionPointData->intersectionPointDistance = step_dist_y;

							intersectionPointData->cellInfo = &map->cellsInfo[cell_bottom];

							intersectionPointData->cellPosition.x = t_check_cell_x;
							intersectionPointData->cellPosition.y = t_check_cell_y;

							float t_texture_coordinate_x = intersectionPointData->intersectionPoint.x - (int)intersectionPointData->intersectionPoint.x;
							float t_texture_coordinate_y = intersectionPointData->intersectionPoint.y - (int)intersectionPointData->intersectionPoint.y;
							if (t_texture_coordinate_x > t_texture_coordinate_y)
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_x < 1.0f - 0.0001f) ? t_texture_coordinate_x : t_texture_coordinate_y;
							else
								intersectionPointData->intersectionTextureCoordinate = (t_texture_coordinate_y < 1.0f - 0.0001f) ? t_texture_coordinate_y : t_texture_coordinate_x;
						}
					}
				}
			}
		}
		else
		{
			finish_search = 1;
			continue;
		}

		if (step_dist_x < step_dist_y)
		{
			// step in direction x
			step_dist_x += delta_dist_x;
			check_cell_x += ray_quadrant_x;
		}
		else
		{
			// step in direction y
			step_dist_y += delta_dist_y;
			check_cell_y += ray_quadrant_y;
		}

		intersection_axis = (step_dist_x < step_dist_y) ? 0 : 1;
	}

	return intersected;
}