#version 130

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_worldMatrix;
uniform mat3 u_normalMatrix;

in vec4 a_vertex;
in vec3 a_normal;

in vec3 a_tangent;
in vec3 a_bitangent;

in vec2 a_texCoords1;
smooth out vec2 io_texCoords1;

#ifdef RIGGING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

in float a_rigging;
#endif

#ifdef SKINNING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

in vec4 a_skinning;
in vec4 a_weighting;
#endif

smooth out vec3 io_lightVec;
smooth out vec3 io_position;
out mat3 io_matrixTBN;

void main()
{
	//
	// transform the vertex
	//

	// for static mesh
#ifdef MESH
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_matrixTBN = u_normalMatrix * mat3(a_tangent.xyz, a_bitangent.xyz, a_normal.xyz);

 	// transform vertex
	io_position = (u_worldMatrix * a_vertex).xyz;
#endif

	// for rigging mesh
#ifdef RIGGING
	vec4 vertex;
	mat3 normalMat;
	int boneId = int(a_rigging);

	if (boneId != -1)
	{
		vertex = u_bonesMatrixArray[boneId] * a_vertex;

		// transformed normal matrix
		normalMat = u_normalMatrix * mat3(
					u_bonesMatrixArray[boneId][0].xyz,
					u_bonesMatrixArray[boneId][1].xyz,
					u_bonesMatrixArray[boneId][2].xyz);
	}
	else
	{
		vertex = a_vertex;

		// default normal matrix
		normalMat = u_normalMatrix;
	}

	gl_Position = u_modelViewProjectionMatrix * vertex;

	io_matrixTBN = normalMat * mat3(a_tangent.xyz, a_bitangent.xyz, a_normal.xyz);

	// transform vertex
	io_position = (u_worldMatrix * vertex).xyz;
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

	// transform vertex and normal by skinning
	vertex = transform * a_vertex;
	gl_Position = u_modelViewProjectionMatrix * vertex;

	mat3 normalMat = u_normalMatrix * mat3(transform[0].xyz, transform[1].xyz, transform[2].xyz);

	// TODO compute the lightvec here dircetly and smooth out it
	io_matrixTBN = normalMat * mat3(a_tangent.xyz, a_bitangent.xyz, a_normal.xyz);

	// transform light
	io_position = (u_worldMatrix * vertex).xyz;
#endif

    io_texCoords1 = a_texCoords1;
}
