#include "Map.h"

#include <stdio.h>
#include <stdlib.h>

#include <PropertiesFileParser.h>
#include <FileSystem.h>
#include <Texture.h>

struct PairIdIndex
{
	size_t id;
	size_t index;
};

/* 
	Finds first occurance of id in array.
	array argument is array of struct PairIdIndex
	index argument can be NULL.
	Returns 0 on success and != 0 on failure. 
*/
int FindIndexById(const struct CArray* array, size_t id, size_t* index)
{
	if (!array)
		return 1;

	for (size_t i = 0; i < array->count; ++i)
	{
		struct PairIdIndex* pair = NULL;
		if (caNoError == CArray_At(array, i, &pair))
		{
			if (id == pair->id)
			{
				if (index)
					*index = pair->index;
				return 0;
			}
		}
	}

	return 1;
}

static void CreateWrongFormatError(enum EnumMapError* error, union TMapErrorData* errorData, enum EnumMapFormatExpectedError expectedError, size_t arraySize, size_t offset)
{
	if (error)
		*error = mapFormatWrongError;
	if (errorData)
	{
		errorData->mapData.expectedError = expectedError;
		errorData->mapData.arraySize     = arraySize;
		errorData->mapData.offset        = offset;
	}
}

static void CreateCArrayError(enum EnumMapError* error, union TMapErrorData* errorData, enum EnumCArrayError caError)
{
	if (error)
		*error = mapCArrayError;
	if (errorData)
		errorData->carrayData.error = caError;
}

static void CreatePFPError(enum EnumMapError* error, union TMapErrorData* errorData, enum EnumPFPError pfpError, const union TPFPErrorData* pfpErrorData)
{
	if (error)
		*error = mapPFPError;
	if (errorData)
	{
		errorData->propertiesFileParser.error     = pfpError;
		errorData->propertiesFileParser.errorData = *pfpErrorData;
	}
}

static void CreateIDRedefinedOrNotFound(enum EnumMapError* error, union TMapErrorData* errorData, enum EnumMapError errorValue, size_t offset)
{
	if (error)
		*error = errorValue;
	if (errorData)
	{
		errorData->mapData.offset = offset;
	}
}

enum EnumMapError TMap_LoadFromFile(struct TMap* map, const char* filePath, union TMapErrorData* errorData)
{
	enum EnumMapError ret_error = mapNoError;

	struct TMap t_map;
	TMap_InitZero(&t_map);

	char*  buffer      = NULL;
	size_t buffer_size = 0;

	FILE* file_in = fopen(filePath, "rb");
	if (file_in)
	{
		fseek(file_in, 0, SEEK_END);
		size_t file_size = ftell(file_in);
		fseek(file_in, 0, SEEK_SET);

		buffer = (char*)malloc(file_size + 1);
		if (buffer)
		{
			if (fread(buffer, sizeof(char), file_size, file_in) == file_size)
			{
				buffer[file_size] = '\0';
				buffer_size = file_size + 1;
			}
			else
			{
				ret_error = mapCouldNotReadFromFileError;

				free(buffer);
				buffer = NULL;
			}
		}
		else
			ret_error = mapMallocError;

		fclose(file_in);
	}
	else
		ret_error = mapCouldNotOpenFileError;

