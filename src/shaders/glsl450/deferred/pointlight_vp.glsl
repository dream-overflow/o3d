// process the point light on the GBuffer
#version 450

uniform mat4 u_modelViewProjectionMatrix;

in vec4 a_vertex;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
