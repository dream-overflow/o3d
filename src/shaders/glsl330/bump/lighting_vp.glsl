#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_worldMatrix;
uniform mat3 u_normalMatrix;

layout(location = 0) in vec4 a_vertex;
layout(location = 1) in vec3 a_normal;

layout(location = 2) in vec3 a_tangent;
layout(location = 3) in vec3 a_bitangent;

layout(location = 5) in vec2 a_texCoords1;
layout(location = 4) smooth out vec2 io_texCoords1;

#ifdef RIGGING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

layout(location = 7) in float a_rigging;
#endif

#ifdef SKINNING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

layout(location = 8) in vec4 a_skinning;
layout(location = 10) in vec4 a_weighting;
#endif

#if defined(POINT_LIGHT) || defined(SPOT_LIGHT)
uniform vec4 u_lightPos;
#else // DIRECTIONAL_LIGHT
uniform vec3 u_lightDir;
#endif

smooth out vec3 io_lightVec;
layout(location = 0) smooth out vec3 io_position;

void main()
{
	//
	// transform the vertex
	//

	// for static mesh
#ifdef MESH
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	mat3 matrixTBN = u_normalMatrix * mat3(a_tangent.xyz, a_bitangent.xyz, a_normal.xyz);

	vec4 vertexPos = u_worldMatrix * a_vertex;
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

	mat3 normalMat = u_normalMatrix * mat3(transform[0].xyz, transform[1].xyz, transform[2].xyz);

	// TODO compute the lightvec here dircetly and smooth out it
	mat3 matrixTBN = normalMat * mat3(a_tangent.xyz, a_bitangent.xyz, a_normal.xyz);

	// transform light
    vec4 vertexPos = u_worldMatrix * vertex;
#endif

	io_texCoords1 = a_texCoords1;

#if defined(POINT_LIGHT) || defined(SPOT_LIGHT)
	io_lightVec = normalize(u_lightPos.xyz - vertexPos.xyz) * matrixTBN;
#else // DIRECTIONAL_LIGHT
	io_lightVec = -u_lightDir * matrixTBN;
#endif

	io_position = vertexPos.xyz;
}
