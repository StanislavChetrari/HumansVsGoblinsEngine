#include "RenderMap2_5D.h"

#include <stdio.h>

#include <RayCasting.h>
#include <Map.h>
#include <Camera.h>
#include <Window.h>
#include <MathUtils.h>

#define DEFAULT_MIRROR_REFLECTION_COUNT 5
#define MAX_DISTANCE 999999

struct TRenderLineAtXData
{
	const struct TIntersectionPointData* intersectionPointData;
	const struct TMap* map;
	const struct TCamera* camera;
	struct TWindow* window;
	const struct Vec2f* rayOrigin;
	const struct Vec2f* rayDirection;
	const struct Vec2f* forwardVector;
	float halfHeight;
	float distance;

	int x;
	int yMin, yMax;
};

static void ComputeLight(const struct TMap* map, uint8_t* colorIn, uint8_t* colorOut, float fragmentDistance)
{
	// Ambient light
	float ambient_r = 0.0f;
	float ambient_g = 0.0f;
	float ambient_b = 0.0f;

	if (map->hasAmbientLight)
	{
		float ambient_light_color[3] = { map->ambientLightColor[0] / 255.0f, map->ambientLightColor[1] / 255.0f, map->ambientLightColor[2] / 255.0f };
	
		ambient_r = ambient_light_color[0] * map->ambientLightStrength;
		ambient_g = ambient_light_color[1] * map->ambientLightStrength;
		ambient_b = ambient_light_color[2] * map->ambientLightStrength;
	}
	// End ambient light

	float point_light_strength = 2.0f;

	// Point diffuse light
	float point_diffuse_r = 0.0f;
	float point_diffuse_g = 0.0f;
	float point_diffuse_b = 0.0f;

	if (map->playerHasLight)
	{
		float point_light_diffuse_color[3] = { map->playerLightDiffuseColor[0] / 255.0f, map->playerLightDiffuseColor[1] / 255.0f, map->playerLightDiffuseColor[2] / 255.0f };
		float point_light_diffuse_strength = map->playerLightDiffuseStrength * powf(1.0f / fragmentDistance, map->playerLightDiffusePower);

		point_diffuse_r = point_light_diffuse_color[0] * point_light_diffuse_strength;
		point_diffuse_g = point_light_diffuse_color[1] * point_light_diffuse_strength;
		point_diffuse_b = point_light_diffuse_color[2] * point_light_diffuse_strength;
	}
	// End point diffuse light

	colorOut[0] = (uint8_t)(min(ambient_r + point_diffuse_r, 1.0f) * (colorIn[0] / 255.0f) * 255.0f);
	colorOut[1] = (uint8_t)(min(ambient_g + point_diffuse_g, 1.0f) * (colorIn[1] / 255.0f) * 255.0f);
	colorOut[2] = (uint8_t)(min(ambient_b + point_diffuse_b, 1.0f) * (colorIn[2] / 255.0f) * 255.0f);
}

static void RenderEnvironment(struct TWindow* window, const struct TMap* map, int x, int y)
{
	if (map->environmentIsTexture)
	{

	}
	else
	{
		uint8_t bg_color[3] = { map->environmentColor[0], map->environmentColor[1], map->environmentColor[2] };
		ComputeLight(map, bg_color, bg_color, MAX_DISTANCE);
		TWindow_RenderDrawPointColor(window, x, y, bg_color);
	}
}

