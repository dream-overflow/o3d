// Simplest shader that draw a using a diffuse map and color
#version 110

uniform vec4 u_color;
uniform sampler2D u_diffuseMap;

varying vec2 io_texCoords;

void main()
{
	vec4 colorDiffuse = texture2D(u_diffuseMap, io_texCoords.xy);
	gl_FragColor = u_color * colorDiffuse;
}
