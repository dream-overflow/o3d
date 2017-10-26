// Simplest shader that draw a using a diffuse map and color
#version 110

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;
attribute vec2 a_texCoords;
 
varying vec2 io_texCoords;
 
void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_texCoords = a_texCoords;
}
