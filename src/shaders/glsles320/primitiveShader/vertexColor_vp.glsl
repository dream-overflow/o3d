// Vertex color with scale on vertex.
#version 320 es

uniform mat4 u_modelViewProjectionMatrix;
uniform vec4 u_color;
uniform vec3 u_scale;

layout(location = 0) in vec4 a_vertex;
layout(location = 4) in vec4 a_color;

layout(location = 0) smooth out vec4 io_color;

invariant gl_Position;

void main()
{
	vec4 vertex = vec4(a_vertex.x * u_scale.x, a_vertex.y * u_scale.y, a_vertex.z * u_scale.z, 1);

	gl_Position = u_modelViewProjectionMatrix * vertex;
	io_color = a_color * u_color;
}
