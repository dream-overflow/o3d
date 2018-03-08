// fragment program
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

precision highp float;

uniform float u_invNoiseScale;						// Ex: a value of 2.0 means the noise texture size is equal to the half distance between two consecutive vertices

uniform sampler2D u_colormap;
uniform sampler2D u_noise;

layout(location = 0) in vec2 io_fragPosition;
layout(location = 1) in vec2 io_texCoords;

layout(location = 0) out vec4 o_finalColor;

void main()
{
    o_finalColor = texture(u_colormap, io_texCoords) * texture(u_noise, u_invNoiseScale*io_fragPosition);
}
