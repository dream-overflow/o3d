// vertex program
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform vec4 u_units;

attribute vec4 a_vertex;
attribute vec2 a_texCoords;

varying vec2 io_texCoords;
varying vec2 io_fragPosition;	// Peut être redondant avec io_texCoords (a voir)

//=================== MAIN =====================//
void main()
{
	vec4 lVertex = u_units * a_vertex;
	
	gl_Position = u_modelViewProjectionMatrix * lVertex;

	io_fragPosition = lVertex.zx;
	io_texCoords = a_texCoords;
}