static void RenderLineAtX(struct TRenderLineAtXData* data, int renderAfterTransparentWall, int mirrorReflectionCountLeft)
{
	// Dot product. Projection player_intersect_vec on camera forward vector.
	float distance_perpendicular_to_camera_plane = data->forwardVector->x * data->rayDirection->x * data->intersectionPointData->intersectionPointDistance +
		                                           data->forwardVector->y * data->rayDirection->y * data->intersectionPointData->intersectionPointDistance;
	
	float half_height     = data->halfHeight;
	float max_wall_height = (float)data->window->height;
	float wall_height     = max_wall_height / (distance_perpendicular_to_camera_plane * 2.0f * half_height);

	int y_top = (int)(data->window->height * 0.5f + wall_height * 0.5f);
	if (y_top > data->yMax)
		y_top = data->yMax;
	int y_bottom = (int)(data->window->height * 0.5f - wall_height * 0.5f);
	if (y_bottom < data->yMin)
		y_bottom = data->yMin;

	if (cellColor == data->intersectionPointData->cellInfo->type)
	{
		uint8_t t_color[3];
		ComputeLight(data->map, data->intersectionPointData->cellInfo->colour, t_color, data->intersectionPointData->intersectionPointDistance);
		TWindow_SetRenderDrawColor(data->window, t_color[0], t_color[1], t_color[2], 255);
		TWindow_RenderDrawLine(data->window, data->x, TO_COORD(data->window->height) - y_top, data->x, TO_COORD(data->window->height) - y_bottom);
	}
	else if (cellTexture == data->intersectionPointData->cellInfo->type)
	{
		float half_compensation = (wall_height - (y_top - y_bottom + 1)) * 0.5f;
		half_compensation = (half_compensation < 0.0f) ? 0.0f : half_compensation;

		struct TTexture* texture = &data->map->textures[data->intersectionPointData->cellInfo->textureIndex];
		int texture_width           = texture->width;
		int texture_height          = texture->height;
		int texture_bytes_per_pixel = (int)texture->bytesPerPixel;

		if (pixelTypeRGB == texture->pixelType)
		{
			int tex_x = (int)(data->intersectionPointData->intersectionTextureCoordinate * texture_width);

			for (int y = y_bottom; y <= y_top; ++y)
			{
				// Compute Y texture coordinate
				int tex_y = (int)((half_compensation + y - y_bottom) / wall_height * texture_height);

				int pixel_index = (tex_y * texture_width + tex_x) * texture_bytes_per_pixel;
				
				uint8_t t_color[3];
				ComputeLight(data->map, &texture->data[pixel_index], t_color, data->intersectionPointData->intersectionPointDistance);
				TWindow_RenderDrawPointColor(data->window, data->x, y, t_color);
			}
		}
		else if (pixelTypeRGBA == texture->pixelType)
		{
			if (renderAfterTransparentWall)
			{
				#define MAX_INTERSECTIONS_TILL_OPAQUE 4
				struct TIntersectionPointData intersections[MAX_INTERSECTIONS_TILL_OPAQUE];
				int                           intersections_count = 0;

				while (intersections_count <= MAX_INTERSECTIONS_TILL_OPAQUE)
				{
					if (RayMapIntersection(data->rayOrigin, data->rayDirection, data->map, &intersections[intersections_count], intersections_count + 2))
					{
						++intersections_count;
						if (cellColor == intersections[intersections_count - 1].cellInfo->type ||
							(cellTexture == intersections[intersections_count - 1].cellInfo->type && pixelTypeRGB == data->map->textures[intersections[intersections_count - 1].cellInfo->textureIndex].pixelType))
							break;
					}
					else
						break;
				}

				struct TRenderLineAtXData t_data = { 0 };
				t_data.map           = data->map;
				t_data.camera        = data->camera;
				t_data.window        = data->window;
				t_data.yMin          = y_bottom;
				t_data.yMax          = y_top;
				t_data.rayOrigin     = data->rayOrigin;
				t_data.rayDirection  = data->rayDirection;
				t_data.x             = data->x;
				t_data.forwardVector = &data->camera->forward;
				t_data.halfHeight    = data->camera->halfHeight;
				t_data.distance = 0.0f;

				for (int i = 0; i < intersections_count; ++i)
				{
					t_data.intersectionPointData = &intersections[intersections_count - i - 1];
					RenderLineAtX(&t_data, 0, DEFAULT_MIRROR_REFLECTION_COUNT);
				}
			}

			// Draw top texture
			int tex_x = (int)(data->intersectionPointData->intersectionTextureCoordinate * texture_width);

			for (int y = y_bottom; y <= y_top; ++y)
			{
				// Compute Y texture coordinate
				int tex_y = (int)((half_compensation + y - y_bottom) / wall_height * texture_height);

				int pixel_index = (tex_y * texture_width + tex_x) * texture_bytes_per_pixel;
				if (texture->data[pixel_index + 3] == 255)
				{
					uint8_t t_color[3];
					ComputeLight(data->map, &texture->data[pixel_index], t_color, data->intersectionPointData->intersectionPointDistance);
					TWindow_RenderDrawPointColor(data->window, data->x, y, t_color);
				}
			}
		}
	}
	else if (cellMirror == data->intersectionPointData->cellInfo->type)
	{
		/*if (mirrorReflectionCountLeft > 0)
		{
			float VdotN = data->rayDirection->x * data->intersectionPointData->intersectionNormal.x + data->rayDirection->y * data->intersectionPointData->intersectionNormal.y;

			struct Vec2f reflected_ray_direction;
			reflected_ray_direction.x = data->rayDirection->x - 2.0f * data->intersectionPointData->intersectionNormal.x * VdotN;
			reflected_ray_direction.y = data->rayDirection->y - 2.0f * data->intersectionPointData->intersectionNormal.y * VdotN;

			struct TRenderLineAtXData t_data;
			t_data.map           = data->map;
			t_data.camera        = data->camera;
			t_data.window        = data->window;
			t_data.yMin          = y_bottom;
			t_data.yMax          = y_top;
			t_data.rayOrigin     = &data->intersectionPointData->intersectionPoint;
			t_data.rayDirection  = &reflected_ray_direction;
			t_data.x             = data->x;
			t_data.forwardVector = &data->intersectionPointData->intersectionNormal;
			t_data.halfHeight =  data->camera->halfHeight;

			struct TIntersectionPointData intersection_point_data;
			TIntersectionPointData_InitZero(&intersection_point_data);

			if (RayMapIntersection(t_data.rayOrigin, t_data.rayDirection, data->map, &intersection_point_data, 2))
			{
				t_data.intersectionPointData = &intersection_point_data;

				t_data.distance = data->distance;
				t_data.distance += sqrtf(powf(t_data.rayOrigin->x - intersection_point_data.intersectionPoint.x, 2.0f) + powf(t_data.rayOrigin->y - intersection_point_data.intersectionPoint.y, 2.0f));
				t_data.distance = distance_perpendicular_to_camera_plane;

				RenderLineAtX(&t_data, 0, mirrorReflectionCountLeft - 1, mouseX, mouseY);
			}
		}*/
	}

	// Draw ceiling and floor
	if (y_bottom > data->yMin)
	{
		for (int y = data->yMin; y < y_bottom; ++y)
		{
			float current_dist = -data->window->height / (2.0f * half_height * (2.0f * y - data->window->height));
			
			double dist_player = 0.0;

			double dist_wall = distance_perpendicular_to_camera_plane;
			
			double weight = (current_dist - dist_player) / (dist_wall - dist_player);

			double current_floor_x = weight * data->intersectionPointData->intersectionPoint.x + (1.0 - weight) * data->rayOrigin->x;
			double current_floor_y = weight * data->intersectionPointData->intersectionPoint.y + (1.0 - weight) * data->rayOrigin->y;

			int current_floor_cell_x = (int)current_floor_x;
			int current_floor_cell_y = (int)current_floor_y;

			int cell_index = (TO_COORD(data->map->numCellsY) - current_floor_cell_y) * data->map->numCellsX + current_floor_cell_x;
			if (cell_index >= 0 && cell_index < data->map->numCellsX * data->map->numCellsY)
			{
				size_t cell_floor_index   = data->map->data[cell_index].floor;
				size_t cell_ceiling_index = data->map->data[cell_index].ceiling;

				if (cell_floor_index)
				{
					if (cellColor == data->map->cellsInfo[cell_floor_index].type)
					{
						uint8_t t_color[3];
						ComputeLight(data->map, data->map->cellsInfo[cell_floor_index].colour, t_color, current_dist);
						TWindow_SetRenderDrawColor(data->window, t_color[0], t_color[1], t_color[2], 255);

						TWindow_RenderDrawPoint(data->window, data->x, y);
					}
					else if (cellTexture == data->map->cellsInfo[cell_floor_index].type)
					{
						struct TTexture* texture = &data->map->textures[data->map->cellsInfo[cell_floor_index].textureIndex];

						int texture_coord_x = (int)((current_floor_x - current_floor_cell_x) * texture->width);
						int texture_coord_y = (int)((current_floor_y - current_floor_cell_y) * texture->height);

						uint8_t t_color[3];
						ComputeLight(data->map, &texture->data[(size_t)texture_coord_y * texture->width * texture->bytesPerPixel + texture_coord_x * texture->bytesPerPixel], t_color, current_dist);
						TWindow_RenderDrawPointColor(data->window, data->x, y, t_color);
					}
				}
				else
				{
					RenderEnvironment(data->window, data->map, data->x, y);
				}

				if (cell_ceiling_index)
				{
					if (cellColor == data->map->cellsInfo[cell_ceiling_index].type)
					{
						uint8_t t_color[3];
						ComputeLight(data->map, data->map->cellsInfo[cell_ceiling_index].colour, t_color, current_dist);
						TWindow_SetRenderDrawColor(data->window, t_color[0], t_color[1], t_color[2], 255);

						TWindow_RenderDrawPoint(data->window, data->x, TO_COORD(data->window->height) - y);
					}
					else if (cellTexture == data->map->cellsInfo[cell_ceiling_index].type)
					{
						struct TTexture* texture = &data->map->textures[data->map->cellsInfo[cell_ceiling_index].textureIndex];

						int texture_coord_x = (int)((current_floor_x - current_floor_cell_x) * texture->width);
						int texture_coord_y = (int)((current_floor_y - current_floor_cell_y) * texture->height);

						uint8_t t_color[3];
						ComputeLight(data->map, &texture->data[(size_t)texture_coord_y * texture->width * texture->bytesPerPixel + texture_coord_x * texture->bytesPerPixel], t_color, current_dist);
						TWindow_RenderDrawPointColor(data->window, data->x, TO_COORD(data->window->height) - y, t_color);
					}
				}
				else
				{
					RenderEnvironment(data->window, data->map, data->x, TO_COORD(data->window->height) - y);
				}
			}
		}
	}
}

