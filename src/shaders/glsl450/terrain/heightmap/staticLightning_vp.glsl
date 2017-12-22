// vertex program
#version 450

uniform mat4 u_modelViewProjectionMatrix;
uniform vec4 u_units;

in vec4 a_vertex;
in vec2 a_texCoords;

out vec2 io_texCoords;
out vec2 io_fragPosition;

//=================== MAIN =====================//
void main()
{
	vec4 lVertex = u_units * a_vertex;
	
	gl_Position = u_modelViewProjectionMatrix * lVertex;

	io_fragPosition = lVertex.zx;
	io_texCoords = a_texCoords;
}
