// default ambiant fragment program
#version 110

precision highp float;

#ifdef OPACITY_MAP
varying vec2 io_texCoords1;
uniform sampler2D u_opacityMap;
#endif

void main()
{
#ifdef OPACITY_MAP
	gl_FragColor.a = texture2D(u_opacityMap, io_texCoords1).a;
#endif
}
