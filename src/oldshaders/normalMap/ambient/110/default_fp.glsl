// default fragment program
#version 110

varying vec2 io_texCoords;

uniform sampler2D u_texDiffuse;
uniform vec4 u_matAmbient;

void main()
{
	// Defining The Material Colors
	vec4 colorDiffuse = texture2D(u_texDiffuse, io_texCoords.xy);

	// Calculating The Final Color
	gl_FragColor = u_matAmbient * colorDiffuse;
}
