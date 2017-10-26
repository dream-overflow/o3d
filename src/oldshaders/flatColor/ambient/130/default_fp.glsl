// default fragment program
#version 130

precision highp float;

uniform vec4 u_matAmbient;

out vec4 o_finalColor;

void main()
{
	// Calculating The Final Color
	o_finalColor = u_matAmbient;
}
