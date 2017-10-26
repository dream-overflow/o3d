// fragment program
#version 110

uniform float u_invNoiseScale;						// Ex: a value of 2.0 means the noise texture size is equal to the half distance between two consecutive vertices

uniform sampler2D u_colormap;
uniform sampler2D u_normalmap;
uniform sampler2D u_noise;

varying vec2 io_texCoords;
varying vec2 io_fragPosition;

void main()
{
	gl_FragData[0] =  texture2D(u_colormap, io_texCoords)
					* texture2D(u_noise, u_invNoiseScale*io_fragPosition);
					
	gl_FragData[1] =  texture2D(u_normalmap, io_texCoords);
}
