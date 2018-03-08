// vertex program
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

uniform mat4 u_modelViewProjectionMatrix;
uniform vec4 u_units;

layout(location = 0) in vec4 a_vertex;

//=================== MAIN =====================//
void main()
{
	gl_Position = u_modelViewProjectionMatrix * (u_units * a_vertex);
}
