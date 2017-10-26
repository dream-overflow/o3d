#version 110

varying vec3 O3D_TextureWeight;

uniform sampler2D O3D_TexColormap;
uniform sampler2D O3D_Material0;
uniform sampler2D O3D_Material1;
uniform sampler2D O3D_Lightmap;

varying vec2 io_texCoords[2];

//=================== MAIN =====================//
void main()
{
	vec4 lBaseColor = (O3D_TextureWeight[0] * texture2D(O3D_Material0, io_texCoords[1].xy) +
					   O3D_TextureWeight[1] * texture2D(O3D_Material1, io_texCoords[1].xy) +
					   O3D_TextureWeight[2] * texture2D(O3D_TexColormap, io_texCoords[0].xy));

	vec4 lColormap = texture2D(O3D_Lightmap, io_texCoords[0].xy);

	gl_FragColor[0] = 4.0 * lColormap[0] * lBaseColor[0];
	gl_FragColor[1] = 4.0 * lColormap[1] * lBaseColor[1];
	gl_FragColor[2] = 4.0 * lColormap[2] * lBaseColor[2];
	gl_FragColor[3] = 1.0;
}
