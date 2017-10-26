// fond 2d fragment program
#version 110

uniform vec4 u_ambient;
uniform sampler2D u_ambientMap;

varying vec2 io_texCoords1;

void main()
{
	gl_FragColor = u_ambient * texture2D(u_ambientMap, io_texCoords1.xy).r;
}
