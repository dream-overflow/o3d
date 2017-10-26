// skinning mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;

uniform mat4 u_matrixArray[50];
attribute vec4 a_skinning;
attribute vec4 a_weighting;

attribute vec4 a_vertex;
attribute vec2 a_texCoords;

varying vec2 io_texCoords;

void main()
{
	// transform the vertex
	vec4 vertex;
	mat4 transform = mat4(0);
	ivec4 bonesId = ivec4(a_skinning);

	if (bonesId.x != -1)
	{
		transform += u_matrixArray[bonesId.x] * a_weighting.x;
		if (bonesId.y != -1)
		{
			transform += u_matrixArray[bonesId.y] * a_weighting.y;
			if (bonesId.z != -1)
			{
				transform += u_matrixArray[bonesId.z] * a_weighting.z;
				if (bonesId.w != -1)
				{
					transform += u_matrixArray[bonesId.w] * a_weighting.w;
				}
			}
		}
	}
	else
	{
		transform = mat4(1);
	}

	vertex = transform * a_vertex;
	gl_Position = u_modelViewProjectionMatrix * vertex;

	// Texture diffuse map coordinates
	io_texCoords = a_texCoords;
}
