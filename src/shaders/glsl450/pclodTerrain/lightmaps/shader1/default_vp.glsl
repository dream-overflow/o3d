#version 450

// Contient les parametres de distance, a savoir:
//   - 0) La distance max ou les materiaux sont utilises sans colormap
//   - 1) La distance min ou uniquement la colormap est utilise
// La difference est deux est la longueur du blend entre materiaux et colormap

uniform vec4 O3D_DistanceConfigs;

// Contient les infos sur les tailles des zones (toutes ces donnees en inverses):
//   - 0) La taille X des zones en unite opengl
//   - 1) La taille Y des zones
//   - 2) Le pas en X de la zone
//   - 3) Le pas en Y de la zone
uniform vec4 O3D_ZoneConfigs;

// Contient les infos relatifs a la TopZone (donnees inversees sauf pour la position)
//   - 0) Largeur de la topZone
//   - 1) Hauteur de la topZone
//   - 2) Position en X
//   - 3) Position en Y
uniform vec4 O3D_TopZonePosition;

// La position dde la camera
uniform vec3 O3D_ViewPosition;

uniform sampler2D O3D_TexColormap;
uniform sampler2D O3D_Material;
uniform sampler2D O3D_Lightmap;

// Ponderation des differentes textures:
//   0) Ponderation du materiau 0
//   1) Ponderation du materiau 1
//   2) Ponderation du materiau 2
//   3) Ponderation de la colormap */
smooth out vec2 O3D_TextureWeight;

uniform mat4 u_modelViewProjectionMatrix;
in vec4 a_vertex;

smooth out vec2 io_texCoords[2];

//=================== MAIN =====================//
void main()
{
	O3D_TextureWeight = vec2(0.0, 0.0);

	float distanceToCamera = distance(a_vertex.xyz, O3D_ViewPosition);

	// Ponderation de la colormap
	O3D_TextureWeight[1] = smoothstep(O3D_DistanceConfigs[0], O3D_DistanceConfigs[1], distanceToCamera);

	// Coordonnees de texture pour la colormap
	io_texCoords[0] = vec2(
			(a_vertex.z - O3D_TopZonePosition.z) * O3D_TopZonePosition.x,
			(a_vertex.x - O3D_TopZonePosition.w) * O3D_TopZonePosition.y);

	// Coordonnees de texture pour le materiau
	io_texCoords[1] = vec2(
			(a_vertex.z - O3D_TopZonePosition.z) * O3D_ZoneConfigs.z,
			(a_vertex.x - O3D_TopZonePosition.w) * O3D_ZoneConfigs.w);

	// A priori, aucun besoin d'appliquer la matrice model view, a voir plus tard
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	// Pour eviter que le fragment shader se tape une multiplication en plus (seulement une addition pour 1 materiau)
	O3D_TextureWeight[0] = 1.0 - O3D_TextureWeight[1];
}
