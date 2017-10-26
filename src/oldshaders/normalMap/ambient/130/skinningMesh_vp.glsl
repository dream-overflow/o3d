// skinning mesh
#version 130

uniform mat4 u_modelViewProjectionMatrix;

uniform mat4 u_matrixArray[50];
in vec4 a_skinning;
in vec4 a_weighting;

in vec4 a_vertex;
in vec2 a_texCoords;

smooth out vec2 io_texCoords;

invariant gl_Position;

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

	// Texture diffuse coordinates
	io_texCoords = a_texCoords;
}
