// static mesh vertex program
#version 130

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

uniform float u_lightInvRad;
uniform vec4  u_lightPos;

in vec3 a_vertex;
in vec2 a_texCoords;

in vec3 a_tangent;
in vec3 a_bitangent;
in vec3 a_normal;

smooth out vec3 io_lightVec;
smooth out vec3 io_viewVec;
smooth out vec2 io_texCoords;

invariant gl_Position;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_texCoords = a_texCoords;

	mat3 matrixTBN = u_normalMatrix * mat3(a_tangent.xyz , a_bitangent.xyz, a_normal.xyz);

	vec4 vertex = u_modelViewMatrix * a_vertex;
	io_viewVec = vec3(-vertex) * matrixTBN;

	vec3 lightVec = u_lightInvRad * (u_lightPos.xyz - vertex.xyz);
	io_lightVec = lightVec * matrixTBN;
}
