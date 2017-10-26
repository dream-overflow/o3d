// simple color/map without any lighting
#version 110

uniform vec4 u_diffuse;

#ifdef DIFFUSE_MAP
varying vec2 io_texCoords1;
uniform sampler2D u_diffuseMap;
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
	float r = texture2D(u_diffuseMap, io_texCoords1.xy).r * u_diffuse.r;
	float g = texture2D(u_diffuseMap, io_texCoords1.xy).a * u_diffuse.g;
#else
	float r = u_diffuse.r;
	float g = u_diffuse.g;
#endif
	#ifdef ALPHA_TEST_REF
	alphaTest(g);
	#endif
	gl_FragColor = vec4(r, r, r, g);
#else // RGBA
#ifdef AMBIENT_MAP
	vec4 diffuse = texture2D(u_diffuseMap, io_texCoords1.xy) * u_diffuse;
	#ifdef ALPHA_TEST_REF
	alphaTest(diffuse.a);
	#endif
	gl_FragColor = diffuse;
#else
	#ifdef ALPHA_TEST_REF
	alphaTest(u_diffuse.a);
	#endif
	gl_FragColor = u_diffuse;
#endif
#endif
}
