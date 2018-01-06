// 7x1 gaussian blur fragment program
#version 320 es

precision highp float;

uniform vec2 u_scaleU;
uniform sampler2D u_textureSource;

layout(location = 0) smooth in vec2 io_texCoords;

layout(location = 0) out vec4 o_finalColor;

void main()
{
	vec4 color = vec4(0.0);

	color += texture(u_textureSource, io_texCoords.st + vec2(-3.0*u_scaleU.x, -3.0*u_scaleU.y)) * 0.015625;
	color += texture(u_textureSource, io_texCoords.st + vec2(-2.0*u_scaleU.x, -2.0*u_scaleU.y)) * 0.09375;
	color += texture(u_textureSource, io_texCoords.st + vec2(-1.0*u_scaleU.x, -1.0*u_scaleU.y)) * 0.234375;
	color += texture(u_textureSource, io_texCoords.st + vec2(0.0 , 0.0)) * 0.3125;
	color += texture(u_textureSource, io_texCoords.st + vec2(1.0*u_scaleU.x,  1.0*u_scaleU.y)) * 0.234375;
	color += texture(u_textureSource, io_texCoords.st + vec2(2.0*u_scaleU.x,  2.0*u_scaleU.y)) * 0.09375;
	color += texture(u_textureSource, io_texCoords.st + vec2(3.0*u_scaleU.x, -3.0*u_scaleU.y)) * 0.015625;

	o_finalColor = color;
}
