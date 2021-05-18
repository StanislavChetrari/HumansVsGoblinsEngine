#include "RenderMap2D.h"

#include <RayCasting.h>
#include <Camera.h>
#include <Window.h>
#include <Map.h>

void RenderMap2D(const struct TMap* map, const struct TCamera* camera, struct TWindow* window, float scaleFactor)
{
	// Scale is computed here for original sizes to avoid recomputing each time. It saved from float precision and rounding problems.
	int cell_size      = (int)(scaleFactor * 50);  // width and height of a cell
	int normal_length  = (int)(scaleFactor * 5);   // length of normal vectors
	int forward_length = (int)(scaleFactor * 100); // length of the forward vector
	int player_size    = (int)(scaleFactor * 20);  // width and height of the player reclangle
	int side_w         = (int)(scaleFactor * 10);  // the width of the wall rectangle

	struct Vec2i offset_map = { (int)(-TO_COORD(camera->position.x * cell_size) + window->width * 0.5f - 1), (int)(-TO_COORD(camera->position.y * cell_size) + TO_COORD(window->height * 0.5f)) };

	// Draw background
	TWindow_SetRenderDrawColor(window, 0, 0, 0, 255);
	TWindow_RenderClear(window);
	// End draw background

	// Draw map
	for (int y = 0; y < map->numCellsY; ++y)
	{
		for (int x = 0; x < map->numCellsX; ++x)
		{
			size_t cell_top_index = map->data[(TO_COORD(map->numCellsY) - y) * map->numCellsX + x].top;
			size_t cell_bottom_index = map->data[(TO_COORD(map->numCellsY) - y) * map->numCellsX + x].bottom;
			size_t cell_left_index = map->data[(TO_COORD(map->numCellsY) - y) * map->numCellsX + x].left;
			size_t cell_right_index = map->data[(TO_COORD(map->numCellsY) - y) * map->numCellsX + x].right;

			if (cell_top_index)
			{
				int rect_x = (int)(offset_map.x + TO_COORD(x * cell_size));
				int rect_y = (int)(offset_map.y + TO_COORD(y * cell_size));
				rect_y += (int)(TO_COORD(cell_size) - TO_COORD(side_w));

				int rect_w = (int)(cell_size);
				int rect_h = (int)(side_w);

				if (cellColor == map->cellsInfo[cell_top_index].type)
				{
					uint8_t* color = map->cellsInfo[cell_top_index].colour;
					TWindow_SetRenderDrawColor(window, (uint8_t)(color[0]), (uint8_t)(color[1]), (uint8_t)(color[2]), 255);
					TWindow_RenderFillRect(window, rect_x, rect_y, rect_w, rect_h);
				}
				else if (cellTexture == map->cellsInfo[cell_top_index].type)
				{
					struct TTexture* texture = &map->textures[map->cellsInfo[cell_top_index].textureIndex];

					for (int pixel_y = 0; pixel_y < rect_h; ++pixel_y)
					{
						int tex_y = (int)((float)pixel_y / (float)rect_h * (float)texture->height);
						for (int pixel_x = 0; pixel_x < rect_w; ++pixel_x)
						{
							int tex_x = (int)((float)pixel_x / (float)rect_w * (float)texture->width);
							TWindow_RenderDrawPointColor(window, rect_x + pixel_x, rect_y + pixel_y, &texture->data[(size_t)tex_y * texture->width * texture->bytesPerPixel + tex_x * texture->bytesPerPixel]);
						}
					}
				}
			}
			if (cell_bottom_index)
			{
				int rect_x = (int)(offset_map.x + TO_COORD(x * cell_size));
				int rect_y = (int)(offset_map.y + TO_COORD(y * cell_size));

				int rect_w = (int)(cell_size);
				int rect_h = (int)(side_w);

				if (cellColor == map->cellsInfo[cell_bottom_index].type)
				{
					uint8_t* color = map->cellsInfo[cell_bottom_index].colour;
					TWindow_SetRenderDrawColor(window, (uint8_t)(color[0]), (uint8_t)(color[1]), (uint8_t)(color[2]), 255);

					TWindow_RenderFillRect(window, rect_x, rect_y, rect_w, rect_h);
				}
				else if (cellTexture == map->cellsInfo[cell_bottom_index].type)
				{
					struct TTexture* texture = &map->textures[map->cellsInfo[cell_bottom_index].textureIndex];

					for (int pixel_y = 0; pixel_y < rect_h; ++pixel_y)
					{
						int tex_y = (int)((float)pixel_y / (float)rect_h * (float)texture->height);
						for (int pixel_x = 0; pixel_x < rect_w; ++pixel_x)
						{
							int tex_x = (int)((float)pixel_x / (float)rect_w * (float)texture->width);
							TWindow_RenderDrawPointColor(window, rect_x + pixel_x, rect_y + pixel_y, &texture->data[(size_t)tex_y * texture->width * texture->bytesPerPixel + tex_x * texture->bytesPerPixel]);
						}
					}
				}
			}
			if (cell_left_index)
			{
				int rect_x = (int)(offset_map.x + TO_COORD(x * cell_size));
				int rect_y = (int)(offset_map.y + TO_COORD(y * cell_size));
				rect_y += (int)(TO_COORD(side_w) + 1);

				int rect_w = (int)(side_w);
				int rect_h = (int)(cell_size - side_w * 2);

				if (cellColor == map->cellsInfo[cell_left_index].type)
				{
					uint8_t* color = map->cellsInfo[cell_left_index].colour;
					TWindow_SetRenderDrawColor(window, (uint8_t)(color[0]), (uint8_t)(color[1]), (uint8_t)(color[2]), 255);

					TWindow_RenderFillRect(window, rect_x, rect_y, rect_w, rect_h);
				}
				else if (cellTexture == map->cellsInfo[cell_left_index].type)
				{
					struct TTexture* texture = &map->textures[map->cellsInfo[cell_left_index].textureIndex];

					for (int pixel_y = 0; pixel_y < rect_h; ++pixel_y)
					{
						int tex_y = (int)((float)pixel_y / (float)rect_h * (float)texture->height);
						for (int pixel_x = 0; pixel_x < rect_w; ++pixel_x)
						{
							int tex_x = (int)((float)pixel_x / (float)rect_w * (float)texture->width);
							TWindow_RenderDrawPointColor(window, rect_x + pixel_x, rect_y + pixel_y, &texture->data[(size_t)tex_y * texture->width * texture->bytesPerPixel + tex_x * texture->bytesPerPixel]);
						}
					}
				}
			}
			if (cell_right_index)
			{
				int rect_x = (int)(offset_map.x + TO_COORD(x * cell_size));
				rect_x += (int)(TO_COORD(cell_size) - TO_COORD(side_w));
				int rect_y = (int)(offset_map.y + TO_COORD(y * cell_size));
				rect_y += (int)(TO_COORD(side_w) + 1);

				int rect_w = (int)(side_w);
				int rect_h = (int)(cell_size - side_w * 2);

				if (cellColor == map->cellsInfo[cell_right_index].type)
				{
					uint8_t* color = map->cellsInfo[cell_right_index].colour;
					TWindow_SetRenderDrawColor(window, (uint8_t)(color[0]), (uint8_t)(color[1]), (uint8_t)(color[2]), 255);

					TWindow_RenderFillRect(window, rect_x, rect_y, rect_w, rect_h);
				}
				else if (cellTexture == map->cellsInfo[cell_right_index].type)
				{
					struct TTexture* texture = &map->textures[map->cellsInfo[cell_right_index].textureIndex];

					for (int pixel_y = 0; pixel_y < rect_h; ++pixel_y)
					{
						int tex_y = (int)((float)pixel_y / (float)rect_h * (float)texture->height);
						for (int pixel_x = 0; pixel_x < rect_w; ++pixel_x)
						{
							int tex_x = (int)((float)pixel_x / (float)rect_w * (float)texture->width);
							TWindow_RenderDrawPointColor(window, rect_x + pixel_x, rect_y + pixel_y, &texture->data[(size_t)tex_y * texture->width * texture->bytesPerPixel + tex_x * texture->bytesPerPixel]);
						}
					}
				}
			}
		}
	}
	// End draw map

	// Draw rays
	float normalized_coord;
	for (int x = 0; x < window->width; ++x)
	{
		normalized_coord = 2.0f * x / (float)window->width - 1.0f;

		struct Vec2f ray_direction;
		TCamera_GenerateRay(camera, normalized_coord, &ray_direction);

		struct TIntersectionPointData intersection_point;
		TIntersectionPointData_InitZero(&intersection_point);
		if (RayMapIntersection(&camera->position, &ray_direction, map, &intersection_point, 1))
		{
			TWindow_SetRenderDrawColor(window, (uint8_t)(0), (uint8_t)(0), (uint8_t)(255), 255);

			int x1 = (int)(offset_map.x + TO_COORD(camera->position.x * cell_size));
			int y1 = (int)(offset_map.y + TO_COORD(camera->position.y * cell_size));
			int x2 = (int)(offset_map.x + TO_COORD(intersection_point.intersectionPoint.x * cell_size));
			int y2 = (int)(offset_map.y + TO_COORD(intersection_point.intersectionPoint.y * cell_size));

			TWindow_RenderDrawLine(window, x1, y1, x2, y2);
		}
	}
	// End draw rays

	// Draw intersection normals
	for (int x = 0; x < window->width; ++x)
	{
		normalized_coord = 2.0f * x / (float)window->width - 1.0f;

		struct Vec2f ray_direction;
		TCamera_GenerateRay(camera, normalized_coord, &ray_direction);

		struct TIntersectionPointData intersection_point_data;
		TIntersectionPointData_InitZero(&intersection_point_data);
		if (RayMapIntersection(&camera->position, &ray_direction, map, &intersection_point_data, 1))
		{
			TWindow_SetRenderDrawColor(window, (uint8_t)(255), (uint8_t)(255), (uint8_t)(255), 255);

			int x1 = (int)(offset_map.x + TO_COORD(intersection_point_data.intersectionPoint.x * cell_size));
			int y1 = (int)(offset_map.y + TO_COORD(intersection_point_data.intersectionPoint.y * cell_size));
			int x2 = x1 + (int)(normal_length * intersection_point_data.intersectionNormal.x);
			int y2 = y1 + (int)(normal_length * intersection_point_data.intersectionNormal.y);

			TWindow_RenderDrawLine(window, x1, y1, x2, y2);
		}
	}
	// End draw intersection normals

	// Draw player
	TWindow_SetRenderDrawColor(window, (uint8_t)(255), (uint8_t)(255), (uint8_t)(255), 255);

	int rect_x = (int)(offset_map.x + TO_COORD(camera->position.x * cell_size) - TO_COORD(player_size * 0.5f));
	int rect_y = (int)(offset_map.y + TO_COORD(camera->position.y * cell_size) - TO_COORD(player_size * 0.5f));
	int rect_w = (int)(player_size);
	int rect_h = (int)(player_size);

	TWindow_RenderFillRect(window, rect_x, rect_y, rect_w, rect_h);
	// End draw player

	// Draw forward vector
	int f_x1 = (int)(offset_map.x + camera->position.x * cell_size);
	int f_y1 = (int)(offset_map.y + camera->position.y * cell_size);
	int f_x2 = f_x1 + (int)(forward_length * camera->forward.x);
	int f_y2 = f_y1 + (int)(forward_length * camera->forward.y);
	TWindow_RenderDrawLine(window, f_x1, f_y1, f_x2, f_y2);
	// End draw forward vector
}