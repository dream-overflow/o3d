// vertex program no forecast
#version 320 es

in vec3 VertexColor;

smooth out vec3 io_color;

in vec4 a_vertex;
uniform mat4 u_modelViewProjectionMatrix;

invariant gl_Position;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_color = VertexColor;
}
