// vertex program
#version 330

uniform mat4 u_modelViewProjectionMatrix;
uniform vec4 u_units;

in vec4 a_vertex;

//=================== MAIN =====================//
void main()
{
	gl_Position = u_modelViewProjectionMatrix * (u_units * a_vertex);
}
