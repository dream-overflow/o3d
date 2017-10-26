// 7x1 gaussian blur fragment program
#version 110

uniform vec2 u_scaleU;
uniform sampler2D u_textureSource;

varying vec2 io_texCoords;
 
void main()
{
	vec4 color = vec4(0.0);

	color += texture2D(u_textureSource, io_texCoords.st + vec2(-3.0*u_scaleU.x, -3.0*u_scaleU.y)) * 0.015625;
	color += texture2D(u_textureSource, io_texCoords.st + vec2(-2.0*u_scaleU.x, -2.0*u_scaleU.y)) * 0.09375;
	color += texture2D(u_textureSource, io_texCoords.st + vec2(-1.0*u_scaleU.x, -1.0*u_scaleU.y)) * 0.234375;
	color += texture2D(u_textureSource, io_texCoords.st + vec2(0.0 , 0.0)) * 0.3125;
	color += texture2D(u_textureSource, io_texCoords.st + vec2(1.0*u_scaleU.x,  1.0*u_scaleU.y)) * 0.234375;
	color += texture2D(u_textureSource, io_texCoords.st + vec2(2.0*u_scaleU.x,  2.0*u_scaleU.y)) * 0.09375;
	color += texture2D(u_textureSource, io_texCoords.st + vec2(3.0*u_scaleU.x, -3.0*u_scaleU.y)) * 0.015625;

	gl_FragColor = color;
}
