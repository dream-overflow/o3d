#----------------------------------------------------------
# build
#----------------------------------------------------------

file(GLOB_RECURSE CORE_CXX *.cpp core)
file(GLOB_RECURSE GEOM_CXX *.cpp geom)
file(GLOB_RECURSE PHYSIC_CXX *.cpp physic)
file(GLOB_RECURSE IMAGE_CXX *.cpp image)
file(GLOB_RECURSE ENGINE_CXX *.cpp engine)
file(GLOB_RECURSE GUI_CXX *.cpp gui)
file(GLOB_RECURSE AUDIO_CXX *.cpp audio)

# @todo should be used as extern lib
set(tinyxml
	../third/tinyxml/tinystr.cpp
	../third/tinyxml/tinyxml.cpp
	../third/tinyxml/tinyxmlerror.cpp
	../third/tinyxml/tinyxmlparser.cpp)

# @todo should be build as a lib in third and then linked
set(tristripper
	../third/TriStripper/Src/connectivity_graph.cpp
	../third/TriStripper/Src/policy.cpp
	../third/TriStripper/Src/tri_stripper.cpp)

# @todo should be build as a lib in third and then linked
set(nvtristrip
	../third/nvTriStrip/NvTriStrip.cpp
	../third/nvTriStrip/NvTriStripObjects.cpp)

# @todo should be used as extern lib
set(uuid
	../third/uuid/sysdep.c
    ../third/uuid/uuid.c
    ../third/uuid/md5c.c)

link_directories(${SDL2_LIBDIR})

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(O3D_LIB_NAME objective3d-dbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	set(O3D_LIB_NAME objective3d-odbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
	set(O3D_LIB_NAME objective3d)
endif()

add_library(${O3D_LIB_NAME} SHARED
			${tinyxml}
			${tristripper}
			${nvtristrip}
			${uuid}
			${CORE_CXX}
			${GEOM_CXX}
			${PHYSIC_CXX}
			${IMAGE_CXX}
			${ENGINE_CXX}
			${GUI_CXX}
			${AUDIO_CXX})		

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(${O3D_LIB_NAME}
			pthread
			${ZLIB_LIBRARY}
			${JPEG_LIBRARY}
			rt
			${XRANDR_LIBRARY}
			${PNG_LIBRARY}
			${OGG_LIBRARY}
			${VORBIS_LIBRARY}
			${VORBISFILE_LIBRARY}
			${OPENAL_LIBRARY}
			${SDL2_LIBRARIES}
			${FREETYPE_LIBRARIES})
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	target_link_libraries(${O3D_LIB_NAME}
			pthread
			z
			jpeg
			rt
			Xrandr	
			png
			vorbis
			vorbisfile
			openal
			${SDL2_LIBRARIES}
			${FREETYPE_LIBRARIES})
ELSEIF(MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(${O3D_LIB_NAME}
			${PNG_LIBRARY}
			${ZLIB_LIBRARY}
			${JPEG_LIBRARY}
			${OGG_LIBRARY}
			${VORBIS_LIBRARY}
			${VORBISFILE_LIBRARY}
			${OPENAL_LIBRARY}
	#		${SDL2_LIBRARIES}
			${FREETYPE_LIBRARIES}
			${OPENGL_LIBRARY})
ELSEIF(NOT MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	#	target_link_libraries({O3D_LIB_NAME}
	#		${OPENGL_gl_LIBRARY}
	#		zlib
	#		libjpeg
	#		libpng
	#		libogg
	#		libvorbis
	#		libvorbisfile
	#		freetype235MT
	#		OpenAL32)
ENDIF()


# build shaders
add_custom_target(copy_shaders 
	COMMAND	${CMAKE_COMMAND} -E copy_directory
			"${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    		"${CMAKE_CURRENT_BINARY_DIR}/shaders/")

add_custom_target(zip_shaders
	COMMAND	zip -r shaders.zip
    	"shaders")

#execute_process(
#    COMMAND
#        "zip -r shaders.zip"
#        	"${CMAKE_CURRENT_SOURCE_DIR}/shaders" 
#    RESULT_VARIABLE _result
#)

# Install

install (TARGETS ${O3D_LIB_NAME}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT library)
