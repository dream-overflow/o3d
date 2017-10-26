// default fragment program
#version 110

uniform vec4 u_matAmbient;

void main()
{
	// Calculating The Final Color
	gl_FragColor = u_matAmbient;
}
