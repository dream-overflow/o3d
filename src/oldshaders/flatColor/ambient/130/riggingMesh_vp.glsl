// rigging mesh
#version 130

uniform mat4 u_modelViewProjectionMatrix;

uniform mat4 u_matrixArray[50];
in float a_rigging;

in vec4 a_vertex;

invariant gl_Position;

void main()
{
	//transform the vertex
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
}
