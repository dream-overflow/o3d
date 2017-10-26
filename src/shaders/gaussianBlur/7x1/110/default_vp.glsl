// 7x1 gaussian blur vertex program
#version 110

uniform mat4 u_modelViewProjectionMatrix;

attribute vec4 a_vertex;

varying vec2 io_texCoords; 

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_texCoords = u_texCoords;
}
