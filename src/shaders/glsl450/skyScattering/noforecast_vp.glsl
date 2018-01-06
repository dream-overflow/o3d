// vertex program no forecast
#version 450

layout(location = 1) in vec3 VertexColor;

layout(location = 0) smooth out vec3 io_color;

layout(location = 0) in vec4 a_vertex;
uniform mat4 u_modelViewProjectionMatrix;

invariant gl_Position;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_color = VertexColor;
}
