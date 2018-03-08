// Simplest shader that draw a using a diffuse map and color
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 0) in vec4 a_vertex;
layout(location = 5) in vec2 a_texCoords;
 
smooth out vec2 io_texCoords;
 
void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_texCoords = a_texCoords;
}
