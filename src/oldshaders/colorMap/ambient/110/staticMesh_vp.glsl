// static mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;
attribute vec2 a_texCoords;

varying vec2 io_texCoords;

void main()
{
	// transform the vertex
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	// Texture diffuse map coordinates
	io_texCoords = a_texCoords;
}
