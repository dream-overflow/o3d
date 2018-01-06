// static mesh
#version 330

uniform mat4 u_modelViewProjectionMatrix;

layout(location = 0) in vec4 a_vertex;

#ifdef RIGGING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

layout(location = 7) in float a_rigging;
#endif
#ifdef SKINNING
uniform mat4 u_bonesMatrixArray[NUM_BONES];

layout(location = 8) in vec4 a_skinning;
layout(location = 10) in vec4 a_weighting;
#endif

#ifdef AMBIENT_MAP
layout(location = 5) in vec2 a_texCoords1;
layout(location = 4) smooth out vec2 io_texCoords1;
#endif

void main()
{
	//
	// transform the vertex
	//

	// for static mesh
#ifdef MESH
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
#endif

	// for rigging mesh
#ifdef RIGGING
	vec4 vertex;
	int boneId = int(a_rigging);

	if (boneId != -1)
		vertex = u_bonesMatrixArray[boneId] * a_vertex;
	else
		vertex = a_vertex;

	gl_Position = u_modelViewProjectionMatrix * vertex;
#endif

	// for skinning mesh
#ifdef SKINNING
	vec4 vertex;
	mat4 transform = mat4(0);
	ivec4 bonesId = ivec4(a_skinning);

	if (bonesId.x != -1)
	{
		transform += u_bonesMatrixArray[bonesId.x] * a_weighting.x;
		if (bonesId.y != -1)
		{
			transform += u_bonesMatrixArray[bonesId.y] * a_weighting.y;
			if (bonesId.z != -1)
			{
				transform += u_bonesMatrixArray[bonesId.z] * a_weighting.z;
				if (bonesId.w != -1)
				{
					transform += u_bonesMatrixArray[bonesId.w] * a_weighting.w;
				}
			}
		}
	}
	else
		transform = mat4(1);

	vertex = transform * a_vertex;
	gl_Position = u_modelViewProjectionMatrix * vertex;
#endif

#ifdef AMBIENT_MAP
	io_texCoords1 = a_texCoords1;
#endif
}
