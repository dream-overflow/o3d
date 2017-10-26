// static mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec2 a_texCoords;

varying vec3 io_normal;
varying vec3 io_eyeVec;
varying vec3 io_vertex;

varying vec2 io_texCoords;

void main()
{
	// transform the vertex
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	// texture diffuse coordinates
	io_texCoords = a_texCoords;

	// transform the normal to modelView-space
	io_normal = u_normalMatrix * a_normal;

	// transform vertex
	vec4 vertexPos = u_modelViewMatrix * a_vertex;

	io_vertex = vertexPos.xyz;
	io_eyeVec = -vertexPos.xyz;
}
