// default ambiant fragment program
#version 450

precision highp float;

uniform vec4 u_pickingColor;

#ifdef OPACITY_MAP
smooth in vec2 io_texCoords1;
uniform sampler2D u_opacityMap;
#endif

out vec4 o_finalColor;

void main()
{
	// Calculating The Final Color
#ifdef OPACITY_MAP
	o_finalColor = floor(texture(u_opacityMap, io_texCoords1).r) * u_pickingColor;
#else
	o_finalColor = u_pickingColor;
#endif
}
