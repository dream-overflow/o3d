#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

precision highp float;

// Ponderation des differentes textures:
//   0) Ponderation du materiau 0
//   1) Ponderation du materiau 1
//   2) Ponderation du materiau 2
//   3) Ponderation de la colormap
smooth in vec2 O3D_TextureWeight;

uniform sampler2D O3D_TexColormap;
uniform sampler2D O3D_Material;
uniform sampler2D O3D_Lightmap;

smooth in vec2 io_texCoords[2];

layout(location = 0) out vec4 o_finalColor;

//=================== MAIN =====================//
void main()
{
	vec4 lBaseColor = (O3D_TextureWeight[0] * texture(O3D_Material, io_texCoords[1].xy) +
					   O3D_TextureWeight[1] * texture(O3D_TexColormap, io_texCoords[0].xy));

	vec4 lColormap = texture(O3D_Lightmap, io_texCoords[0].xy);

	o_finalColor[0] = 4.0 * lColormap[0] * lBaseColor[0];
	o_finalColor[1] = 4.0 * lColormap[1] * lBaseColor[1];
	o_finalColor[2] = 4.0 * lColormap[2] * lBaseColor[2];
	o_finalColor[3] = 1.0;
}
