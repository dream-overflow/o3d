// Simple color fragment program
#version 320 es

precision highp float;

layout(location = 0) smooth in vec4 io_color;

layout(location = 0) out vec4 o_finalColor;

void main()
{					 
	o_finalColor = io_color;
}
