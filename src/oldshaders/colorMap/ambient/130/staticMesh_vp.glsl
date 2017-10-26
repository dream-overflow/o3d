// static mesh
#version 130

uniform mat4 u_modelViewProjectionMatrix;

in vec4 a_vertex;
in vec2 a_texCoords;

smooth out vec2 io_texCoords;

invariant gl_Position;

void main()
{
	// transform the vertex
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	// Texture diffuse coordinates
	io_texCoords = a_texCoords;
}
