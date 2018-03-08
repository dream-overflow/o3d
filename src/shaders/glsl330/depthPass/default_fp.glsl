// default ambiant fragment program
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

precision highp float;

#ifdef OPACITY_MAP
layout(location = 4) smooth in vec2 io_texCoords1;
uniform sampler2D u_opacityMap;
#endif

#ifdef OPACITY_MAP
layout(location = 0) out vec4 o_finalColor;
#endif

void main()
{
#ifdef OPACITY_MAP
	o_finalColor.a = texture(u_opacityMap, io_texCoords1).a;
#endif
}
