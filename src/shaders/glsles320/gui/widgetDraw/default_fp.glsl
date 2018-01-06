// default ambiant fragment program
#version 320 es

precision highp float;

uniform vec4 u_ambient;

layout(location = 0) smooth in vec2 io_texCoords1;
uniform sampler2D u_ambientMap;

layout(location = 0) out vec4 o_finalColor;

void main()
{
	// Calculating The Final Color
	o_finalColor = texture(u_ambientMap, io_texCoords1.xy) * u_ambient;
}
