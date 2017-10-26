cmake_minimum_required (VERSION 2.6)
project (O3D)

set (O3D_VERSION_MAJOR 1)
set (O3D_VERSION_MINOR 3)
set (O3D_VERSION_BETA 1)
set (O3D_VERSION 0x0130)
set (O3D_VERSION_FILE_MIN 0x0120)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/third")

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
   message(STATUS "Setting build type to 'Debug' as none was specified.")
   set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
   # Set the possible values of build type for cmake-gui
   set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "DebugFull" "Release" "RelWithDebInfo")
endif()

if(NOT MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/O2 /Zi /D_DEBUG /EHsc /MP /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE        "/O2 /DNDEBUG /EHsc /MP /MD")
    set(CMAKE_CXX_FLAGS_DEBUG          "/D_DEBUG /Zi /EHsc /MP /MDd")
    set(CMAKE_CXX_FLAGS_DEBUGFULL      "/D_DEBUG /Zi /EHsc /MP /MDd")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO   "/O2 /Zi /D_DEBUG /EHsc /MP /MD")
    set(CMAKE_C_FLAGS_RELEASE          "/O2 /DNDEBUG /EHsc /MP /MD")
    set(CMAKE_C_FLAGS_DEBUG            "/D_DEBUG /Zi /EHsc /MP /MDd")
    set(CMAKE_C_FLAGS_DEBUGFULL        "/D_DEBUG /Zi /EHsc /MP /MDd -DO3D_RAM_MEMORY_MANAGER -DO3D_FAST_MEMORY_MANAGER")
else()
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -ggdb -D_DEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG          "-O0 -ggdb -D_DEBUG")
	set(CMAKE_CXX_FLAGS_DEBUGFULL      "-O0 -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_RELWITHDEBINFO   "-O2 -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_RELEASE          "-O2 -DNDEBUG")
	set(CMAKE_C_FLAGS_DEBUG            "-O0 -ggdb -D_DEBUG")
	set(CMAKE_C_FLAGS_DEBUGFULL        "-O0 -ggdb -D_DEBUG -DO3D_RAM_MEMORY_MANAGER -DO3D_FAST_MEMORY_MANAGER")
endif()

#----------------------------------------------------------
# externals libraries
#----------------------------------------------------------

include(FindOpenGL)
#include(Bullet)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	include(FindOpenAL)
	include(FindFreetype)
	include(FindJPEG)
	include(FindPNG)
	include(FindX11)
	include(FindPkgConfig)
	pkg_check_modules(SDL2 sdl2)
	
	include_directories(${FREETYPE_INCLUDE_DIR_ft2build})
	include_directories(${FREETYPE_INCLUDE_DIR_freetype2})	
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	include(FindX11)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	include(FindPkgConfig)
	pkg_check_modules(FREETYPE freetype2)

	#set (O3D_DEPS_PATH $ENV{O3D_DEPS_PATH})
	#set (OPENAL_PATH $ENV{OPENAL_PATH})

	#include_directories("${O3D_DEPS_PATH}")
	#include_directories("${O3D_DEPS_PATH}/freetype2/include")
	#include_directories("${OPENAL_PATH}/include")
	#link_directories("${O3D_DEPS_PATH}/lib")
	#link_directories("${OPENAL_PATH}/libs/Win32")
	#link_directories("${OPENAL_PATH}/lib")

	#	set (O3D_DEPS_INCLUDE /home/frederic/dev/mw32/include)
	#	set (O3D_DEPS_LIB /home/frederic/dev/mw32/lib)

	include_directories(${FREETYPE_INCLUDE_DIRS})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

#----------------------------------------------------------
# options
#----------------------------------------------------------

option(O3D_USE_SSE2 "Compile SSE2 optimizations" ON)

# Dynamic library
set(O3D_EXPORT_DLL "") 

