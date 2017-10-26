// fragment program
#version 110

varying vec3 io_color;

void main()
{
	gl_FragColor.rgb = io_color;
	gl_FragColor.a = 1.0;
}
