// process the ambient on the GBuffer
#version 330

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 0) in vec4 a_vertex;
//layout(location = 8) in vec2 a_texcoords1;

//layout(loction = 4) smooth out vec2 io_texcoords1;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
//	io_texCoords1 = a_texCoords1;
}
