#----------------------------------------------------------
# build
#----------------------------------------------------------

if(${O3D_CORE_ONLY})

    #
    # Only core functionnalities without display, GL...
    #

    file(GLOB_RECURSE CORE_CXX core/*.cpp)

    # @todo should be used as extern lib
    set(tinyxml_cxx
        ../third/tinyxml/tinystr.cpp
        ../third/tinyxml/tinyxml.cpp
        ../third/tinyxml/tinyxmlerror.cpp
        ../third/tinyxml/tinyxmlparser.cpp)

    # android only
    if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
        set(ANDROID_LIBRARY "-llog -landroid -ldl")
    endif()

    if(${O3D_USE_SDL2})
        link_directories(${SDL2_LIBDIR})
    endif()

    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(O3D_LIB_NAME objective3d-dbg)
    elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
        set(O3D_LIB_NAME objective3d-odbg)
    elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
        set(O3D_LIB_NAME objective3d)
    endif()

    add_library(${O3D_LIB_NAME} SHARED
        ${tinyxml_cxx}
        ${CORE_CXX})

    if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
        target_link_libraries(${O3D_LIB_NAME}
            # ${tinyxml}
            ${ZLIB_LIBRARY}
            ${ANDROID_LIBRARY})

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        target_link_libraries(${O3D_LIB_NAME}
            pthread
            # ${tinyxml}
            ${ZLIB_LIBRARY}
            rt)

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        target_link_libraries(${O3D_LIB_NAME}
            # ${tinyxml}
            pthread
            z
            rt)

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

    elseif(MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        target_link_libraries(${O3D_LIB_NAME}
            # ${tinyxml}
            ${ZLIB_LIBRARY})

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

    elseif(NOT MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        # todo upgrade and more cmake like
        target_link_libraries({O3D_LIB_NAME}
            # ${tinyxml}
            zlib)

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

    endif()

else()

    #
    # Complete default build
    #

    file(GLOB_RECURSE CORE_CXX core/*.cpp)
    file(GLOB_RECURSE GEOM_CXX geom/*.cpp)
    file(GLOB_RECURSE PHYSIC_CXX physic/*.cpp)
    file(GLOB_RECURSE IMAGE_CXX image/*.cpp)
    file(GLOB_RECURSE ENGINE_CXX engine/*.cpp)
    file(GLOB_RECURSE GUI_CXX gui/*.cpp)
    file(GLOB_RECURSE AUDIO_CXX audio/*.cpp)

    # @todo should be used as extern lib
    set(tinyxml_cxx
        ../third/tinyxml/tinystr.cpp
        ../third/tinyxml/tinyxml.cpp
        ../third/tinyxml/tinyxmlerror.cpp
        ../third/tinyxml/tinyxmlparser.cpp)

    # @todo should be build as a lib in third and then linked
    set(tristripper_cxx
        ../third/TriStripper/Src/connectivity_graph.cpp
        ../third/TriStripper/Src/policy.cpp
        ../third/TriStripper/Src/tri_stripper.cpp)

    # @todo should be build as a lib in third and then linked
    set(nvtristrip_cxx
        ../third/nvTriStrip/NvTriStrip.cpp
        ../third/nvTriStrip/NvTriStripObjects.cpp)

    # android only
    if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
        set(ANDROID_LIBRARY "-llog -landroid -ldl -lEGL -lGLESv3")
    endif()

    if(${O3D_USE_SDL2})
        link_directories(${SDL2_LIBDIR})
    endif()

    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
        set(O3D_LIB_NAME objective3d-dbg)
    elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
        set(O3D_LIB_NAME objective3d-odbg)
    elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
        set(O3D_LIB_NAME objective3d)
    endif()

    if(${O3D_USE_EGL})
        set(EGL_LIBRARIES "-lEGL")
    endif()

    add_library(${O3D_LIB_NAME} SHARED
        ${tinyxml_cxx}
        ${tristripper_cxx}
        ${nvtristrip_cxx}
        ${CORE_CXX}
        ${GEOM_CXX}
        ${PHYSIC_CXX}
        ${IMAGE_CXX}
        ${ENGINE_CXX}
        ${GUI_CXX}
        ${AUDIO_CXX})

    if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
        target_link_libraries(${O3D_LIB_NAME}
            ${ZLIB_LIBRARY}
            ${JPEG_LIBRARY}
            ${PNG_LIBRARY}
            ${OGG_LIBRARY}
            ${VORBIS_LIBRARY}
            ${VORBISFILE_LIBRARY}
            ${OPENAL_LIBRARY}
            ${FREETYPE_LIBRARIES}
            ${ANDROID_LIBRARY})
            # ${tinyxml}
            # ${nvtristrip}
            # ${tristripper})

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

        if(${O3D_USE_EGL})
            target_link_libraries(${O3D_LIB_NAME} ${EGL_LIBRARIES})
        endif()

    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
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
            ${FREETYPE_LIBRARIES})
            # tinyxml
            # nvtristrip
            # tristripper)
        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

        if(${O3D_USE_EGL})
            target_link_libraries(${O3D_LIB_NAME} ${EGL_LIBRARIES})
        endif()

    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
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
            ${FREETYPE_LIBRARIES})
            # ${tinyxml}
            # ${nvtristrip}
            # ${tristripper})

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

    elseif(MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        target_link_libraries(${O3D_LIB_NAME}
            ${PNG_LIBRARY}
            ${JPEG_LIBRARY}
            ${OGG_LIBRARY}
            ${VORBIS_LIBRARY}
            ${VORBISFILE_LIBRARY}
            ${OPENAL_LIBRARY}
            ${FREETYPE_LIBRARIES}
            ${ZLIB_LIBRARY}
            opengl32
            ${OPENGL_LIBRARY})
            # ${tinyxml}
            # ${nvtristrip}
            #${tristripper})

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

    elseif(NOT MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        # todo upgrade and more cmake like
        target_link_libraries({O3D_LIB_NAME}
            ${OPENGL_gl_LIBRARY}
            libjpeg
            libpng
            libogg
            libvorbis
            libvorbisfile
            freetype235MT
            zlib
            OpenAL32)
            # ${tinyxml}
            # ${nvtristrip}
            # ${tristripper})

        if(${O3D_USE_SDL2})
            target_link_libraries(${O3D_LIB_NAME} ${SDL2_LIBRARIES})
        endif()

    endif()

    # build shaders @todo for any versions
    #add_custom_target(copy_shaders
    #	COMMAND	${CMAKE_COMMAND} -E copy_directory
    #            "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    #            "${CMAKE_CURRENT_BINARY_DIR}/shaders/")

    #add_custom_target(zip_shaders COMMAND zip -r shaders.zip "shaders")

    #execute_process(
    #    COMMAND ${CMAKE_COMMAND} -E copy_directory
    #			"${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    #    		"${CMAKE_CURRENT_BINARY_DIR}/shaders/"
    #    RESULT_VARIABLE _result
    #)

    #execute_process(
    #    COMMAND zip -r shaders.zip
    #        "${CMAKE_CURRENT_SOURCE_DIR}/shaders"
    #    RESULT_VARIABLE _result
    #)
endif()

# Install

install (TARGETS ${O3D_LIB_NAME}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
    COMPONENT library)
