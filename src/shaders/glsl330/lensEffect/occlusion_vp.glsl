// Minimalistic vertex program, use only of a vertex attribute and a transform matrix 
#version 330

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 0) in vec4 a_vertex;
 
void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
