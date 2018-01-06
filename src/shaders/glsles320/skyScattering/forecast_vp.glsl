// vertex program forecast
#version 320 es

uniform vec2 uCoef;

in vec3 VertexColor;
in vec3 NextVertexColor;

layout(location = 0) smooth out vec3 io_color;

layout(location = 0) in vec4 a_vertex;
uniform mat4 u_modelViewProjectionMatrix;

invariant gl_Position;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_color = uCoef[0] * VertexColor + uCoef[1] * NextVertexColor;
}
