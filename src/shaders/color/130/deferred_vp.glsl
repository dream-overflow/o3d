// simple diffuse color/map without lighting
#version 130

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

in vec4 a_vertex;
in vec3 a_normal;

#ifdef RIGGING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

in float a_rigging;
#endif
#ifdef SKINNING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

in vec4 a_skinning;
in vec4 a_weighting;
#endif

#ifdef DIFFUSE_MAP
in vec2 a_texCoords1;
smooth out vec2 io_texCoords1;
#endif

smooth out vec3 io_normal;
smooth out vec3 io_position;

void main()
{
	//
	// transform the vertex
	//

	// for static mesh
#ifdef MESH
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	// transform the normal to modelView-space
	io_normal = u_normalMatrix * normal;

	// transform vertex
	io_position = (u_modelViewMatrix * vertex).xyz;
#endif

	// for rigging mesh
#ifdef RIGGING
	vec4 vertex;
	int boneId = int(a_rigging);

	if (boneId != -1)
		vertex = u_bonesMatrixArray[boneId] * a_vertex;
	else
		vertex = a_vertex;

	gl_Position = u_modelViewProjectionMatrix * vertex;
 
	// transform the normal to modelView-space
	io_normal = u_normalMatrix * normal;

	// transform vertex
	io_position = (u_modelViewMatrix * vertex).xyz;   
#endif

	// for skinning mesh
#ifdef SKINNING
	vec4 vertex;
	mat4 transform = mat4(0);
	ivec4 bonesId = ivec4(a_skinning);

	if (bonesId.x != -1)
	{
		transform += u_bonesMatrixArray[bonesId.x] * a_weighting.x;
		if (bonesId.y != -1)
		{
			transform += u_bonesMatrixArray[bonesId.y] * a_weighting.y;
			if (bonesId.z != -1)
			{
				transform += u_bonesMatrixArray[bonesId.z] * a_weighting.z;
				if (bonesId.w != -1)
				{
					transform += u_bonesMatrixArray[bonesId.w] * a_weighting.w;
				}
			}
		}
	}
	else
		transform = mat4(1);

	vertex = transform * a_vertex;
	gl_Position = u_modelViewProjectionMatrix * vertex;

	io_normal = u_normalMatrix * normal;

	// transform vertex
	io_position = (u_modelViewMatrix * vertex).xyz;   
#endif

#ifdef DIFFUSE_MAP
	io_texCoords1 = a_texCoords1;
#endif
}

