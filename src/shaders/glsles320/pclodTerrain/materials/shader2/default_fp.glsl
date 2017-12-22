#version 320 es

precision highp float;

smooth in vec3 O3D_TextureWeight;

uniform sampler2D O3D_TexColormap;
uniform sampler2D O3D_Material0;
uniform sampler2D O3D_Material1;

smooth in vec2 io_texCoords[2];

out vec4 o_finalColor;

//=================== MAIN =====================//
void main()
{
	o_finalColor = O3D_TextureWeight[0] * texture(O3D_Material0, io_texCoords[1].xy) +
				   O3D_TextureWeight[1] * texture(O3D_Material1, io_texCoords[1].xy) +
				   O3D_TextureWeight[2] * texture(O3D_TexColormap, io_texCoords[0].xy);
}
