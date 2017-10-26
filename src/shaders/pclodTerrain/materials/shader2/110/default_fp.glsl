#version 110

varying vec3 O3D_TextureWeight;

uniform sampler2D O3D_TexColormap;
uniform sampler2D O3D_Material0;
uniform sampler2D O3D_Material1;

varying vec2 io_texCoords[2];

//=================== MAIN =====================//
void main()
{
	gl_FragColor = O3D_TextureWeight[0] * texture2D(O3D_Material0, io_texCoords[1].xy) +
				   O3D_TextureWeight[1] * texture2D(O3D_Material1, io_texCoords[1].xy) +
				   O3D_TextureWeight[2] * texture2D(O3D_TexColormap, io_texCoords[0].xy);
}
