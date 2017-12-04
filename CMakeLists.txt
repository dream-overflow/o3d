cmake_minimum_required (VERSION 2.8)
project (O3D)

set (O3D_VERSION_MAJOR 1)
set (O3D_VERSION_MINOR 3)
set (O3D_VERSION_BETA 1)
set (O3D_VERSION 0x0130)
set (O3D_VERSION_FILE_MIN 0x0120)

set (DREAMOVERFLOW_CMAKE_MODULES $ENV{DREAMOVERFLOW_CMAKE_MODULES})
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${DREAMOVERFLOW_CMAKE_MODULES})

include(dreamoverflow)

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
# options
#----------------------------------------------------------

# SSE2 ON by default
option(O3D_USE_SSE2 "Compile using x86_64 SSE2 optimizations" ON)

# SDL2 OFF by default
option(O3D_USE_SDL2 "Compile using SDL2 support" OFF)

# EGL2 OFF by default
option(O3D_USE_EGL "Compile using EGL support" OFF)

# WAYLAND OFF by default
option(O3D_USE_WAYLAND "Compile using WAYLAND support" OFF)

# VULKAN OFF by default
option(O3D_USE_VULKAN "Compile using VULKAN support" OFF)

#----------------------------------------------------------
# externals libraries
#----------------------------------------------------------

set (CMAKE_FIND_ROOT_PATH ${CMAKE_FIND_ROOT_PATH} $ENV{PREFIX})
# set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_ROOT}/Modules)
# message(${CMAKE_MODULE_PATH})

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_PREFIX_PATH $ENV{PREFIX} ${CMAKE_PREFIX_PATH})
    set(CMAKE_INCLUDE_PATH $ENV{PREFIX}/include ${CMAKE_INCLUDE_PATH})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

find_package(OpenGL REQUIRED)
find_package(OpenAL REQUIRED)
find_package(Freetype REQUIRED)
find_package(JPEG REQUIRED)
find_package(PNG REQUIRED)
find_package(OggVorbis REQUIRED)
#find_package(Bullet REQUIRED)

if(${O3D_USE_SDL2})
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 sdl2)
endif()

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Threads REQUIRED)
    find_package(X11 REQUIRED)
    find_package(Xrandr REQUIRED)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    find_package(Threads REQUIRED)
    find_package(X11 REQUIRED)
    find_package(Xrandr REQUIRED)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    IF(${TOOLCHAIN_PREFIX} MATCHES "x86_64-w64-mingw32")
    ELSE()
        # @todo upgrade more cmake like
        #set (O3D_DEPS_PATH $ENV{O3D_DEPS_PATH})
        #set (OPENAL_PATH $ENV{OPENAL_PATH})

        #include_directories("${O3D_DEPS_PATH}")
        #include_directories("${O3D_DEPS_PATH}/freetype2/include")
        #include_directories("${OPENAL_PATH}/include")
        #link_directories("${O3D_DEPS_PATH}/lib")
        #link_directories("${OPENAL_PATH}/libs/Win32")
        #link_directories("${OPENAL_PATH}/lib")
    ENDIF()

    include_directories(${FREETYPE_INCLUDE_DIRS})
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

include_directories(${FREETYPE_INCLUDE_DIR_ft2build})
include_directories(${FREETYPE_INCLUDE_DIR_freetype2})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

#----------------------------------------------------------
# settings
#----------------------------------------------------------

# Dynamic library
set(O3D_EXPORT_DLL "") 

# Library base path
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(LIB_PATH $ENV{LIB_PATH})
else()
    set(LIB_PATH $ENV{PREFIX})
endif()

# Library extension depending of the build mode
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(LIB_EXT "-dbg")
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    set(LIB_EXT "-odbg")
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(LIB_EXT "")
endif()

# Linux GCC cflags
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter -std=c++14 -fexceptions -Wextra -DO3D_X11 -DO3D_EXPORT_DLL")
    if (O3D_USE_SSE2)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    endif (O3D_USE_SSE2)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Darwin GCC cflags
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter -std=c++14 -fexceptions -Wextra -DO3D_X11 -DO3D_EXPORT_DLL")
        if (O3D_USE_SSE2)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    endif (O3D_USE_SSE2)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

# Windows VC++ or GCC cflags
IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # todo SSE2 support On
    option(O3D_USE_SSE2 "Compile SSE2 optimizations" OFF)
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

#----------------------------------------------------------
# configuration target
#----------------------------------------------------------

# Configuration header
configure_file (
	"include/o3d/objective3dconfig.h.in"
    "${CMAKE_BINARY_DIR}/include/objective3dconfig.h"
)

include_directories(${CMAKE_BINARY_DIR}/include)

# add a cmake configuration file
configure_file(${PROJECT_SOURCE_DIR}/cmake/Configs/Objective3DConfig.cmake.in
  "${PROJECT_BINARY_DIR}/lib/Objective3DConfig.cmake" @ONLY)

#----------------------------------------------------------
# sub-directories
#----------------------------------------------------------

add_subdirectory(third)
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

# objective3dconfig.h and cmake config
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    install (FILES ${CMAKE_BINARY_DIR}/include/objective3dconfig.h DESTINATION lib/objective3d-dbg)
    install (FILES ${CMAKE_BINARY_DIR}/lib/Objective3DConfig.cmake DESTINATION lib/objective3d-dbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
    install (FILES ${CMAKE_BINARY_DIR}/include/objective3dconfig.h DESTINATION lib/objective3d-odbg)
    install (FILES ${CMAKE_BINARY_DIR}/lib/Objective3DConfig.cmake DESTINATION lib/objective3d-odbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
    install (FILES ${CMAKE_BINARY_DIR}/include/objective3dconfig.h DESTINATION lib/objective3d)
    install (FILES ${CMAKE_BINARY_DIR}/lib/Objective3DConfig.cmake DESTINATION lib/objective3d)
endif()

# Cmake module
install (FILES ${PROJECT_SOURCE_DIR}/cmake/Modules/FindObjective3D.cmake DESTINATION share/cmake/Modules)

# shaders.zip
install (FILES ${CMAKE_BINARY_DIR}/src/shaders.zip DESTINATION share/o3d)

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