	if (buffer)
	{
		struct TPFPProperty* data_property                   = NULL;
		struct TPFPProperty* cells_info_color_property       = NULL;
		struct TPFPProperty* cells_info_textures_property    = NULL;
		struct TPFPProperty* textures_property               = NULL;
		struct TPFPProperty* cells_info_that_are_mirrors     = NULL;
		struct TPFPProperty* environment_texture_id_property = NULL;

		struct CArray properties;
		CArray_InitZero(&properties);
		enum EnumCArrayError ca_error = CArray_Create(&properties, 1, sizeof(struct TPFPProperty), TPFPProperty_Clear);
		if (caNoError == ca_error)
		{
			union TPFPErrorData pfp_error_data;
			enum EnumPFPError pfp_error = PFP_ParseFromMemory(buffer, &properties, &pfp_error_data);
			if (pfpNoError == pfp_error)
			{
				for (size_t prop_i = 0; prop_i < properties.count; ++prop_i)
				{
					if (mapNoError != ret_error)
						break;

					struct TPFPProperty* property = NULL;
					ca_error = CArray_At(&properties, prop_i, &property);
					if (caNoError == ca_error)
					{
						if (strcmp(property->name, "title") == 0)
						{
							if (pfpSimpleValue == property->type && pfpStringSimpleValue == property->value.simpleValue.type)
							{
								size_t str_length = strlen(property->value.simpleValue.data.stringData);
								t_map.title = malloc(str_length + 1);
								if (t_map.title)
									strcpy(t_map.title, property->value.simpleValue.data.stringData);
								else
									ret_error = mapMallocError;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedString, 0, property->offset);
						}
						else if (strcmp(property->name, "playerStartPos") == 0)
						{
							if (pfpArrayValue == property->type && 2 == property->value.arrayOfSimpleValues.count)
							{
								struct TPFPSimpleValue* element = NULL;
								ca_error = CArray_At(&property->value.arrayOfSimpleValues, 0, &element);
								if (caNoError == ca_error)
								{
									if (pfpFloatSimpleValue == element->type)
										t_map.playerStartPos.x = element->data.floatData;
									else
										CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, element->offset);
								}
								else
									CreateCArrayError(&ret_error, errorData, ca_error);

								if (mapNoError == ret_error)
								{
									element = NULL;
									ca_error = CArray_At(&property->value.arrayOfSimpleValues, 1, &element);
									if (caNoError == ca_error)
									{
										if (pfpFloatSimpleValue == element->type)
											t_map.playerStartPos.y = element->data.floatData;
										else
											CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, element->offset);
									}
									else
										CreateCArrayError(&ret_error, errorData, ca_error);
								}
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSize, 2, property->offset);
						}
						else if (strcmp(property->name, "size") == 0)
						{
							if (pfpArrayValue == property->type && 2 == property->value.arrayOfSimpleValues.count)
							{
								struct TPFPSimpleValue* element = NULL;
								ca_error = CArray_At(&property->value.arrayOfSimpleValues, 0, &element);
								if (caNoError == ca_error)
								{
									if (pfpIntSimpleValue == element->type)
										t_map.numCellsX = element->data.intData;
									else
										CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, element->offset);
								}
								else
									CreateCArrayError(&ret_error, errorData, ca_error);

								if (mapNoError == ret_error)
								{
									element = NULL;
									ca_error = CArray_At(&property->value.arrayOfSimpleValues, 1, &element);
									if (caNoError == ca_error)
									{
										if (pfpIntSimpleValue == element->type)
											t_map.numCellsY = element->data.intData;
										else
											CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, element->offset);
									}
									else
										CreateCArrayError(&ret_error, errorData, ca_error);
								}
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSize, 2, property->offset);
						}
						else if (strcmp(property->name, "playerHasLight") == 0)
						{
							if (pfpSimpleValue == property->type && pfpIntSimpleValue == property->value.simpleValue.type)
							{
								t_map.playerHasLight = property->value.simpleValue.data.intData;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
						}
						else if (strcmp(property->name, "playerLightDiffuseStrength") == 0)
						{
							if (pfpSimpleValue == property->type && pfpFloatSimpleValue == property->value.simpleValue.type)
							{
								t_map.playerLightDiffuseStrength = property->value.simpleValue.data.floatData;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedFloat, 0, property->offset);
						}
						else if (strcmp(property->name, "playerLightDiffusePower") == 0)
						{
							if (pfpSimpleValue == property->type && pfpFloatSimpleValue == property->value.simpleValue.type)
							{
								t_map.playerLightDiffusePower = property->value.simpleValue.data.floatData;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedFloat, 0, property->offset);
						}
						else if (strcmp(property->name, "playerLightDiffuseColor") == 0)
						{
							if (pfpArrayValue == property->type && 3 == property->value.arrayOfSimpleValues.count)
							{
								struct TPFPSimpleValue* element = NULL;
								ca_error = CArray_At(&property->value.arrayOfSimpleValues, 0, &element);
								if (caNoError == ca_error)
								{
									if (pfpIntSimpleValue == element->type)
										t_map.playerLightDiffuseColor[0] = element->data.intData;
									else 
										CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
								}
								else
									CreateCArrayError(&ret_error, errorData, ca_error);

								if (mapNoError == ret_error)
								{
									ca_error = CArray_At(&property->value.arrayOfSimpleValues, 1, &element);
									if (caNoError == ca_error)
									{
										if (pfpIntSimpleValue == element->type)
											t_map.playerLightDiffuseColor[1] = element->data.intData;
										else
											CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
									}
									else
										CreateCArrayError(&ret_error, errorData, ca_error);
								}

								if (mapNoError == ret_error)
								{
									ca_error = CArray_At(&property->value.arrayOfSimpleValues, 2, &element);
									if (caNoError == ca_error)
									{
										if (pfpIntSimpleValue == element->type)
											t_map.playerLightDiffuseColor[2] = element->data.intData;
										else
											CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
									}
									else
										CreateCArrayError(&ret_error, errorData, ca_error);
								}
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSize, 3, property->offset);
						}
						else if (strcmp(property->name, "hasAmbientLight") == 0)
						{
							if (pfpSimpleValue == property->type && pfpIntSimpleValue == property->value.simpleValue.type)
							{
								t_map.hasAmbientLight = property->value.simpleValue.data.intData;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
						}
						else if (strcmp(property->name, "ambientLightStrength") == 0)
						{
							if (pfpSimpleValue == property->type && pfpFloatSimpleValue == property->value.simpleValue.type)
							{
								t_map.ambientLightStrength = property->value.simpleValue.data.floatData;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
						}
						else if (strcmp(property->name, "ambientLightColor") == 0)
						{
							if (pfpArrayValue == property->type && 3 == property->value.arrayOfSimpleValues.count)
							{
								struct TPFPSimpleValue* element = NULL;
								ca_error = CArray_At(&property->value.arrayOfSimpleValues, 0, &element);
								if (caNoError == ca_error)
								{
									if (pfpIntSimpleValue == element->type)
										t_map.ambientLightColor[0] = element->data.intData;
									else
										CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
								}
								else
									CreateCArrayError(&ret_error, errorData, ca_error);

								if (mapNoError == ret_error)
								{
									ca_error = CArray_At(&property->value.arrayOfSimpleValues, 1, &element);
									if (caNoError == ca_error)
									{
										if (pfpIntSimpleValue == element->type)
											t_map.ambientLightColor[1] = element->data.intData;
										else
											CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
									}
									else
										CreateCArrayError(&ret_error, errorData, ca_error);
								}

								if (mapNoError == ret_error)
								{
									ca_error = CArray_At(&property->value.arrayOfSimpleValues, 2, &element);
									if (caNoError == ca_error)
									{
										if (pfpIntSimpleValue == element->type)
											t_map.ambientLightColor[2] = element->data.intData;
										else
											CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
									}
									else
										CreateCArrayError(&ret_error, errorData, ca_error);
								}
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSize, 3, property->offset);
						}
						else if (strcmp(property->name, "environmentIsTexture") == 0)
						{
							if (pfpSimpleValue == property->type && pfpIntSimpleValue == property->value.simpleValue.type)
							{
								t_map.environmentIsTexture = property->value.simpleValue.data.intData;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
						}
						else if (strcmp(property->name, "environmentColor") == 0)
						{
							if (pfpArrayValue == property->type && 3 == property->value.arrayOfSimpleValues.count)
							{
								struct TPFPSimpleValue* element = NULL;
								ca_error = CArray_At(&property->value.arrayOfSimpleValues, 0, &element);
								if (caNoError == ca_error)
								{
									if (pfpIntSimpleValue == element->type)
										t_map.environmentColor[0] = element->data.intData;
									else
										CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
								}
								else
									CreateCArrayError(&ret_error, errorData, ca_error);

								if (mapNoError == ret_error)
								{
									ca_error = CArray_At(&property->value.arrayOfSimpleValues, 1, &element);
									if (caNoError == ca_error)
									{
										if (pfpIntSimpleValue == element->type)
											t_map.environmentColor[1] = element->data.intData;
										else
											CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
									}
									else
										CreateCArrayError(&ret_error, errorData, ca_error);
								}

								if (mapNoError == ret_error)
								{
									ca_error = CArray_At(&property->value.arrayOfSimpleValues, 2, &element);
									if (caNoError == ca_error)
									{
										if (pfpIntSimpleValue == element->type)
											t_map.environmentColor[2] = element->data.intData;
										else
											CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
									}
									else
										CreateCArrayError(&ret_error, errorData, ca_error);
								}
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSize, 3, property->offset);
						}
						else if (strcmp(property->name, "environmentTextureId") == 0)
						{
							if (pfpSimpleValue == property->type && pfpIntSimpleValue == property->value.simpleValue.type)
							{
								environment_texture_id_property = property;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, property->offset);
						}
						else if (strcmp(property->name, "data") == 0)
						{
							if (pfpArrayValue == property->type)
							{
								data_property = property;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArray, 0, property->offset);
						}
						else if (strcmp(property->name, "cellsInfoColor") == 0)
						{
							if (pfpArrayValue == property->type && property->value.arrayOfSimpleValues.count % 4 == 0)
							{
								cells_info_color_property = property;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSizeMultipleOf, 4, property->offset);
						}
						else if (strcmp(property->name, "cellsInfoTexture") == 0)
						{
							if (pfpArrayValue == property->type && property->value.arrayOfSimpleValues.count % 2 == 0)
							{
								cells_info_textures_property = property;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSizeMultipleOf, 2, property->offset);
						}
						else if (strcmp(property->name, "textures") == 0)
						{
							if (pfpArrayValue == property->type && property->value.arrayOfSimpleValues.count % 2 == 0)
							{
								textures_property = property;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSizeMultipleOf, 2, property->offset);
						}
						else if (strcmp(property->name, "cellsInfoThatAreMirror") == 0)
						{
							if (pfpArrayValue == property->type)
							{
								cells_info_that_are_mirrors = property;
							}
							else
								CreateWrongFormatError(&ret_error, errorData, mapExpectedArray, 0, property->offset);
						}
					}
					else 
						CreateCArrayError(&ret_error, errorData, ca_error);
				}
			}
			else
				CreatePFPError(&ret_error, errorData, pfp_error, &pfp_error_data);

			// Load textures
			struct CArray textures_array;
			struct CArray textures_id_to_index_array;
			CArray_InitZero(&textures_array);
			CArray_InitZero(&textures_id_to_index_array);
			if (mapNoError == ret_error && textures_property)
			{
				enum EnumCArrayError ca_error_textures_array             = CArray_Create(&textures_array, 5, sizeof(struct TTexture), TTexture_Clear);
				enum EnumCArrayError ca_error_textures_id_to_intex_array = CArray_Create(&textures_id_to_index_array, 5, sizeof(struct PairIdIndex), NULL);

				if (caNoError != ca_error_textures_array)
					CreateCArrayError(&ret_error, errorData, ca_error_textures_array);
				if (caNoError != ca_error_textures_id_to_intex_array)
					CreateCArrayError(&ret_error, errorData, ca_error_textures_id_to_intex_array);

				if (mapNoError == ret_error)
				{
					for (size_t texture_i = 0; texture_i < textures_property->value.arrayOfSimpleValues.count; texture_i += 2)
					{
						if (mapNoError != ret_error)
							break;

						struct TPFPSimpleValue* texture_id_value   = NULL;
						struct TPFPSimpleValue* texture_path_value = NULL;

						enum EnumCArrayError ca_error_texture_id   = CArray_At(&textures_property->value.arrayOfSimpleValues, texture_i, &texture_id_value);
						enum EnumCArrayError ca_error_texture_path = CArray_At(&textures_property->value.arrayOfSimpleValues, texture_i + 1, &texture_path_value);

						if (caNoError != ca_error_texture_id || !texture_id_value)
						{
							CreateCArrayError(&ret_error, errorData, ca_error_texture_id);
							break;
						}
						if (caNoError != ca_error_texture_path || !texture_path_value)
						{
							CreateCArrayError(&ret_error, errorData, ca_error_texture_path);
							break;
						}

						// Check format texture
						if (pfpIntSimpleValue != texture_id_value->type)
						{
							CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, texture_id_value->offset);
							break;
						}
						if (pfpStringSimpleValue != texture_path_value->type)
						{
							CreateWrongFormatError(&ret_error, errorData, mapExpectedString, 0, texture_path_value->offset);
							break;
						}

						if (0 != FindIndexById(&textures_id_to_index_array, (size_t)texture_id_value->data.intData, NULL))
						{
							struct TTexture texture;
							TTexture_InitZero(&texture);

							char final_texture_path[1024] = { '\0' };
							GetPathWithoutFile(filePath, final_texture_path);
							if (strlen(final_texture_path) <= 1023 - 1)
								strcat(final_texture_path, "/");
							if (strlen(final_texture_path) <= 1023 - strlen(texture_path_value->data.stringData))
								strcat(final_texture_path, texture_path_value->data.stringData);

							if (0 == TTexture_LoadFromFile(&texture, final_texture_path, 1))
							{
								struct PairIdIndex pair_id_index = { (size_t)texture_id_value->data.intData, textures_array.count };

								enum EnumCArrayError ca_error = CArray_PushBack(&textures_array, &texture);

								if (caNoError == ca_error)
								{
									enum EnumCArrayError ca_error = CArray_PushBack(&textures_id_to_index_array, &pair_id_index);

									if (caNoError == ca_error)
									{
										// All good. Texture and pair id, index pushed.
									}
									else
									{
										CreateCArrayError(&ret_error, errorData, ca_error);
									}
								}
								else
								{
									CreateCArrayError(&ret_error, errorData, ca_error);
								}
							}
							else
							{
								ret_error = mapCouldNotLoadTexture;
							}
						}
						else
						{
							CreateIDRedefinedOrNotFound(&ret_error, errorData, mapTextureIDRedefinedError, texture_id_value->offset);
						}
					}
				}
			}

			// Load cells info
			struct CArray cells_info_array;
			struct CArray cells_info_id_to_index_array;
			CArray_InitZero(&cells_info_array);
			CArray_InitZero(&cells_info_id_to_index_array);
			if (mapNoError == ret_error && cells_info_color_property && cells_info_textures_property)
			{
				enum EnumCArrayError ca_error_cells_info_array             = CArray_Create(&cells_info_array, 5, sizeof(struct TCellInfo), NULL);
				enum EnumCArrayError ca_error_cells_info_id_to_index_array = CArray_Create(&cells_info_id_to_index_array, 5, sizeof(struct PairIdIndex), NULL);

				if (caNoError != ca_error_cells_info_array)
					CreateCArrayError(&ret_error, errorData, ca_error_cells_info_array);
				if (caNoError != ca_error_cells_info_id_to_index_array)
					CreateCArrayError(&ret_error, errorData, ca_error_cells_info_id_to_index_array);

				if (mapNoError == ret_error)
				{
					struct TCellInfo zero_dummmy_cell_info;
					zero_dummmy_cell_info.type = cellUndefined;

					enum EnumCArrayError ca_error = CArray_PushBack(&cells_info_array, &zero_dummmy_cell_info);
					if (caNoError == ca_error)
					{
						// parse color
						for (size_t cell_info_i = 0; cell_info_i < cells_info_color_property->value.arrayOfSimpleValues.count; cell_info_i += 4)
						{
							if (mapNoError != ret_error)
								break;

							struct TPFPSimpleValue* cell_info_id_value = NULL;
							struct TPFPSimpleValue* colour_r_value     = NULL;
							struct TPFPSimpleValue* colour_g_value     = NULL;
							struct TPFPSimpleValue* colour_b_value     = NULL;

							enum EnumCArrayError ca_error = CArray_At(&cells_info_color_property->value.arrayOfSimpleValues, cell_info_i, &cell_info_id_value);
							if (caNoError != ca_error || !cell_info_id_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							ca_error = CArray_At(&cells_info_color_property->value.arrayOfSimpleValues, cell_info_i + 1, &colour_r_value);
							if (caNoError != ca_error || !colour_r_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							ca_error = CArray_At(&cells_info_color_property->value.arrayOfSimpleValues, cell_info_i + 2, &colour_g_value);
							if (caNoError != ca_error || !colour_g_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							ca_error = CArray_At(&cells_info_color_property->value.arrayOfSimpleValues, cell_info_i + 3, &colour_b_value);
							if (caNoError != ca_error || !colour_b_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}

							// Check format of cell info color
							if (pfpIntSimpleValue != cell_info_id_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_info_id_value->offset);
								break;
							}
							if (pfpIntSimpleValue != colour_r_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, colour_r_value->offset);
								break;
							}
							if (pfpIntSimpleValue != colour_g_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, colour_g_value->offset);
								break;
							}
							if (pfpIntSimpleValue != colour_b_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, colour_b_value->offset);
								break;
							}

							if (0 != FindIndexById(&cells_info_id_to_index_array, cell_info_id_value->data.intData, NULL))
							{
								struct TCellInfo cell_info;
								cell_info.type = cellColor;
								cell_info.colour[0] = ColorGammaDecode(colour_r_value->data.intData, DEFAULT_GAMMA);
								cell_info.colour[1] = ColorGammaDecode(colour_g_value->data.intData, DEFAULT_GAMMA);
								cell_info.colour[2] = ColorGammaDecode(colour_b_value->data.intData, DEFAULT_GAMMA);

								struct PairIdIndex pair_id_index = { (size_t)cell_info_id_value->data.intData, cells_info_array.count };

								enum EnumCArrayError ca_error = CArray_PushBack(&cells_info_array, &cell_info);
								if (caNoError == ca_error)
								{
									enum EnumCArrayError ca_error = CArray_PushBack(&cells_info_id_to_index_array, &pair_id_index);
									if (caNoError == ca_error)
									{
										// All good cell info colour and id, index pair pushed.
									}
									else
									{
										CreateCArrayError(&ret_error, errorData, ca_error);
									}
								}
								else
								{
									CreateCArrayError(&ret_error, errorData, ca_error);
								}
							}
							else
							{
								CreateIDRedefinedOrNotFound(&ret_error, errorData, mapCellInfoIDRedefinedError, cell_info_id_value->offset);
							}
						}

						// parse texture
						if (mapNoError == ret_error)
						{
							for (size_t cell_info_i = 0; cell_info_i < cells_info_textures_property->value.arrayOfSimpleValues.count; cell_info_i += 2)
							{
								if (mapNoError != ret_error)
									break;

								struct TPFPSimpleValue* cell_info_id_value = NULL;
								struct TPFPSimpleValue* texture_id_value   = NULL;

								enum EnumCArrayError ca_error_cell_info_id = CArray_At(&cells_info_textures_property->value.arrayOfSimpleValues, cell_info_i, &cell_info_id_value);
								enum EnumCArrayError ca_error_texture_id   = CArray_At(&cells_info_textures_property->value.arrayOfSimpleValues, cell_info_i + 1, &texture_id_value);

								if (caNoError != ca_error_cell_info_id || !cell_info_id_value)
								{
									CreateCArrayError(&ret_error, errorData, ca_error_cell_info_id);
									break;
								}
								if (caNoError != ca_error_texture_id || !texture_id_value)
								{
									CreateCArrayError(&ret_error, errorData, ca_error_texture_id);
									break;
								}

								if (pfpIntSimpleValue != cell_info_id_value->type)
								{
									CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_info_id_value->offset);
									break;
								}
								if(pfpIntSimpleValue != texture_id_value->type)
								{
									CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, texture_id_value->offset);
									break;
								}

								if (0 != FindIndexById(&cells_info_id_to_index_array, cell_info_id_value->data.intData, NULL))
								{
									size_t texture_index = 0;
									if (0 == FindIndexById(&textures_id_to_index_array, texture_id_value->data.intData, &texture_index))
									{
										struct TCellInfo cell_info;
										cell_info.type = cellTexture;
										cell_info.textureIndex = texture_index;

										struct PairIdIndex pair_id_index = { (size_t)cell_info_id_value->data.intData, cells_info_array.count };

										enum EnumCArrayError ca_error = CArray_PushBack(&cells_info_array, &cell_info);
										if (caNoError == ca_error)
										{
											enum EnumCArrayError ca_error = CArray_PushBack(&cells_info_id_to_index_array, &pair_id_index);
											if (caNoError == ca_error)
											{
												// All good cell info colour and id, index pair pushed.
											}
											else
											{
												CreateCArrayError(&ret_error, errorData, ca_error);
											}
										}
										else
										{
											CreateCArrayError(&ret_error, errorData, ca_error);
										}
									}
									else
									{
										CreateIDRedefinedOrNotFound(&ret_error, errorData, mapTextureIDNotFoundError, texture_id_value->offset);
									}
								}
								else
								{
									CreateIDRedefinedOrNotFound(&ret_error, errorData, mapCellInfoIDRedefinedError, cell_info_id_value->offset);
								}
							}
						}

						// Parse cells info that are mirrors
						if (cells_info_that_are_mirrors)
						{
							for (size_t i = 0; i < cells_info_that_are_mirrors->value.arrayOfSimpleValues.count; ++i)
							{
								struct TPFPSimpleValue* element = NULL;
								ca_error = CArray_At(&cells_info_that_are_mirrors->value.arrayOfSimpleValues, i, &element);
								if (caNoError == ca_error)
								{
									if (pfpIntSimpleValue == element->type)
									{
										size_t cell_info_index = 0;
										if (0 == FindIndexById(&cells_info_id_to_index_array, element->data.intData, &cell_info_index))
										{
											// Change the existing one
											struct TCellInfo* found_cell_info = NULL;
											ca_error = CArray_At(&cells_info_array, cell_info_index, &found_cell_info);
											if (caNoError == ca_error)
											{
												if (cellTexture == found_cell_info->type)
												{
													found_cell_info->type = cellMirrorWithTexture;
												}
												else
													CreateIDRedefinedOrNotFound(&ret_error, errorData, mapColorCellInfoCannotBeMirror, element->offset);
											}
											else
												CreateCArrayError(&ret_error, errorData, ca_error);
										}
										else
										{
											// Insert as new
											struct TCellInfo cell_info;
											cell_info.type = cellMirror;

											struct PairIdIndex pair_id_index = { (size_t)element->data.intData, cells_info_array.count };

											enum EnumCArrayError ca_error = CArray_PushBack(&cells_info_array, &cell_info);
											if (caNoError == ca_error)
											{
												enum EnumCArrayError ca_error = CArray_PushBack(&cells_info_id_to_index_array, &pair_id_index);
												if (caNoError == ca_error)
												{
													// All good cell infoa and id, index pair pushed.
												}
												else
												{
													CreateCArrayError(&ret_error, errorData, ca_error);
												}
											}
											else
												CreateCArrayError(&ret_error, errorData, ca_error);
										}
									}
									else
										CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, element->offset);
								}
								else
									CreateCArrayError(&ret_error, errorData, ca_error);
							}
						}
					}
					else
					{
						CreateCArrayError(&ret_error, errorData, ca_error);
					}
				}
			}

			if (mapNoError == ret_error && data_property)
			{
				if ((size_t)t_map.numCellsX * t_map.numCellsY * 6 == data_property->value.arrayOfSimpleValues.count)
				{
					t_map.data = malloc(sizeof(size_t) * data_property->value.arrayOfSimpleValues.count);
					if (t_map.data)
					{
						for (size_t cell_i = 0; cell_i < data_property->value.arrayOfSimpleValues.count; cell_i += 6)
						{
							if (mapNoError != ret_error)
								break;

							/*struct TPFPSimpleValue* cell_info_id_value = NULL;
							enum EnumCArrayError ca_error = CArray_At(&data_property->value.arrayOfSimpleValues, cell_i, &cell_info_id_value);
							if (caNoError != ca_error || !cell_info_id_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							if (pfpIntSimpleValue != cell_info_id_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_info_id_value->offset);
								break;
							}
							*/

							struct TPFPSimpleValue* cell_top_id_value     = NULL;
							struct TPFPSimpleValue* cell_bottom_id_value  = NULL;
							struct TPFPSimpleValue* cell_left_id_value    = NULL;
							struct TPFPSimpleValue* cell_right_id_value   = NULL;
							struct TPFPSimpleValue* cell_ceiling_id_value = NULL;
							struct TPFPSimpleValue* cell_floor_id_value   = NULL;

							enum EnumCArrayError ca_error = caNoError;

							// Top
							ca_error = CArray_At(&data_property->value.arrayOfSimpleValues, cell_i, &cell_top_id_value);
							if (caNoError != ca_error || !cell_top_id_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							if (pfpIntSimpleValue != cell_top_id_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_top_id_value->offset);
								break;
							}

							// Bottom
							ca_error = CArray_At(&data_property->value.arrayOfSimpleValues, cell_i + 1, &cell_bottom_id_value);
							if (caNoError != ca_error || !cell_bottom_id_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							if (pfpIntSimpleValue != cell_bottom_id_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_bottom_id_value->offset);
								break;
							}

							// Left
							ca_error = CArray_At(&data_property->value.arrayOfSimpleValues, cell_i + 2, &cell_left_id_value);
							if (caNoError != ca_error || !cell_left_id_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							if (pfpIntSimpleValue != cell_left_id_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_left_id_value->offset);
								break;
							}

							// Right
							ca_error = CArray_At(&data_property->value.arrayOfSimpleValues, cell_i + 3, &cell_right_id_value);
							if (caNoError != ca_error || !cell_right_id_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							if (pfpIntSimpleValue != cell_right_id_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_right_id_value->offset);
								break;
							}

							// Ceiling
							ca_error = CArray_At(&data_property->value.arrayOfSimpleValues, cell_i + 4, &cell_ceiling_id_value);
							if (caNoError != ca_error || !cell_ceiling_id_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							if (pfpIntSimpleValue != cell_ceiling_id_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_ceiling_id_value->offset);
								break;
							}

							// Floor
							ca_error = CArray_At(&data_property->value.arrayOfSimpleValues, cell_i + 5, &cell_floor_id_value);
							if (caNoError != ca_error || !cell_floor_id_value)
							{
								CreateCArrayError(&ret_error, errorData, ca_error);
								break;
							}
							if (pfpIntSimpleValue != cell_floor_id_value->type)
							{
								CreateWrongFormatError(&ret_error, errorData, mapExpectedInt, 0, cell_floor_id_value->offset);
								break;
							}

							/*size_t cell_info_index = 0;
							FindIndexById(&cells_info_id_to_index_array, cell_info_id_value->data.intData, &cell_info_index);
							t_map.data[cell_i] = cell_info_index;*/

							size_t top_cell_info_index     = 0;
							size_t bottom_cell_info_index  = 0;
							size_t left_cell_info_index    = 0;
							size_t right_cell_info_index   = 0;
							size_t ceiling_cell_info_index = 0;
							size_t floor_cell_info_index   = 0;
							FindIndexById(&cells_info_id_to_index_array, cell_top_id_value->data.intData, &top_cell_info_index);
							FindIndexById(&cells_info_id_to_index_array, cell_bottom_id_value->data.intData, &bottom_cell_info_index);
							FindIndexById(&cells_info_id_to_index_array, cell_left_id_value->data.intData, &left_cell_info_index);
							FindIndexById(&cells_info_id_to_index_array, cell_right_id_value->data.intData, &right_cell_info_index);
							FindIndexById(&cells_info_id_to_index_array, cell_ceiling_id_value->data.intData, &ceiling_cell_info_index);
							FindIndexById(&cells_info_id_to_index_array, cell_floor_id_value->data.intData, &floor_cell_info_index);

							size_t cell_index = cell_i / 6;
							t_map.data[cell_index].top     = (int)top_cell_info_index;
							t_map.data[cell_index].bottom  = (int)bottom_cell_info_index;
							t_map.data[cell_index].left    = (int)left_cell_info_index;
							t_map.data[cell_index].right   = (int)right_cell_info_index;
							t_map.data[cell_index].ceiling = (int)ceiling_cell_info_index;
							t_map.data[cell_index].floor   = (int)floor_cell_info_index;
						}

						if (mapNoError != ret_error)
						{
							free(t_map.data);
							t_map.data = NULL;
						}
					}
					else
					{
						ret_error = mapMallocError;
					}
				}
				else
				{
					CreateWrongFormatError(&ret_error, errorData, mapExpectedArrayOfSize, (size_t)t_map.numCellsX* t_map.numCellsY, data_property->offset);
				}
			}

			// Environment texture
			if (mapNoError == ret_error && environment_texture_id_property)
			{
				size_t texture_index = 0;

				if (0 == FindIndexById(&textures_id_to_index_array, environment_texture_id_property->value.simpleValue.data.intData, &texture_index))
				{
					t_map.environmentHasTexture   = 1;
					t_map.environmentTextureIndex = texture_index;
				}
			}
			// End environment texture

			// copy cells info 
			if (mapNoError == ret_error)
			{
				t_map.cellsInfoCount = cells_info_array.count;
				t_map.cellsInfo      = malloc(sizeof(struct TCellInfo) * t_map.cellsInfoCount);
				if (t_map.cellsInfo)
				{
					for (size_t cell_info_i = 0; cell_info_i < cells_info_array.count; ++cell_info_i)
					{
						struct TCellInfo* cell_info_value = NULL;

						enum EnumCArrayError ca_error = CArray_At(&cells_info_array, cell_info_i, &cell_info_value);
						if (caNoError != ca_error || !cell_info_value)
						{
							CreateCArrayError(&ret_error, errorData, ca_error);
							break;
						}
						
						t_map.cellsInfo[cell_info_i] = *cell_info_value;
					}
				}
				else
				{
					ret_error = mapMallocError;
				}
			}

			// copy textures
			if (mapNoError == ret_error)
			{
				t_map.texturesCount = textures_array.count;
				t_map.textures = malloc(sizeof(struct TTexture) * t_map.texturesCount);
				if (t_map.textures)
				{
					for (size_t texture_i = 0; texture_i < textures_array.count; ++texture_i)
					{
						struct TTexture* texture_value = NULL;

						enum EnumCArrayError ca_error = CArray_At(&textures_array, texture_i, &texture_value);
						if (caNoError != ca_error || !texture_value)
						{
							CreateCArrayError(&ret_error, errorData, ca_error);
							break;
						}

						t_map.textures[texture_i] = *texture_value;
					}
				}
				else
				{
					ret_error = mapMallocError;
				}
			}

			if (mapNoError != ret_error)
			{
				CArray_Clear(&cells_info_array);
				CArray_Clear(&textures_array);
			}
			CArray_Clear(&cells_info_id_to_index_array);
			CArray_Clear(&textures_id_to_index_array);

			CArray_Clear(&properties);
		}
		else
		{
			CreateCArrayError(&ret_error, errorData, ca_error);
		}

		free(buffer);
		buffer = NULL;
	}

	if (mapNoError != ret_error)
		TMap_Clear(&t_map);
	else
		*map = t_map;

	return ret_error;
}