# objective3dconfig.h
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if(MINGW)
		set(LIB_PATH $ENV{CROSSROAD_PREFIX})
	else()
		set(LIB_PATH $ENV{LIB_PATH})
	endif()
else()
	set(LIB_PATH $ENV{PREFIX})
endif()

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	include_directories(${LIB_PATH}/lib/objective3d-dbg)
	set(LIB_EXT "-dbg")
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	include_directories(${LIB_PATH}/lib/objective3d-odbg)
	set(LIB_EXT "-odbg")
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
	include_directories(${LIB_PATH}/lib/objective3d)
	set(LIB_EXT "")
endif()
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter -std=c++14 -fexceptions -Wextra -DO3D_X11 -DO3D_EXPORT_DLL")
	if (O3D_USE_SSE2)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
	endif (O3D_USE_SSE2)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter -std=c++14 -fexceptions -Wextra -DO3D_X11 -DO3D_EXPORT_DLL")
	if (O3D_USE_SSE2)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
	endif (O3D_USE_SSE2)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	option(O3D_USE_SSE2 "Compile SSE2 optimizations" OFF) # TODO
	if(MINGW)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter -std=c++14 -fexceptions -Wextra -DO3D_WIN32 -DO3D_EXPORT_DLL")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DO3D_EXPORT_DLL")
	endif()
    if(O3D_USE_SSE2)
		if(MINGW)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
		else()
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:SSE2")
		endif()
    endif (O3D_USE_SSE2)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

if (O3D_USE_SSE2)
	set(O3D_USE_SSE2 "1")
endif (O3D_USE_SSE2)

# configuration
configure_file (
	"include/o3d/objective3dconfig.h.in"
    "${CMAKE_BINARY_DIR}/include/objective3dconfig.h"
)

include_directories(${CMAKE_BINARY_DIR}/include)

#----------------------------------------------------------
# sub-directories
#----------------------------------------------------------

#add_subdirectory(third)
add_subdirectory(src)
add_subdirectory(test)

#----------------------------------------------------------
# install
#----------------------------------------------------------

