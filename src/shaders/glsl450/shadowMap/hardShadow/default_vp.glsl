// hard shadow map vertex program
#version 450

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
