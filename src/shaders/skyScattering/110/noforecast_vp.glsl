// vertex program no forecast
#version 110

attribute vec3 VertexColor;

varying vec3 io_color;

attribute vec4 a_vertex;
uniform mat4 u_modelViewProjectionMatrix;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_color = VertexColor;
}
