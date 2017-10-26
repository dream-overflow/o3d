// skinning mesh
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

uniform mat4 u_matrixArray[50];
attribute vec4 a_skinning;
attribute vec4 a_weighting;

attribute vec4 a_vertex;
attribute vec3 a_normal;
attribute vec2 a_texCoords;

varying vec3 io_normal;
varying vec3 io_eyeVec;
varying vec3 io_vertex;

varying vec2 io_texCoords;

void main()
{
	// transform the vertex
	vec4 vertex;
	vec3 normal;
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

	// texture diffuse coordinates
	io_texCoords = a_texCoords;

	// transform the normal to modelView-space
	//normal = mat3(transform) * a_normal; this is GLSL 120
	normal = mat3(transform[0].xyz, transform[1].xyz, transform[2].xyz) * a_normal;
	io_normal = u_normalMatrix * normal;

	// transform light
	vec4 vertexPos = u_modelViewMatrix * vertex;

	io_vertex = vertexPos.xyz;
	io_eyeVec = -vertexPos.xyz;
}
