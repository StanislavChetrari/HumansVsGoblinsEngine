`Humans Vs Goblins Engine` is a preudo-3D game engine what uses ray casting. It will be designed for usage in RPG games. It is inspired by `The Quest` (2006) by Redshift.

## Features

* Fast ray - tiled map intersection
* Rendering of color and texture walls
* Transparent rextures
* 2 draw modes: 2.5D and 2D
* Collision detection
* Simple ambient light and simple diffuse light from the player
* Parser for file format called `PropertiesFileFormat`. For more details `Docs/PropertiesFileFormat`
* Maps in file format `PropertiesFileFormat` with support for color tiles and texture tiles. For more details `Docs/Map`
* Load png images for textures
* CArray object to help with arrays manipulation
* An Object Model. For more details `Docs/ObjectModel`

# Dependencies

## SDL2

SDL2 is used to create window, handle input and drawing in the window.

* `SDL2_INCLUDE_DIR:PATH` (mandatory) - the path to include directory.
* `SDL2_LIB_PATH:PATH` (mandatory) - the path to lib file.
* `SDL2_MAIN_LIB_PATH:PATH` (mandatory) - Windows only! the path to `SDL2main.lib`.
* `SDL2_WIN_DLL_PATH:PATH` (optional) - Windows only! the path to `SDL2.dll` that will be copied to the build directory.

## libpng

* `LIBPNG_INCLUDE_DIR:PATH` (mandatory) - the path to include directory.
* `LIBPNG_LIB_PATH:PATH` (mandatory) - the path to lib file.
* `LIBPNG_WIN_DLL_PATH:PATH` (optional) - Windows only! the path to `libpng.dll` that will be copied to the build directory.
* `LIBPNG_ZLIP_WIN_DLL_PATH:PATH` (optional) - Windows only! the path to `zlib.dll` that will be copied to the build directory. Usually needed by libpng.

## acutest

* `ACUTEST_INCLUDE_DIR:PATH`(optional) the path to include directory (see `Testing` section)

# Testing
`GENERATE_TESTS:BOOL` Default `ON`. If to generate tests in the build.

If tests are generated you also need `acutest` library. 

	https://github.com/mity/acutest

And specify the include directory path in `ACUTEST_INCLUDE_DIR`

acutest is only needed if you build tests.

To run tests go to build directory and run:

	ctest -C Debug

This will test Debug build.

# Shared lib

To build the engine as a shared library define variable `BUILD_HUMANS_VS_GOBLINS_ENGINE_SHARED:BOOL` ON.

# Examples

To see the engine in action see repository `HumansVsGoblinsDemo`.

# License

This project is licensed under Zlib license, see the file `LICENSE.txt`.