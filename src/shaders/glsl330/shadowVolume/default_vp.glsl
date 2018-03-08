#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) in vec4 a_vertex;

uniform mat4 u_modelViewProjectionMatrix;

uniform vec4 u_lightPos;

void main()
{
	vec4 vertexToLight = a_vertex - u_lightPos;
	vertexToLight.w = 0.0;
	vec4 vertex = a_vertex.w * u_lightPos + vertexToLight;

	gl_Position = u_modelViewProjectionMatrix * vertex;
}
