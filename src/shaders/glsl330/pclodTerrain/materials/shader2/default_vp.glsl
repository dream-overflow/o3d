#version 330

uniform vec4 O3D_DistanceConfigs;
uniform vec4 O3D_ZoneConfigs;
uniform vec4 O3D_TopZonePosition;
uniform vec3 O3D_ViewPosition;

uniform sampler2D O3D_TexColormap;
uniform sampler2D O3D_Material0;
uniform sampler2D O3D_Material1;

// Materiau utilise pour le vertex en cours
in float O3D_MaterialId;

// Contient les ids des deux materiaux utilises
uniform vec2 O3D_MaterialTable;

smooth out vec3 O3D_TextureWeight;

uniform mat4 u_modelViewProjectionMatrix;
layout(location = 0) in vec4 a_vertex;

smooth out vec2 io_texCoords[2];

//=================== MAIN =====================//
void main()
{
	O3D_TextureWeight = vec3(0.0, 0.0, 0.0);

	float distanceToCamera = distance(a_vertex.xyz, O3D_ViewPosition);

	O3D_TextureWeight[2] = smoothstep(O3D_DistanceConfigs[0], O3D_DistanceConfigs[1], distanceToCamera);

	io_texCoords[0] = vec2(
			(a_vertex.z - O3D_TopZonePosition.z) * O3D_TopZonePosition.x,
			(a_vertex.x - O3D_TopZonePosition.w) * O3D_TopZonePosition.y);
	io_texCoords[1] = vec2(
			(a_vertex.z - O3D_TopZonePosition.z) * O3D_ZoneConfigs.z,
			(a_vertex.x - O3D_TopZonePosition.w) * O3D_ZoneConfigs.w);

	if (O3D_MaterialId == O3D_MaterialTable[0])
	{
		O3D_TextureWeight[0] = 1.0 - O3D_TextureWeight[2];
	}
	else if (O3D_MaterialId == O3D_MaterialTable[1])
	{
		O3D_TextureWeight[1] = 1.0 - O3D_TextureWeight[2];
	}
	// else les deux sont nuls (erreur)

	gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
