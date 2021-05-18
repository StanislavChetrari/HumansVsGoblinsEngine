#ifndef _MAP_H_
#define _MAP_H_

#include <stdint.h>

#include <EngineAPI.h>

#include <MathUtils.h>
#include <PropertiesFileParser.h>
#include <CArray.h>

struct TTexture;

enum EnumMapError
{
	mapNoError = 0,

	mapCouldNotOpenFileError     = 1,
	mapCouldNotReadFromFileError = 2,
	mapCouldNotLoadTexture       = 3,
	mapMallocError               = 4,

	mapCArrayError = 5,
	mapPFPError    = 6,

	mapTexturesPropertyNotFoundError         = 7,
	mapCellsInfoTexturePropertyNotFoundError = 8,
	mapCellsInfoColorPropertyNotFoundError   = 9,
	mapDataPropertyNotFoundError             = 10,
	
	mapSizePropertyNotFoundError      = 11,
	mapPlayerStartPosPropertyNotFound = 12,
	mapTitlePropertyNotFound          = 13,

	mapCellInfoIDRedefinedError = 14,
	mapCellInfoIDNotFoundError  = 15,
	mapTextureIDRedefinedError  = 16,
	mapTextureIDNotFoundError   = 17,

	mapFormatWrongError = 18,

	mapColorCellInfoCannotBeMirror = 19
};

enum EnumMapFormatExpectedError
{
	mapExpectedUndefined = 0,

	mapExpectedString                = 1,
	mapExpectedArray                 = 2,
	mapExpectedArrayOfSize           = 3,
	mapExpectedArrayOfSizeMultipleOf = 4,
	mapExpectedInt                   = 5,
	mapExpectedFloat                 = 6
};

union TMapErrorData
{
	struct
	{
		enum EnumPFPError   error;
		union TPFPErrorData errorData;
	} propertiesFileParser;
	struct
	{
		enum EnumMapFormatExpectedError expectedError;
		size_t                          arraySize;
		size_t                          offset;
	} mapData;
	struct
	{
		enum EnumCArrayError error;
	} carrayData;
};

enum EnumCellType
{
	cellUndefined         = 0,
	cellColor             = 1,
	cellTexture           = 2,
	cellMirror            = 3,
	cellMirrorWithTexture = 4
};

struct TCellInfo
{
	enum EnumCellType type;

	uint8_t colour[3];
	// tectureIndices reffering to textures array.
	size_t  textureIndex;
};

struct TCell
{
	int top, bottom;
	int left, right;
	int ceiling, floor;
};

struct TMap
{
	char*         title;
	int           numCellsX, numCellsY;
	struct Vec2f  playerStartPos;
	struct TCell* data;

	struct TCellInfo* cellsInfo;
	size_t            cellsInfoCount;

	struct TTexture* textures;
	size_t           texturesCount;

	int     playerHasLight;
	float   playerLightDiffuseStrength;
	float   playerLightDiffusePower;
	uint8_t playerLightDiffuseColor[3];

	int     hasAmbientLight;
	float   ambientLightStrength;
	uint8_t ambientLightColor[3];

	int     environmentIsTexture;
	uint8_t environmentColor[3];
	int     environmentHasTexture;
	size_t  environmentTextureIndex;
};

/*
	Return: 
		mapNoError 
		mapCouldNotOpenFileError     
		mapCouldNotReadFromFileError 
		mapCouldNotLoadTexture       
		mapMallocError              
		mapCArrayError 
		mapPFPError    
		mapTexturesPropertyNotFoundError         
		mapCellsInfoTexturePropertyNotFoundError 
		mapCellsInfoColorPropertyNotFoundError  
		mapDataPropertyNotFoundError            
		mapSizePropertyNotFoundError      
		mapPlayerStartPosPropertyNotFound 
		mapTitlePropertyNotFound         
		mapCellInfoIDRedefinedError    
		mapCellInfoIDNotFoundError 
		mapTextureIDRedefinedError
		mapTextureIDNotFoundError
		mapFormatWrongError
		mapColorCellInfoCannotBeMirror
*/
DECL_API enum EnumMapError TMap_LoadFromFile(struct TMap* map, const char* filePath, union TMapErrorData* errorData);

DECL_API void TMap_PrintScreen(const struct TMap* map);

DECL_API void TMap_InitZero(void* map);
DECL_API void TMap_Clear(void* map);

/*
	Appends to errorStr the string text that coresponds to error.
	Make shure there is enough space left!
	If errorStr is NULL the function will not do anything.
*/
DECL_API void TMap_GetErrorStr(enum EnumMapError* error, union TMapErrorData* errorData, char* errorStr);

/*
	Returns size in bytes that is needed to store error string with TMap_GetErrorStr(). It includes NULL terminator.
*/
DECL_API size_t TMap_GetErrorStrSize(enum EnumMapError* error, union TMapErrorData* errorData);

#endif /* #ifndef _MAP_H_ */