# Map

## Description

Maps are in format `PropertiesFileFormat`.

The following properties are mandatory:

* `title` : (string) the title of the map
* `playerStartPos` : (array of 2 floats) the x, y start position of the player
* `size` : (array of 2 ints) the width and height of the map in cells
* `data` : (array of ints) CellInfoIDs for each cell. Each cell has 6 faces. They are packed (Top, Bottom, Left, Right, Ceiling, Floor). Its size must be size[0] * size[1] * 6 which means map width * map height * six faces of a cell
* `cellsInfoColor` : (array of ints) CellInfoID followed by R, G, B values of the color. The size of the array must be multiple of 4
* `cellsInfoTexture` : (array of ints) CellInfoID followed by TextureID. The size of the array must be multiple of 2
* `textures` : (array ints and strings) TextureID followed by a string texture path. The size of the aray must be multiple of 2. Textures must be path relative to map file
* `playerHasLight` : (0-false, 1-true) If the player should have light arround
* `playerLightDiffuseStrength` : (float) The range of the light
* `playerLightDiffusePower` : (float) The intensity of the light
* `playerLightDiffuseColor` : (array of 3 ints) The color of light of the player
* `hasAmbientLight` : (0-false, 1-true) If there is ambient light
* `ambientLightStrength` : (float) The intensity of the ambient light
* `ambientLightColor` : (array of 3 ints) The color of ambient light
* `environmentIsTexture` : (0-false, 1-true) If the environment is texture of color
* `environmentColor` : (array of 3 ints) The color of the environment
* `environmentTextureId` : (int) The TextureID of the environment (Not implemented yet!)
* `cellsInfoThatAreMirror` : (array of ints) The CellInfoIDs of cells info that are mirror. Color cells info cannot be mirror (Not implemented yet!)

## Example 

	title="map1";

	playerStartPos=[1.5, 4.5]; // x, y

	size =[20, 10]; // number of cells on x, y

	// Cell format is: 
	// Top, Bottom, Left, Right, Ceiling, Floor
	data=
	[
		1, 2, 6, 6, 0, 0,   2, 2, 2, 2, 0, 0,   1, 1, 1, 1, 0, 0,   6, 6, 6, 6, 0, 0,   1, 1, 1, 1, 0, 0, 
		2, 2, 2, 2, 4, 5,   0, 0, 0, 0, 4, 5,   0, 0, 0, 0, 4, 5,   0, 0, 0, 0, 4, 5,   1, 1, 1, 1, 4, 5, 
		6, 6, 6, 6, 4, 5,   0, 0, 0, 0, 4, 5,   0, 0, 0, 0, 4, 5,   0, 0, 0, 0, 4, 5,   2, 2, 2, 2, 4, 5, 
		1, 1, 1, 1, 4, 5,   0, 0, 0, 0, 0, 5,   0, 0, 0, 0, 2, 1,   0, 0, 0, 0, 4, 5,   1, 1, 1, 1, 4, 5, 
		2, 2, 2, 2, 4, 5,   0, 0, 0, 0, 4, 5,   0, 0, 0, 0, 4, 5,   0, 0, 0, 0, 4, 5,   1, 1, 1, 1, 4, 5, 
		6, 6, 6, 6, 4, 5,   0, 0, 0, 0, 4, 5,   0, 0, 0, 0, 0, 5,   0, 0, 0, 0, 4, 5,   1, 1, 1, 1, 4, 5, 
		1, 1, 1, 1, 0, 0,   0, 3, 3, 3, 4, 5,   2, 2, 2, 2, 4, 5,   5, 5, 5, 5, 4, 5,   1, 1, 1, 1, 0, 0
	];

	cellsInfoColor=
	[ /* CellInfoID   R    G    B */
		1,           255, 0,   0,
		2,           0,   255, 0
	];
	cellsInfoTexture=
	[ /* CellInfoID   TextureID */
		3,           1,
		4,           3,
		5,           2
	];

	cellsInfoThatAreMirror=
	[  /* CellInfoID */
		
	];

	textures=
	[ /* TextureID   FilePath */
		1,          "map1_textures/aerial_rocks_04_diff_1k.png",
		2,          "map1_textures/brick_floor_003_diffuse_1k.png",
		3,          "map1_textures/brick_wall_001_diffuse_1k.png"
	];

	playerHasLight             = 0;
	playerLightDiffuseStrength = 2.0;
	playerLightDiffusePower    = 3.0;
	playerLightDiffuseColor    = [255, 255, 255];

	hasAmbientLight      = 1;
	ambientLightStrength = 1.0;
	ambientLightColor    = [255, 255, 255];

	environmentIsTexture = 0;
	environmentColor     = [102, 127, 153];
	environmentTextureId = 0;