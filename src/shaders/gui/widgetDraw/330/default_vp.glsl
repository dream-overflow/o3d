// static mesh
#version 330

uniform mat4 u_modelViewProjectionMatrix;

in vec4 a_vertex;

in vec2 a_texCoords1;
smooth out vec2 io_texCoords1;

invariant gl_Position;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_texCoords1 = a_texCoords1;
}
