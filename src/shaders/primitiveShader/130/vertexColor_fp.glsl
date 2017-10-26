// Simple color fragment program
#version 130

precision highp float;

smooth in vec4 io_color;

out vec4 o_finalColor;

void main()
{					 
	o_finalColor = io_color;
}
