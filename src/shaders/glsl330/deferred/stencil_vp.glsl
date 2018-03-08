// process the transform
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 0) in vec4 a_vertex;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
