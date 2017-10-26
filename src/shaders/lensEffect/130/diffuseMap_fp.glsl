// Simplest shader that draw a using a diffuse map and color
#version 130

precision highp float;

uniform vec4 u_color;
uniform sampler2D u_diffuseMap;

smooth in vec2 io_texCoords;

out vec4 o_finalColor;

void main()
{
	vec4 colorDiffuse = texture2D(u_diffuseMap, io_texCoords.xy);
	o_finalColor = u_color * colorDiffuse;
}
