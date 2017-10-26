// soft shadow map vertex program
#version 110

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;

varying vec4 io_vertexPos;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_vertexPos = gl_Position;
}
