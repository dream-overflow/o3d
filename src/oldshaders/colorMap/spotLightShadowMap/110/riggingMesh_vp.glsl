// rigging mesh
#version 110

uniform mat4 u_shadowProjectionMatrix;

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

uniform mat4 u_matrixArray[50];
attribute float a_rigging;

attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec2 a_texCoords;

varying vec3 io_normal;
varying vec3 io_eyeVec;
varying vec3 io_vertex;

varying vec2 io_texCoords;
varying vec4 io_shadowCoords;

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
	else {
		vertex = a_vertex;
		normal = a_normal;
	}

	gl_Position = u_modelViewProjectionMatrix * vertex;

	// texture diffuse coordinates
	io_texCoords = a_texCoords;

	// transform the normal to modelView-space
	io_normal = u_normalMatrix * normal;

	// transform vertex
	vec4 vertexPos = u_modelViewMatrix * vertex;

	io_vertex = vertexPos.xyz;
	io_eyeVec = -vertexPos.xyz;
	
	// shadow map
	io_shadowCoords = u_shadowProjectionMatrix * a_vertex;
}