void TMap_InitZero(void* map)
{
	struct TMap* t_map = (struct TMap*)map;
	if (t_map)
	{
		t_map->title            = NULL;
		t_map->numCellsX        = 0;
		t_map->numCellsY        = 0;
		t_map->playerStartPos.x = 0.0f;
		t_map->playerStartPos.y = 0.0f;
		t_map->data             = NULL;
		t_map->cellsInfo        = NULL;
		t_map->cellsInfoCount   = 0;
		t_map->textures         = NULL;
		t_map->texturesCount    = 0;

		t_map->playerHasLight             = 0;
		t_map->playerLightDiffuseStrength = 1.0f;
		t_map->playerLightDiffusePower    = 1.0f;
		t_map->playerLightDiffuseColor[0] = 255;
		t_map->playerLightDiffuseColor[1] = 255;
		t_map->playerLightDiffuseColor[2] = 255;

		t_map->hasAmbientLight      = 0;
		t_map->ambientLightStrength = 1.0f;
		t_map->ambientLightColor[0] = 255;
		t_map->ambientLightColor[1] = 255;
		t_map->ambientLightColor[2] = 255;

		t_map->environmentIsTexture    = 0;
		t_map->environmentColor[0]     = 255;
		t_map->environmentColor[1]     = 255;
		t_map->environmentColor[2]     = 255;
		t_map->environmentHasTexture   = 0;
		t_map->environmentTextureIndex = 0;
	}
}

