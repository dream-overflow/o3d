// normal mapping vertex program 'test version'
#version 110

uniform mat4 u_modelViewProjectionMatrix;
uniform mat4 u_modelViewMatrix;
uniform mat3 u_normalMatrix;

uniform vec4 u_lightPos;
uniform vec4 u_viewPos;

attribute vec4 a_vertex;
attribute vec2 a_texCoords;
attribute vec3 a_normal;
attribute vec3 a_tangent;
attribute vec3 a_bitangent;

varying vec3 io_lightVec;
varying vec3 io_viewVec;
varying vec2 io_texCoord;

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;
	io_texCoords = a_texCoords;

	mat3 matrixTBN = u_normalMatrix * mat3(a_tangent.xyz, a_bitangent.xyz, a_normal.xyz);

	vec4 vertex = u_modelViewMatrix * a_vertex;

	vec3 viewVec = u_viewPos.xyz - vertex.xyz;
	io_viewVec = viewVec * matrixTBN;

	vec3 lightVec = u_invRad * (u_lightPos.xyz - vertex.xyz);
	io_lightVec = lightVec * matrixTBN;
}
