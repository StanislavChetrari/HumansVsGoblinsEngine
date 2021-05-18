
set(LIB_NAME HumansVsGoblinsEngine)

# This function adds a source/header file to the project. 
# SRCS_VAR   - is the variable where all source files are written
# PROJ_GROUP - group in the project. For example filters in Visual Studio or other IDE. This group folders must be separated by \\
# FILE_PATH  - file to be added
# Example call AddSrcFile(SRCS YLE\\Src Src/main.c)
function(AddFile SRCS_VAR PROJ_GROUP FILE_PATH)
	#message("source_group(${PROJ_GROUP} FILES ${FILE_PATH})")
	source_group(${PROJ_GROUP} FILES ${FILE_PATH})
	set(${SRCS_VAR} ${${SRCS_VAR}} ${FILE_PATH} PARENT_SCOPE)
endfunction()

set(LIB_SOURCE_FILES "")

#######################################################################
AddFile(LIB_SOURCE_FILES Src Src/Camera.h)
AddFile(LIB_SOURCE_FILES Src Src/Camera.c)

AddFile(LIB_SOURCE_FILES Src Src/CArray.h)
AddFile(LIB_SOURCE_FILES Src Src/CArray.c)

AddFile(LIB_SOURCE_FILES Src Src/FileSystem.h)
AddFile(LIB_SOURCE_FILES Src Src/FileSystem.c)

AddFile(LIB_SOURCE_FILES Src Src/Map.h)
AddFile(LIB_SOURCE_FILES Src Src/Map.c)

AddFile(LIB_SOURCE_FILES Src Src/MathUtils.h)
AddFile(LIB_SOURCE_FILES Src Src/MathUtils.c)

AddFile(LIB_SOURCE_FILES Src Src/Physics.h)
AddFile(LIB_SOURCE_FILES Src Src/Physics.c)

AddFile(LIB_SOURCE_FILES Src Src/PropertiesFileParser.h)
AddFile(LIB_SOURCE_FILES Src Src/PropertiesFileParser.c)

AddFile(LIB_SOURCE_FILES Src Src/RayCasting.h)
AddFile(LIB_SOURCE_FILES Src Src/RayCasting.c)

AddFile(LIB_SOURCE_FILES Src Src/RenderMap2_5D.h)
AddFile(LIB_SOURCE_FILES Src Src/RenderMap2_5D.c)

AddFile(LIB_SOURCE_FILES Src Src/RenderMap2D.h)
AddFile(LIB_SOURCE_FILES Src Src/RenderMap2D.c)

AddFile(LIB_SOURCE_FILES Src Src/Texture.h)
AddFile(LIB_SOURCE_FILES Src Src/Texture.c)

AddFile(LIB_SOURCE_FILES Src Src/Window.h)
AddFile(LIB_SOURCE_FILES Src Src/Window.c)

AddFile(LIB_SOURCE_FILES Src Src/EngineAPI.h)
#######################################################################

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	
		add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	
		# /MP (multi-processor compilation)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++14 /MP")
		
		# Set as startup project 
		set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT ${LIB_NAME})
		
	endif()
endif()

if(NOT DEFINED BUILD_HUMANS_VS_GOBLINS_ENGINE_SHARED)
	set(BUILD_HUMANS_VS_GOBLINS_ENGINE_SHARED OFF)
endif()
# In case CMake is forced all libs to add as SHARED
if(DEFINED BUILD_SHARED_LIBS)
	if(${BUILD_SHARED_LIBS})
	set(BUILD_HUMANS_VS_GOBLINS_ENGINE_SHARED ON)
	endif()
endif()

if(${BUILD_HUMANS_VS_GOBLINS_ENGINE_SHARED})
	add_definitions(-D_H_VS_G_EXPORT_API)
	add_definitions(-D_H_VS_G_SHARED_LIB)
	add_library(${LIB_NAME} SHARED ${LIB_SOURCE_FILES})
