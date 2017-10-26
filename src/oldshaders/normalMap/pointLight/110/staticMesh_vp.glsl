// point light normal mapping static mesh vertex program
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec3 a_bitangent;
attribute vec2 a_texCoords;

uniform vec4 u_lightPos;

varying vec3 io_lightVec;
//varying vec3 io_eyeVec;
varying vec2 io_texCoords;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_texCoords = a_texCoords;

	mat3 matrixTBN = u_normalMatrix * mat3(a_tangent.xyz, a_bitangent.xyz, a_normal.xyz);

	vec4 vertexPos = u_modelViewMatrix * a_vertex;

	io_lightVec = normalize(u_lightPos.xyz - vertexPos.xyz) * matrixTBN;
	//io_eyeVec = -vertexPos.xyz;
}
