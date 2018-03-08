// soft shadow map vertex program
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;

varying vec4 io_vertexPos;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_vertexPos = gl_Position;
}
