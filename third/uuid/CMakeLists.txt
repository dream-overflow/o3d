#----------------------------------------------------------
# build uuid library
#----------------------------------------------------------

if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
	set(UUID_LIB_NAME uuid-dbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "RelWithDebInfo")
	set(UUID_LIB_NAME uuid-odbg)
elseif(${CMAKE_BUILD_TYPE} MATCHES "Release")
	set(UUID_LIB_NAME uuid)
endif()

add_library(${UUID_LIB_NAME} STATIC
			sysdep.c
            uuid.c
            md5c.c)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(${UUID_LIB_NAME}
)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	target_link_libraries(${UUID_LIB_NAME}
)
ELSEIF(MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(${UUID_LIB_NAME}
)
ELSEIF(NOT MINGW AND ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries({UUID_LIB_NAME}
)
ENDIF()

# Install
install (TARGETS ${UUID_LIB_NAME}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	COMPONENT library)
