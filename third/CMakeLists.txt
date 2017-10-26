#----------------------------------------------------------
# build
#----------------------------------------------------------

add_library(tinyxml STATIC
	tinyxml/tinystr.cpp 
	tinyxml/tinyxml.cpp
	tinyxml/tinyxmlerror.cpp
	tinyxml/tinyxmlparser.cpp)

add_library(tristripper STATIC
	TriStripper/Src/connectivity_graph.cpp
	TriStripper/Src/policy.cpp
	TriStripper/Src/tri_stripper.cpp)

add_library(nvtristrip STATIC
	nvTriStrip/NvTriStrip.cpp
	nvTriStrip/NvTriStripObjects.cpp)

