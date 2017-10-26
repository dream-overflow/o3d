// rigging mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

uniform mat4 u_matrixArray[50];
attribute float a_rigging;

attribute vec4 a_vertex;
attribute vec3 a_normal;

uniform vec4 u_lightPos;

varying vec3 io_normal;
varying vec3 io_lightVec;
varying vec3 io_eyeVec;
varying vec3 io_vertex;

void main()
{
	// transform the vertex
	vec4 vertex;
	vec3 normal;
	int boneId = int(a_rigging);

	if (boneId != -1)
	{
		vertex = u_matrixArray[boneId] * a_vertex;
		//normal = mat3(u_matrixArray[boneId]) * a_normal; this is GLSL 120
		normal = mat3(u_matrixArray[boneId][0].xyz, u_matrixArray[boneId][1].xyz, u_matrixArray[boneId][2].xyz) * a_normal;
	}
	else
	{
		vertex = a_vertex;
		normal = a_normal;
	}

	gl_Position = u_modelViewProjectionMatrix * vertex;

	// transform the normal to modelView-space
	io_normal = u_normalMatrix * normal;

	// transform light
	vec4 vertexPos = u_modelViewMatrix * vertex;

	io_vertex = vertexPos.xyz;
	io_eyeVec = -vertexPos.xyz;
}
