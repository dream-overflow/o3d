// static mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;

void main()
{
	// transform the vertex
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
