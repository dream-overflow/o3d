// simple color/map without any lighting
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

precision highp float;

uniform vec4 u_diffuse;

layout(location = 0) smooth in vec3 io_position;
layout(location = 1) smooth in vec3 io_normal;

#ifdef DIFFUSE_MAP
layout(location = 4) smooth in vec2 io_texCoords1;
uniform sampler2D u_diffuseMap;
#endif

#ifdef AMBIENT
layout (location = 0) out vec4 o_ambient;
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
#ifdef DIFFUSE_MAP
	float r = texture(u_diffuseMap, io_texCoords1.xy).r * u_diffuse.r;
	float g = texture(u_diffuseMap, io_texCoords1.xy).g * u_diffuse.g;
#else
	float r = u_diffuse.r;
	float g = u_diffuse.g;
#endif
	#ifdef ALPHA_TEST_REF
	alphaTest(g);
	#endif
	o_diffuse = vec4(r, r, r, g);
#else // RGBA
#ifdef DIFFUSE_MAP
	vec4 diffuse = texture(u_diffuseMap, io_texCoords1.xy) * u_diffuse;
	#ifdef ALPHA_TEST_REF
	alphaTest(diffuse.a);
	#endif
	o_diffuse = diffuse;
#else
	#ifdef ALPHA_TEST_REF
	alphaTest(u_diffuse.a);
	#endif
	o_diffuse = u_diffuse;
#endif
#endif

#ifdef SPECULAR
    o_specular = vec4(0,0,0,1);
#endif

    o_position = io_position;
    o_normal.xyz = normalize(io_normal);
    // shine
    o_normal.w = 1.0;
}
