// vertex program
#version 110

attribute vec2 a_texCoords1;
varying vec2 io_texCoords1;

attribute vec4 a_vertex;
uniform mat4 u_modelViewProjectionMatrix;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_texCoords1 = a_texCoords1;
}
