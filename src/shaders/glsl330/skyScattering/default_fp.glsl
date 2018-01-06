// default fragment program
#version 330

precision highp float;

layout(location = 0) smooth in vec3 io_color;

layout(location = 0) out vec4 o_finalColor;

void main()
{
	o_finalColor.rgb = io_color;
	o_finalColor.a = 1.0;
}
