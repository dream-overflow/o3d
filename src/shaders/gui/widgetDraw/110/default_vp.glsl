// static mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;

attribute vec2 a_texCoords1;
varying vec2 io_texCoords1;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_texCoords1 = a_texCoords1;
}