void TMap_Clear(void* map)
{
	if (map)
	{
		struct TMap* t_map = map;

		if (t_map->title)
			free(t_map->title);

		if (t_map->data)
			free(t_map->data);

		if (t_map->cellsInfo)
			free(t_map->cellsInfo);

		if (t_map->textures)
			free(t_map->textures);

		TMap_InitZero(t_map);
	}
}

void TMap_PrintScreen(const struct TMap* map)
{
	if (map)
	{
		if (map->title)
			printf("Title: %s\n", map->title);

		if (map->textures)
		{
			printf("Textures: \n");
			for (size_t texture_i = 0; texture_i < map->texturesCount; ++texture_i)
				printf("\t%zd) %dx%d\n", texture_i, map->textures[texture_i].width, map->textures[texture_i].height);
		}
		else
			printf("No textures!\n");
		if (map->cellsInfo)
		{
			printf("Cells info: \n");
			for (size_t cell_info_i = 0; cell_info_i < map->cellsInfoCount; ++cell_info_i)
			{
				printf("\t%zd)", cell_info_i);
				if (cellUndefined == map->cellsInfo[cell_info_i].type)
					printf(" Undefined\n");
				else if (cellTexture == map->cellsInfo[cell_info_i].type)
					printf(" textureIndex=%zd\n", map->cellsInfo[cell_info_i].textureIndex);
				else if (cellColor == map->cellsInfo[cell_info_i].type)
					printf(" colour=(%d, %d, %d)\n", map->cellsInfo[cell_info_i].colour[0], map->cellsInfo[cell_info_i].colour[1], map->cellsInfo[cell_info_i].colour[2]);
				else if (cellMirror == map->cellsInfo[cell_info_i].type)
					printf(" mirror\n");
				else if(cellMirrorWithTexture == map->cellsInfo[cell_info_i].type)
					printf(" mirror with texture. textureIndex=%zd\n", map->cellsInfo[cell_info_i].textureIndex);
			}
		}
		else
			printf("No cells info!\n");
		if (map->data)
		{
			printf("Data: \n");
			for (size_t y = 0; y < map->numCellsY; ++y)
			{
				printf("\t");
				for (size_t x = 0; x < map->numCellsX; ++x)
					printf("%d, %d, %d, %d, %d, %d\t", map->data[y * map->numCellsX + x].top, map->data[y * map->numCellsX + x].bottom, map->data[y * map->numCellsX + x].left, map->data[y * map->numCellsX + x].right, map->data[y * map->numCellsX + x].ceiling, map->data[y * map->numCellsX + x].floor);
				printf("\n");
			}
		}
	}
}

