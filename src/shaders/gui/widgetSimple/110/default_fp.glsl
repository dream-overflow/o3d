// widget fragment program
#version 110

uniform float u_alpha;
uniform sampler2D u_texture;

varying vec2 io_texCoords;

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
	vec4 color = texture2D(u_texture, io_texCoords.xy);

	#ifdef ALPHA_TEST_REF
	alphaTest(color.a);
	#endif
	
	gl_FragColor.rgb = sign(color.a) * color.xyz;
	gl_FragColor.a = color.a * u_alpha;
}
