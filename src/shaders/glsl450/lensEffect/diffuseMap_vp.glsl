// Simplest shader that draw a using a diffuse map and color
#version 450

uniform mat4 u_modelViewProjectionMatrix;

in vec4 a_vertex;
in vec2 a_texCoords;
 
smooth out vec2 io_texCoords;
 
void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_texCoords = a_texCoords;
}