static const char* expectedErrorsStrs[] =
{
	/* mapExpectedUndefined */ "Unknown error.",

	/* mapExpectedString */                "Expected string value.",
	/* mapExpectedArray */                 "Expected array value.",
	/* mapExpectedArrayOfSize */           "Expected array of size",
	/* mapExpectedArrayOfSizeMultipleOf */ "Expected array of size multiple of",
	/* mapExpectedInt */                   "Expected int value.",
	/* mapExpectedFloat */                 "Expected float value."
};

static const char* errorNotFoundStr = "No string for this error.";
static const char* expectedErrorNotFoundStr = "No string for this expected error.";

static const char* errorsStrs[] =
{
	/* mapNoError */ "No error.",

	/* mapCouldNotOpenFileError */     "Could not open file.",
	/* mapCouldNotReadFromFileError */ "Could not read from file.",
	/* mapCouldNotLoadTexture */       "Could not Load Texture.",
	/* mapMallocError */               "Malloc function failed.",

	/* mapCArrayError */ "CArray error.",
	/* mapPFPError */    "PropertiesFileParser error.",

	/* mapTexturesPropertyNotFoundError */         "Property 'textures' not found.",
	/* mapCellsInfoTexturePropertyNotFoundError */ "Property 'cellsInfoTextures' not found.",
	/* mapCellsInfoColorPropertyNotFoundError */   "Property 'cellsInfoColor' not found.",
	/* mapDataPropertyNotFoundError */             "Property 'data' not found.",

	/* mapSizePropertyNotFoundError */      "Property 'size' not found.",
	/* mapPlayerStartPosPropertyNotFound */ "Property 'playerStartPos' not found.",
	/* mapTitlePropertyNotFound */          "Property 'title' not found.",

	/* mapCellInfoIDRedefinedError */ "CellInfoID redefined.",
	/* mapCellInfoIDNotFoundError */  "CellInfoID not found.",
	/* mapTextureIDRedefinedError */  "TextureID redefined.",
	/* mapTextureIDNotFoundError */   "TextureID not found.",

	/* mapFormatWrongError */ "Wrong format.",

	/* mapColorCellInfoCannotBeMirror */ "Color cells info cannot be mirror."
};

