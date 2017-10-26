// default fragment program
#version 130

precision highp float;

smooth in vec3 io_color;

out vec4 o_finalColor;

void main()
{
	o_finalColor.rgb = io_color;
	o_finalColor.a = 1.0;
}
