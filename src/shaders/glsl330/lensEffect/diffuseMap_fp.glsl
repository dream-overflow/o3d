// Simplest shader that draw a using a diffuse map and color
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

precision highp float;

uniform vec4 u_color;
uniform sampler2D u_diffuseMap;

smooth in vec2 io_texCoords;

layout(location = 0) out vec4 o_finalColor;

void main()
{
    vec4 colorDiffuse = texture(u_diffuseMap, io_texCoords.xy);
	o_finalColor = u_color * colorDiffuse;
}