void TMap_GetErrorStr(enum EnumMapError* error, union TMapErrorData* errorData, char* errorStr)
{
	if (mapCArrayError == *error) 
	{
		strcpy(errorStr, errorsStrs[(size_t)mapCArrayError]);
		strcpy(errorStr, " ");
		CArray_GetErrorStr(errorData->carrayData.error, errorStr);
	}
	else if (mapPFPError == *error)
	{
		strcpy(errorStr, errorsStrs[(size_t)mapPFPError]);
		strcpy(errorStr, " ");
		TPFP_GetErrorStr(errorData->propertiesFileParser.error, &errorData->propertiesFileParser.errorData, errorStr);
	}
	else if (mapCellInfoIDRedefinedError == *error ||
		     mapCellInfoIDNotFoundError == *error ||
		     mapTextureIDRedefinedError == *error ||
		     mapTextureIDNotFoundError == *error)
	{
		strcpy(errorStr, errorsStrs[(size_t)*error]);
		strcpy(errorStr, " ");

		char offset_str[128] = {'\0'};
		sprintf(offset_str, "Offset %zd.", errorData->mapData.offset);
		strcpy(errorStr, offset_str);
	}
	else if (mapFormatWrongError == *error)
	{
		strcpy(errorStr, errorsStrs[(size_t)mapFormatWrongError]);
		strcpy(errorStr, " ");

		size_t expected_errors_count = sizeof(errorsStrs) / sizeof(const char*);
		if ((size_t)errorData->mapData.expectedError < expected_errors_count)
		{
			if (mapExpectedArrayOfSize == errorData->mapData.expectedError ||
				mapExpectedArrayOfSizeMultipleOf == errorData->mapData.expectedError)
			{
				strcpy(errorStr, errorsStrs[(size_t)errorData->mapData.expectedError]);
				strcpy(errorStr, " ");

				char size_str[128] = { '\0' };
				sprintf(size_str, "%zd", errorData->mapData.arraySize);
				strcpy(errorStr, size_str);
				strcpy(errorStr, " ");

				char offset_str[128] = { '\0' };
				sprintf(offset_str, "Offset %zd.", errorData->mapData.offset);
				strcpy(errorStr, offset_str);
			}
			else
			{
				strcpy(errorStr, errorsStrs[(size_t)errorData->mapData.expectedError]);
			}
		}
		else
		{
			strcpy(errorStr, expectedErrorNotFoundStr);
		}
	}
	else
	{
		size_t errors_count = sizeof(errorsStrs) / sizeof(const char*);
		if ((size_t)*error < errors_count)
		{
			strcpy(errorStr, errorsStrs[(size_t)*error]);
		}
		else
		{
			strcpy(errorStr, errorNotFoundStr);
		}
	}
}

