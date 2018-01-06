// vertex program forecast
#version 450

uniform vec2 uCoef;

layout(location = 0) in vec4 a_vertex;
layout(location = 1) in vec3 VertexColor;
layout(location = 2) in vec3 NextVertexColor;

layout(location = 0) smooth out vec3 io_color;

uniform mat4 u_modelViewProjectionMatrix;

invariant gl_Position;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_color = uCoef[0] * VertexColor + uCoef[1] * NextVertexColor;
}
