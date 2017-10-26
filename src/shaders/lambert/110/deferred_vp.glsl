// static mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_worldMatrix;
uniform mat3 u_normalMatrix;

attribute vec4 a_vertex;
attribute vec3 a_normal;

#ifdef DIFFUSE_MAP
attribute vec2 a_texCoords1;
varying vec2 io_texCoords1;
#endif

#ifdef RIGGING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

attribute float a_rigging;
#endif

#ifdef SKINNING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

attribute vec4 a_skinning;
attribute vec4 a_weighting;
#endif

varying vec3 io_normal;
varying vec3 io_position;

void main()
{
	//
	// transform the vertex
	//

	// for static mesh
#ifdef MESH
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	// transform the normal to modelView-space
	io_normal = u_normalMatrix * a_normal;

 	// transform vertex
	vec4 vertexPos = u_worldMatrix * a_vertex;
#endif

	// for rigging mesh
#ifdef RIGGING
	vec4 vertex;
	vec3 normal;
	int boneId = int(a_rigging);

	if (boneId != -1)
	{
		vertex = u_bonesMatrixArray[boneId] * a_vertex;
		normal = mat3(
				u_bonesMatrixArray[boneId][0].xyz,
				u_bonesMatrixArray[boneId][1].xyz,
				u_bonesMatrixArray[boneId][2].xyz) * a_normal;
	}
	else
	{
		vertex = a_vertex;
		normal = a_normal;
	}

	gl_Position = u_modelViewProjectionMatrix * vertex;

	// transform the normal to modelView-space
	io_normal = u_normalMatrix * normal;

	// transform vertex
	vec4 vertexPos = u_worldMatrix * vertex;
#endif

	// for skinning mesh
#ifdef SKINNING
	vec4 vertex;
	vec3 normal;
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

	// transform the normal to modelView-space
	normal = mat3(transform[0].xyz, transform[1].xyz, transform[2].xyz) * a_normal;
	io_normal = u_normalMatrix * normal;

	// transform light
	vec4 vertexPos = u_worldMatrix * vertex;
#endif

#ifdef DIFFUSE_MAP
	io_texCoords1 = a_texCoords1;
#endif

	io_position = vertexPos.xyz;
}