file(GLOB CORE_HXX include/o3d/core/*.h)
file(GLOB GEOM_HXX include/o3d/geom/*.h)
file(GLOB IMAGE_HXX include/o3d/image/*.h)
file(GLOB AUDIO_HXX include/o3d/audio/*.h)
file(GLOB ENGINE_HXX include/o3d/engine/*.h)
file(GLOB ENGINE_ANIMATION_HXX include/o3d/engine/animation/*.h)
file(GLOB ENGINE_ANIMATION_HXX include/o3d/engine/animation/*.h)
file(GLOB ENGINE_DEFERRED_HXX include/o3d/engine/deferred/*.h)
file(GLOB ENGINE_EFFECT_HXX include/o3d/engine/effect/*.h)
file(GLOB ENGINE_HIERARCHY_HXX include/o3d/engine/hierarchy/*.h)
file(GLOB ENGINE_LANDSCAPE_HXX include/o3d/engine/landscape/*.h)
file(GLOB ENGINE_LANDSCAPE_HEIGHTMAP_HXX include/o3d/engine/landscape/heightmap/*.h)
file(GLOB ENGINE_LANDSCAPE_PCLOD_HXX include/o3d/engine/landscape/pclod/*.h)
file(GLOB ENGINE_MAP2D_HXX include/o3d/engine/map2d/*.h)
file(GLOB ENGINE_MATERIAL_HXX include/o3d/engine/material/*.h)
file(GLOB ENGINE_OBJECT_HXX include/o3d/engine/object/*.h)
file(GLOB ENGINE_SCENE_HXX include/o3d/engine/scene/*.h)
file(GLOB ENGINE_SHADER_HXX include/o3d/engine/shader/*.h)
file(GLOB ENGINE_SHADOW_HXX include/o3d/engine/shadow/*.h)
file(GLOB ENGINE_SKY_HXX include/o3d/engine/sky/*.h)
file(GLOB ENGINE_TEXTURE_HXX include/o3d/engine/texture/*.h)
file(GLOB ENGINE_UTILS_HXX include/o3d/engine/utils/*.h)
file(GLOB ENGINE_VISIBILIY_HXX include/o3d/engine/visibility/*.h)
file(GLOB ENGINE_SHADOW_HXX include/o3d/engine/shadow/*.h)
file(GLOB PHYSIC_HXX include/o3d/physic/*.h)
file(GLOB GUI_HXX include/o3d/gui/*.h)
file(GLOB GUI_WIDGETS_HXX include/o3d/gui/widgets/*.h)
file(GLOB TINYXML_HXX third/tinyxml/*.h)

# objective3dconfig.h and shaders.zip
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	install (FILES ${CMAKE_BINARY_DIR}/include/objective3dconfig.h DESTINATION lib/objective3d-dbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	install (FILES ${CMAKE_BINARY_DIR}/include/objective3dconfig.h DESTINATION lib/objective3d-odbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
	install (FILES ${CMAKE_BINARY_DIR}/include/objective3dconfig.h DESTINATION lib/objective3d)
endif()

# Config.h and shaders.zip
install (FILES src/shaders.zip DESTINATION share/o3d)

# Headers
install (FILES ${CORE_HXX} DESTINATION include/o3d/core)
install (FILES ${IMAGE_HXX} DESTINATION include/o3d/image)
install (FILES ${GEOM_HXX} DESTINATION include/o3d/geom)
install (FILES ${AUDIO_HXX} DESTINATION include/o3d/audio)
install (FILES ${ENGINE_HXX} DESTINATION include/o3d/engine)
install (FILES ${ENGINE_ANIMATION_HXX} DESTINATION include/o3d/engine/animation)
install (FILES ${ENGINE_DEFERRED_HXX} DESTINATION include/o3d/engine/deferred)
install (FILES ${ENGINE_EFFECT_HXX} DESTINATION include/o3d/engine/effect)
install (FILES ${ENGINE_HIERARCHY_HXX} DESTINATION include/o3d/engine/hierarchy)
install (FILES ${ENGINE_LANDSCAPE_HXX} DESTINATION include/o3d/engine/landscape)
install (FILES ${ENGINE_LANDSCAPE_HEIGHTMAP_HXX} DESTINATION include/o3d/engine/landscape/heightmap)
install (FILES ${ENGINE_LANDSCAPE_PCLOD_HXX} DESTINATION include/o3d/engine/landscape/pclod)
install (FILES ${ENGINE_MAP2D_HXX} DESTINATION include/o3d/engine/map2d)
install (FILES ${ENGINE_MATERIAL_HXX} DESTINATION include/o3d/engine/material)
install (FILES ${ENGINE_OBJECT_HXX} DESTINATION include/o3d/engine/object)
install (FILES ${ENGINE_SCENE_HXX} DESTINATION include/o3d/engine/scene)
install (FILES ${ENGINE_SHADER_HXX} DESTINATION include/o3d/engine/shader)
install (FILES ${ENGINE_SHADOW_HXX} DESTINATION include/o3d/engine/shadow)
install (FILES ${ENGINE_SKY_HXX} DESTINATION include/o3d/engine/sky)
install (FILES ${ENGINE_TEXTURE_HXX} DESTINATION include/o3d/engine/texture)
install (FILES ${ENGINE_UTILS_HXX} DESTINATION include/o3d/engine/utils)
install (FILES ${ENGINE_VISIBILIY_HXX} DESTINATION include/o3d/engine/visibility)
install (FILES ${PHYSIC_HXX} DESTINATION include/o3d/physic)
install (FILES ${GUI_HXX} DESTINATION include/o3d/gui)
install (FILES ${GUI_WIDGETS_HXX} DESTINATION include/o3d/gui/widgets)
install (FILES ${TINYXML_HXX} DESTINATION include/tinyxml)

