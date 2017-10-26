// static mesh
#version 130

uniform mat4 u_modelViewProjectionMatrix;

in vec4 a_vertex;

invariant gl_Position;

void main()
{
	// transform the vertex
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
