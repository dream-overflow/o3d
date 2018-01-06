// vertex program
#version 450

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 0) in vec4 a_vertex;
layout(location = 5) in vec2 a_texCoords1;

layout(location = 0) smooth out vec2 io_texCoords1;

invariant gl_Position;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_texCoords1 = a_texCoords1;
}
