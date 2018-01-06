// default ambiant fragment program for deferred shading
#version 330

precision highp float;

uniform vec4 u_ambient;

layout(location = 0) smooth in vec3 io_position;
layout(location = 1) smooth in vec3 io_normal;

#ifdef AMBIENT_MAP
layout(location = 4) smooth in vec2 io_texCoords1;
uniform sampler2D u_ambientMap;
#endif

#ifdef AMBIENT
layout (location = 0) layout (location = 0) out vec4 o_ambient;
#endif

layout(location = 3) out vec4 o_diffuse;

#ifdef SPECULAR
layout(location = 4) out vec4 o_specular;
#endif

layout(location = 1) out vec4 o_normal;
layout(location = 2) out vec3 o_position;

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
	o_ambient = vec4(r, r, r, g);
#else // RGBA
#ifdef AMBIENT_MAP
	vec4 ambient = texture(u_ambientMap, io_texCoords1.xy) * u_ambient;
	#ifdef ALPHA_TEST_REF
	alphaTest(ambient.a);
	#endif
	o_ambient = ambient;
#else
	#ifdef ALPHA_TEST_REF
	alphaTest(u_ambient.a);
	#endif
	o_ambient = u_ambient;
#endif
#endif

    o_diffuse = vec4(0,0,0,1); 

#ifdef SPECULAR
    o_specular = vec4(0,0,0,1);
#endif

    o_position = io_position;
    o_normal.xyz = normalize(io_normal);
    // shine
    o_normal.w = 1.0;
}
