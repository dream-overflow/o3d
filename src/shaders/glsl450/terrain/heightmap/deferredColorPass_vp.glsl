// vertex program
#version 450

uniform mat4 u_modelViewProjectionMatrix;
uniform vec4 u_units;

layout(location = 0) in vec4 a_vertex;
layout(location = 5) in vec2 a_texCoords;

// Peut etre redondant avec io_texCoords (a voir)
layout(location = 0) out vec2 io_fragPosition;
layout(location = 1) out vec2 io_texCoords;

//=================== MAIN =====================//
void main()
{
	vec4 lVertex = u_units * a_vertex;
	
	gl_Position = u_modelViewProjectionMatrix * lVertex;

	io_fragPosition = lVertex.zx;
	io_texCoords = a_texCoords;
}
