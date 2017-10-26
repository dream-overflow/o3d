// icon vertex program
#version 110

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;
attribute vec2 a_texCoords;

varying vec2 io_texCoords;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	gl_Position.z = 0.0;

	io_texCoords = a_texCoords;
}
