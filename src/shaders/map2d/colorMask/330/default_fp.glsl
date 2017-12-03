// icon fragment program
#version 330

precision highp float;

uniform vec4 u_color;
uniform sampler2D u_texture;

smooth in vec2 io_texCoords;

out vec4 o_finalColor;

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
	vec4 color = texture(u_texture, io_texCoords.xy);

	#ifdef ALPHA_TEST_REF
	alphaTest(color.a);
	#endif		
	
	o_finalColor.rgb = sign(color.a) * u_color.rgb * color.xyz;
	o_finalColor.a = color.a;
}
