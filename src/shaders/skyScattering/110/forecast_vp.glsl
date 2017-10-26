// vertex program forecast
#version 110

uniform vec2 uCoef;

attribute vec3 VertexColor;
attribute vec3 NextVertexColor;

varying vec3 io_color;

attribute vec4 a_vertex;
uniform mat4 u_modelViewProjectionMatrix;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_color = uCoef[0] * VertexColor + uCoef[1] * NextVertexColor;
}
