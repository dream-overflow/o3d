// default fragment program
#version 320 es

precision highp float;

#ifdef AMBIENT_MAP
uniform sampler2D u_ambientMap;
#endif
uniform vec4 u_ambient;

#ifdef DIFFUSE_MAP
uniform sampler2D u_diffuseMap;
#else
uniform vec4 u_diffuse;
#endif

#ifdef SPECULAR_MAP
uniform sampler2D u_specularMap;
#else
uniform vec4 u_specular;
#endif

#ifdef SHINE_MAP
uniform sampler2D u_shineMap;
#else
uniform float u_shine;
#endif

#if defined(AMBIENT_MAP) || defined(DIFFUSE_MAP) || defined(SPECULAR_MAP) || defined(SHINE_MAP)
layout(location = 4) smooth in vec2 io_texCoords1;
#endif

layout(location = 1) smooth in vec3 io_normal;
layout(location = 0) smooth in vec3 io_position;

#ifdef AMBIENT
layout (location = 0) out vec4 o_ambient;
#endif

#ifdef DIFFUSE
layout(location = 3) out vec4 o_diffuse;
#endif

#ifdef SPECULAR
layout(location = 4) out vec4 o_specular;
#endif

layout(location = 1) out vec4 o_normal;   // A is SHINE
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
#ifdef DIFFUSE
#ifdef DIFFUSE_MAP
	o_diffuse = texture(u_diffuseMap, io_texCoords1.xy);
#ifdef ALPHA_TEST_REF
	alphaTest(diffuse.a);
#endif
#else
    o_diffuse = u_diffuse;
#endif
#endif

#ifdef AMBIENT
#ifdef AMBIENT_MAP
	o_ambient = texture(u_ambientMap, io_texCoords1.xy) * u_ambient;
#else
    #ifdef DIFFUSE_MAP
        // use diffuse map as ambient parameter
	    o_ambient = u_ambient * o_diffuse;
    #else
	    o_ambient = u_ambient;
    #endif
#endif
#endif

#ifdef SPECULAR
#ifdef SPECULAR_MAP
	o_specular = texture(u_specularMap, io_texCoords1.xy);
#else
	o_specular = u_specular;
#endif
#endif

    // normal
	o_normal.xyz = normalize(io_normal);
    // shine
	o_normal.a = u_shine;

    // vertex position
    o_position = io_position;
}
