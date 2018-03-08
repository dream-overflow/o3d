// default ambiant fragment program
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

precision highp float;

uniform vec4 u_ambient;

#ifdef AMBIENT_MAP
layout(location = 4) smooth in vec2 io_texCoords1;
uniform sampler2D u_ambientMap;
#endif

layout(location = 0) out vec4 o_finalColor;

#ifdef ALPHA_TEST_REF
void alphaTest(float alpha)
{
#ifdef ALPHA_FUNC_GREATER
	if (alpha <= ALPHA_TEST_REF)
		discard;
#endif
#ifdef ALPHA_FUNC_LESS
	if (alpha >= ALPHA_TEST_REF)
		discard;
#endif
}
#endif

void main()
{
	// Calculating The Final Color
#ifdef RG // RG
#ifdef AMBIENT_MAP
	float r = texture(u_ambientMap, io_texCoords1.xy).r * u_ambient.r;
	float g = texture(u_ambientMap, io_texCoords1.xy).g * u_ambient.g;
#else
	float r = u_ambient.r;
	float g = u_ambient.g;
#endif
	#ifdef ALPHA_TEST_REF
	alphaTest(g);
	#endif
	o_finalColor = vec4(r, r, r, g);
#else // RGBA
#ifdef AMBIENT_MAP
	vec4 ambient = texture(u_ambientMap, io_texCoords1.xy) * u_ambient;
	#ifdef ALPHA_TEST_REF
	alphaTest(ambient.a);
	#endif
	o_finalColor = ambient;
#else
	#ifdef ALPHA_TEST_REF
	alphaTest(u_ambient.a);
	#endif
	o_finalColor = u_ambient;
#endif
#endif
}

