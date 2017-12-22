// fragment program
#version 450

uniform float u_invNoiseScale;						// Ex: a value of 2.0 means the noise texture size is equal to the half distance between two consecutive vertices

uniform sampler2D u_colormap;
uniform sampler2D u_lightmap;
uniform sampler2D u_shadowmap;
uniform sampler2D u_noise;

in vec2 io_texCoords;
in vec2 io_fragPosition;

out vec4 o_finalColor[2];

void main()
{
    o_finalColor[0] = 	texture2D(u_colormap, io_texCoords)
                      * texture2D(u_lightmap, io_texCoords)
                      * texture2D(u_shadowmap, io_texCoords)
                      * texture2D(u_noise, u_invNoiseScale*io_fragPosition);
}
