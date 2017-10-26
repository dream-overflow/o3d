// default fragment program
#version 130

precision highp float;

uniform sampler2D u_texDiffuse;
uniform vec4 u_matAmbient;

smooth in vec2 io_texCoords;

out vec4 o_finalColor;

void main()
{
	// Defining The Material Colors
	vec4 colorDiffuse = texture(u_texDiffuse, io_texCoords.xy);

	// Calculating The Final Color
	o_finalColor = u_matAmbient * colorDiffuse;
}
