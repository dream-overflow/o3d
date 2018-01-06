// fragment program
#version 320 es

precision highp float;

uniform float u_brightness;
uniform sampler2D u_objectTexture;

layout(location = 0) smooth in vec2 io_texCoords1;

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
	o_finalColor = texture(u_objectTexture, io_texCoords1);
	o_finalColor.a *= u_brightness;
	
	#ifdef ALPHA_TEST_REF
	alphaTest(o_finalColor.a);
	#endif		
}
