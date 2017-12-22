#version 450

precision highp float;

// Ponderation des differentes textures:
//   0) Ponderation du materiau 0
//   1) Ponderation du materiau 1
//   2) Ponderation du materiau 2
//   3) Ponderation de la colormap
smooth in vec2 O3D_TextureWeight;

uniform sampler2D O3D_TexColormap;
uniform sampler2D O3D_Material;

smooth in vec2 io_texCoords[2];

out vec4 o_finalColor;

//=================== MAIN =====================//
void main()
{
	o_finalColor = O3D_TextureWeight[0] * texture(O3D_Material, io_texCoords[1].xy) +
				   O3D_TextureWeight[1] * texture(O3D_TexColormap, io_texCoords[0].xy);
}
