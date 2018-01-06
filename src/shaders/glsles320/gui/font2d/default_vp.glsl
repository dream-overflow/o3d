// fond 2d vertex program
#version 320 es

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 5) in vec2 a_texCoords1;
layout(location = 0) in vec2 a_vertex;

layout(location = 0) smooth out vec2 io_texCoords1;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * vec4(a_vertex.x, a_vertex.y, 0.0, 1.0);
	gl_Position.z = 0.0;

	io_texCoords1 = a_texCoords1;
}
