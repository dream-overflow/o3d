# - Try to find the Objective3D libraries
# Once done this will define
#
#  OBJECTIVE3D_FOUND - System has Objective3D
#  OBJECTIVE3D_INCLUDE_DIR - The Objective3D include directory
#  OBJECTIVE3D_INCLUDE_FILE_objective3dconfig - The Objective3D config header
#  OBJECTIVE3D_INCLUDE_DIR_objective3dconfig - The Objective3D config header include directory
#  OBJECTIVE3D_LIBRARY - The Objective3D library
#  OBJECTIVE3D_SHADERS_ZIP - The Objective3D shaders.zip file

# Copyright (c) 2017, DreamOverflow, <frederic.scherma@dreamoverflow.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (UNIX OR MINGW)
	include (CheckLibraryExists)

	# build mode set library name
	if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
		set(O3D_LIB_NAME "objective3d-dbg")
	elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
		set(O3D_LIB_NAME "objective3d-odbg")
	elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
		set(O3D_LIB_NAME "objective3d")
	endif()

	find_path(OBJECTIVE3D_INCLUDE_DIR o3d)
	find_file(OBJECTIVE3D_INCLUDE_FILE_objective3dconfig objective3dconfig.h HINTS ENV PREFIX PATH_SUFFIXES lib/${O3D_LIB_NAME})
	find_library(OBJECTIVE3D_LIBRARY NAMES ${O3D_LIB_NAME})

	get_filename_component(OBJECTIVE3D_INCLUDE_DIR_objective3dconfig ${OBJECTIVE3D_INCLUDE_FILE_objective3dconfig} DIRECTORY)
	get_filename_component(OBJECTIVE3D_LIBRARY_DIR ${OBJECTIVE3D_LIBRARY} DIRECTORY)

	# include the package build config
	include(${OBJECTIVE3D_LIBRARY_DIR}/${O3D_LIB_NAME}/Objective3DConfig.cmake)	

    # shaders
    file(GLOB OBJECTIVE3D_SHADERS_ZIP "$ENV{PREFIX}/share/o3d/glsl*.zip")
    message(${OBJECTIVE3D_SHADERS_ZIP})
    # find_file(OBJECTIVE3D_SHADERS_ZIP *.zip HINTS ENV PREFIX PATH_SUFFIXES share/o3d)

	mark_as_advanced(OBJECTIVE3D_INCLUDE_DIR OBJECTIVE3D_LIBRARY)

	if (OBJECTIVE3D_INCLUDE_DIR AND OBJECTIVE3D_LIBRARY AND OBJECTIVE3D_INCLUDE_FILE_objective3dconfig AND OBJECTIVE3D_SHADERS_ZIP)
		set(OBJECTIVE3D_FOUND TRUE)
	else ()
		set(OBJECTIVE3D_FOUND FALSE)
	endif ()

	if (OBJECTIVE3D_FOUND)
		if (NOT objective3d_FIND_QUIETLY)
      		message(STATUS "Found Objective3D: ${OBJECTIVE3D_LIBRARY}")
		endif (NOT objective3d_FIND_QUIETLY)
	else (OBJECTIVE3D_FOUND)
		if (objective3d_FIND_REQUIRED)
			message("*****************************************************")
			message_color(${TextError} "Error : Objective3D library not found")
			message("*****************************************************")
			message(FATAL_ERROR "Could NOT find Objective3D library")
		endif (objective3d_FIND_REQUIRED)
		if (NOT objective3d_FIND_QUITELY)
			message_color(${TextWarning} "Could NOT find Objective3D library")
		endif (NOT objective3d_FIND_QUITELY)
	endif (OBJECTIVE3D_FOUND)
endif (UNIX OR MINGW)
