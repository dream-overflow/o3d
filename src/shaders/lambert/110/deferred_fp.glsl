// default fragment program
#version 110

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

#if defined(DIFFUSE_MAP) || defined(SPECULAR_MAP) || defined(SHINE_MAP)
smooth in vec2 io_texCoords1;
#endif

varying vec3 io_normal;
varying vec3 io_position;

#ifdef AMBIENT
out vec4 o_ambient;
#endif

#ifdef DIFFUSE
out vec4 o_diffuse;
#endif

#ifdef SPECULAR
out vec4 o_specular;
#endif

out vec4 o_normal;   // A is SHINE
out vec3 o_position;

void main()
{
#ifdef AMBIENT
//#ifdef DIFFUSE_MAP
//	o_ambient = texture(u_diffuseMap, io_texCoords1.xy);
//#else
	o_ambient = vec4(0,0,0,1);
//#endif
#endif

#ifdef DIFFUSE
#ifdef DIFFUSE_MAP
	o_diffuse = texture(u_diffuseMap, io_texCoords1.xy);
#else
	o_diffuse = u_diffuse;
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

