// vertex program
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform vec4 u_units;

attribute vec4 a_vertex;

//=================== MAIN =====================//
void main()
{
	gl_Position = u_modelViewProjectionMatrix * (u_units * a_vertex);
}
