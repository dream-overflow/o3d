// process the spot light on the GBuffer
#version 320 es

uniform mat4 u_modelViewProjectionMatrix;

in vec4 a_vertex;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
