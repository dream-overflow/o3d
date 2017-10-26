// default ambiant fragment program
#version 110

uniform vec4 u_ambient;

#ifdef AMBIENT_MAP
varying vec2 io_texCoords1;
uniform sampler2D u_ambientMap;
#endif

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
	float r = texture2D(u_ambientMap, io_texCoords1.xy).r * u_ambient.r;
	float g = texture2D(u_ambientMap, io_texCoords1.xy).a * u_ambient.g;
#else
	float r = u_ambient.r;
	float g = u_ambient.g;
#endif
	#ifdef ALPHA_TEST_REF
	alphaTest(g);
	#endif
	gl_FragColor = vec4(r, r, r, g);
#else // RGBA
#ifdef AMBIENT_MAP
	vec4 ambient = texture2D(u_ambientMap, io_texCoords1.xy) * u_ambient;
	#ifdef ALPHA_TEST_REF
	alphaTest(ambient.a);
	#endif
	gl_FragColor = ambient;
#else
	#ifdef ALPHA_TEST_REF
	alphaTest(u_ambient.a);
	#endif
	gl_FragColor = u_ambient;
#endif
#endif
}

