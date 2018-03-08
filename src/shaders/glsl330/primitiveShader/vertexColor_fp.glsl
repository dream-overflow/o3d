// Simple color fragment program
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

precision highp float;

layout(location = 0) smooth in vec4 io_color;

layout(location = 0) out vec4 o_finalColor;

void main()
{					 
	o_finalColor = io_color;
}