else()
	add_library(${LIB_NAME} ${LIB_SOURCE_FILES})
endif()

include_directories(${LIB_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/Src)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/Src)

# Link SDL2
	if(NOT DEFINED SDL2_INCLUDE_DIR)
		message(FATAL_ERROR "SDL2 include directory not specified (SDL2_INCLUDE_DIR)!")
	endif()
	if(NOT DEFINED SDL2_LIB_PATH)
		message(FATAL_ERROR "SDL2 lib path not specified (SDL2_LIB_PATH)!")
	endif()

	get_filename_component(SDL2_LIB_DIR ${SDL2_LIB_PATH} DIRECTORY)
	get_filename_component(SDL2_LIB_FILE ${SDL2_LIB_PATH} NAME)
	
	include_directories(${SDL2_INCLUDE_DIR})
	target_link_directories(${LIB_NAME} PUBLIC "${SDL2_LIB_DIR}")
	target_link_libraries(${LIB_NAME} PUBLIC ${SDL2_LIB_FILE})
	
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	
		if(NOT DEFINED SDL2_MAIN_LIB_PATH)
			message(FATAL_ERROR "SDL2 main lib not specified (SDL2_MAIN_LIB_PATH)!")
		endif()
	
		get_filename_component(SDL2_MAIN_LIB_DIR ${SDL2_MAIN_LIB_PATH} DIRECTORY)
		get_filename_component(SDL2_MAIN_LIB_FILE ${SDL2_MAIN_LIB_PATH} NAME)
	
		target_link_directories(${LIB_NAME} PUBLIC "${SDL2_MAIN_LIB_DIR}")
		target_link_libraries(${LIB_NAME} PUBLIC ${SDL2_MAIN_LIB_FILE})
	
		#if(DEFINED SDL2_WIN_DLL_PATH)
		#	configure_file("${SDL2_WIN_DLL_PATH}" ${PROJECT_BINARY_DIR} COPYONLY)
		#endif()
	endif()
# End link SDL2

# Link libpng
	if(NOT DEFINED LIBPNG_INCLUDE_DIR)
		message(FATAL_ERROR "libpng include directory not specified (LIBPNG_INCLUDE_DIR)!")
	endif()
	if(NOT DEFINED LIBPNG_LIB_PATH)
		message(FATAL_ERROR "libpng lib path not specified (LIBPNG_LIB_PATH)!")
	endif()
	
	get_filename_component(LIBPNG_LIB_DIR ${LIBPNG_LIB_PATH} DIRECTORY)
	get_filename_component(LIBPNG_LIB_FILE ${LIBPNG_LIB_PATH} NAME)
	
	include_directories(${LIBPNG_INCLUDE_DIR})
	target_link_directories(${LIB_NAME} PUBLIC "${LIBPNG_LIB_DIR}")
	target_link_libraries(${LIB_NAME} PUBLIC ${LIBPNG_LIB_FILE})
	
	if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
		#if(DEFINED LIBPNG_WIN_DLL_PATH)
		#	configure_file("${LIBPNG_WIN_DLL_PATH}" ${PROJECT_BINARY_DIR} COPYONLY)
		#endif()
		#if(DEFINED LIBPNG_ZLIP_WIN_DLL_PATH)
		#	configure_file("${LIBPNG_ZLIP_WIN_DLL_PATH}" ${PROJECT_BINARY_DIR} COPYONLY)
		#endif()
	endif()
# End link libpng

# Test generation
if(NOT DEFINED GENERATE_TESTS)
	set(GENERATE_TESTS ON)
endif()
if(${GENERATE_TESTS})
	if(NOT DEFINED ACUTEST_INCLUDE_DIR)
		message(FATAL_ERROR "Acutest include directory not specified (ACUTEST_INCLUDE_DIR)!")
	endif()

	include_directories(${ACUTEST_INCLUDE_DIR})
	
	enable_testing()
	add_subdirectory(Tests)
endif()