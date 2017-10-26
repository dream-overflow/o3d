// rigging mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;

uniform mat4 u_matrixArray[50];
attribute float a_rigging;

attribute vec4 a_vertex;
attribute vec2 a_texCoords;

varying vec2 io_texCoords;

void main()
{
	// transform the vertex
	vec4 vertex;
	int boneId = int(a_rigging);

	if (boneId != -1)
	{
		vertex = u_matrixArray[boneId] * a_vertex;
	}
	else
	{
		vertex = a_vertex;
	}

	gl_Position = u_modelViewProjectionMatrix * vertex;

	// Texture diffuse map coordinates
	io_texCoords = a_texCoords;
}
