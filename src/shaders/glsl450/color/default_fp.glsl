// simple color/map without any lighting
#version 450

precision highp float;

uniform vec4 u_diffuse;

#ifdef DIFFUSE_MAP
layout(location = 8) smooth in vec2 io_texCoords1;
uniform sampler2D u_diffuseMap;
#endif

layout(location = 0) out vec4 o_finalColor;

#ifdef ALPHA_TEST_REF
void alphaTest(float alpha)
{
#ifdef ALPHA_FUNC_GREATER
    if (alpha <= ALPHA_TEST_REF) {
		discard;
    }
#endif
#ifdef ALPHA_FUNC_LESS
    if (alpha >= ALPHA_TEST_REF) {
		discard;
    }
#endif
}
#endif

void main()
{
	// Calculating The Final Color
#ifdef RG // RG
#ifdef DIFFUSE_MAP
	float r = texture(u_diffuseMap, io_texCoords1.xy).r * u_diffuse.r;
	float g = texture(u_diffuseMap, io_texCoords1.xy).g * u_diffure.g;
#else
	float r = u_diffuse.r;
	float g = u_diffuse.g;
#endif
	#ifdef ALPHA_TEST_REF
	alphaTest(g);
	#endif
	o_finalColor = vec4(r, r, r, g);
#else // RGBA
#ifdef DIFFUSE_MAP
	vec4 diffuse = texture(u_diffuseMap, io_texCoords1.xy) * u_diffuse;
	#ifdef ALPHA_TEST_REF
	alphaTest(diffuse.a);
	#endif
	o_finalColor = diffuse;
#else
	#ifdef ALPHA_TEST_REF
	alphaTest(u_diffuse.a);
	#endif
	o_finalColor = u_diffuse;
#endif
#endif
}
