// default ambiant fragment program
#version 110

uniform vec4 u_pickingColor;

#ifdef OPACITY_MAP
varying vec2 io_texCoords1;
uniform sampler2D u_opacityMap;
#endif

void main()
{
	// Calculating The Final Color
#ifdef OPACITY_MAP
	gl_FragColor = floor(texture2D(u_opacityMap, io_texCoords1).r) * u_pickingColor;
#else
	gl_FragColor = u_pickingColor;
#endif
}
