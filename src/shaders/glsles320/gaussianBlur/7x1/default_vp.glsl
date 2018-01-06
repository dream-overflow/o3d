// 7x1 gaussian blur vertex program
#version 320 es

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 0) in vec4 a_vertex;

layout(location = 0) smooth out vec2 io_texCoords;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_texCoords = u_texCoords;
}
