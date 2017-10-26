// static mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

attribute vec4 a_vertex;
attribute vec3 a_normal;

varying vec3 io_normal;
varying vec3 io_eyeVec;
varying vec3 io_vertex;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	// transform the normal to modelView-space
	io_normal = u_normalMatrix * a_normal;

	// transform light
	vec4 vertexPos = u_modelViewMatrix * a_vertex;

	io_vertex = vertexPos.xyz;
	io_eyeVec = -vertexPos.xyz;
}
