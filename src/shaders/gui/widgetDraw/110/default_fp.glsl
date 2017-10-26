// default ambiant fragment program
#version 110

uniform vec4 u_ambient;

varying vec2 io_texCoords1;
uniform sampler2D u_ambientMap;

void main()
{
	// Calculating The Final Color
	gl_FragColor = texture2D(u_ambientMap, io_texCoords1.xy) * u_ambient;
}
