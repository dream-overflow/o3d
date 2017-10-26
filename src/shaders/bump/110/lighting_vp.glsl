// static mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

attribute vec4 a_vertex;
attribute vec3 a_normal;

attribute vec3 a_tangent;
attribute vec3 a_bitangent;

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

varying vec3 io_eyeVec;
varying vec3 io_lightVec;

#if defined(POINT_LIGHT) || defined(SPOT_LIGHT)
varying vec3 io_vertex;
uniform vec4 u_lightPos;
#else // DIRECTIONAL_LIGHT
uniform vec3 u_lightDir;
#endif

void main()
{
	//
	// transform the vertex
	//

	// for static mesh
#ifdef MESH
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	mat3 matrixTBN = u_normalMatrix * mat3(a_tangent.xyz, a_bitangent.xyz, a_normal.xyz);

	vec4 vertexPos = u_modelViewMatrix * a_vertex;
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

	mat3 matrixTBN = u_normalMatrix * mat3(a_tangent.xyz, a_bitangent.xyz, normal.xyz);

    vec4 vertexPos = u_modelViewMatrix * vertex;
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

	mat3 matrixTBN = u_normalMatrix * mat3(a_tangent.xyz, a_bitangent.xyz, normal.xyz);

	// transform light
    vec4 vertexPos = u_modelViewMatrix * vertex;
#endif

#ifdef DIFFUSE_MAP
	io_texCoords1 = a_texCoords1;
#endif

#if defined(POINT_LIGHT) || defined(SPOT_LIGHT)
	io_lightVec = normalize(u_lightPos.xyz - vertexPos.xyz) * matrixTBN;
	io_vertex = vertexPos.xyz;
#else // DIRECTIONAL_LIGHT
	io_lightVec = -u_lightDir * matrixTBN;
#endif
	io_eyeVec = -vertexPos.xyz;
}
