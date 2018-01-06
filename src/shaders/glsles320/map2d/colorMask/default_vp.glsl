// icon vertex program
#version 320 es

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 0) in vec4 a_vertex;
layout(location = 5) in vec2 a_texCoords;

smooth out vec2 io_texCoords;

invariant gl_Position;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	gl_Position.z = 0.0;

	io_texCoords = a_texCoords;
}