size_t TMap_GetErrorStrSize(enum EnumMapError* error, union TMapErrorData* errorData)
{
	size_t ret_size = 0;

	if (mapCArrayError == *error)
	{
		ret_size = 0;
		ret_size += strlen(errorsStrs[(size_t)mapCArrayError]);
		ret_size += strlen(" ");
		ret_size += CArray_GetErrorStrSize(errorData->carrayData.error);
	}
	else if (mapPFPError == *error)
	{
		ret_size = 0;
		ret_size += strlen(errorsStrs[(size_t)mapPFPError]);
		ret_size += strlen(" ");
		ret_size += TPFP_GetErrorStrSize(errorData->propertiesFileParser.error, &errorData->propertiesFileParser.errorData);
	}
	else if (mapCellInfoIDRedefinedError == *error ||
		mapCellInfoIDNotFoundError == *error ||
		mapTextureIDRedefinedError == *error ||
		mapTextureIDNotFoundError == *error)
	{
		ret_size = 0;
		ret_size += strlen(errorsStrs[(size_t)*error]);
		ret_size += strlen(" ");

		char offset_str[128] = { '\0' };
		sprintf(offset_str, "Offset %zd.", errorData->mapData.offset);
		ret_size += strlen(offset_str);
		ret_size += 1;
	}
	else if (mapFormatWrongError == *error)
	{
		ret_size = 0;
		ret_size += strlen(errorsStrs[(size_t)mapFormatWrongError]);
		ret_size += strlen(" ");

		size_t expected_errors_count = sizeof(errorsStrs) / sizeof(const char*);
		if ((size_t)errorData->mapData.expectedError < expected_errors_count)
		{
			if (mapExpectedArrayOfSize == errorData->mapData.expectedError ||
				mapExpectedArrayOfSizeMultipleOf == errorData->mapData.expectedError)
			{
				ret_size += strlen(errorsStrs[(size_t)errorData->mapData.expectedError]);
				ret_size += strlen(" ");

				char size_str[128] = { '\0' };
				sprintf(size_str, "%zd", errorData->mapData.arraySize);
				ret_size += strlen(size_str);
				ret_size += strlen(" ");

				char offset_str[128] = { '\0' };
				sprintf(offset_str, "Offset %zd.", errorData->mapData.offset);
				ret_size += strlen(offset_str);
			}
			else
			{
				ret_size += strlen(errorsStrs[(size_t)errorData->mapData.expectedError]);
			}
		}
		else
		{
			ret_size += strlen(expectedErrorNotFoundStr);
		}

		ret_size += 1;
	}
	else
	{
		ret_size = 0;
		size_t errors_count = sizeof(errorsStrs) / sizeof(const char*);
		if ((size_t)*error < errors_count)
		{
			ret_size += strlen(errorsStrs[(size_t)*error]);
		}
		else
		{
			ret_size += strlen(errorNotFoundStr);
		}
		ret_size += 1;
	}

	return ret_size;
}