// fond 2d vertex program
#version 130

uniform mat4 u_modelViewProjectionMatrix;

in vec2 a_texCoords1;
in vec2 a_vertex;

smooth out vec2 io_texCoords1;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * vec4(a_vertex.x, a_vertex.y, 0.0, 1.0);
	gl_Position.z = 0.0;

	io_texCoords1 = a_texCoords1;
}
