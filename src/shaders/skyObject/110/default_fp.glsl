// fragment program
#version 110

uniform float u_brightness;
uniform sampler2D u_objectTexture;

varying vec2 io_texCoords1;

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
	gl_FragColor = texture2D(u_objectTexture, io_texCoords1);
	gl_FragColor.a *= u_brightness;

	#ifdef ALPHA_TEST_REF
	alphaTest(gl_FragColor.a);
	#endif	
}