void RenderMap2_5D(const struct TMap* map, const struct TCamera* camera, struct TWindow* window)
{
	struct TRenderLineAtXData line_at_x_data;
	line_at_x_data.map           = map;
	line_at_x_data.camera        = camera;
	line_at_x_data.window        = window;
	line_at_x_data.yMin          = 0;
	line_at_x_data.yMax          = TO_COORD(window->height);
	line_at_x_data.rayOrigin     = &camera->position;
	line_at_x_data.forwardVector = &camera->forward;
	line_at_x_data.halfHeight    = camera->halfHeight;

	for (int x = 0; x < window->width; ++x)
	{
		float normalized_coordinate = 2.0f * x / (float)window->width - 1.0f;

		struct Vec2f ray_direction;
		TCamera_GenerateRay(camera, normalized_coordinate, &ray_direction);

		line_at_x_data.rayDirection = &ray_direction;

		struct TIntersectionPointData intersection_point_data;
		TIntersectionPointData_InitZero(&intersection_point_data);

		if (RayMapIntersection(&camera->position, &ray_direction, map, &intersection_point_data, 1))
		{
			line_at_x_data.intersectionPointData = &intersection_point_data;
			line_at_x_data.x = x;

			line_at_x_data.distance = 0.0f;

			RenderLineAtX(&line_at_x_data, 1, DEFAULT_MIRROR_REFLECTION_COUNT);
		}
	}
}