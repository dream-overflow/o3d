// default fragment program
#version 130

precision highp float;

uniform sampler2D u_normalMap;
uniform sampler2D u_diffuseMap;
uniform sampler2D u_heightMap;

uniform vec4 u_lightCol;
uniform vec2 u_bumpSize;

smooth in vec3 io_lightVec;
smooth in vec3 io_viewVec;
smooth in vec2 io_texCoords;

out vec4 o_finalColor:

void main()
{
	// LightAttenuation = clamp(1.0 - dot(g_lightVec, g_lightVec), 0.0, 1.0);
	vec3 lightVec = normalize(io_lightVec);
	vec3 viewVec = normalize(io_viewVec);

	float height = texture(u_heightMap, io_texCoords.xy).r;
	height = height * u_bumpSize.x + u_bumpSize.y;

	vec2 newUV = io_texCoord.xy + viewVec.xy * height;
	vec4 colorBase = texture(u_diffuseMap, newUV);
	vec3 bump = texture(u_normalMap, newUV.xy).rgb * 2.0 - 1.0;
	bump = normalize(bump);

	float base = texture(u_heightMap, newUV.xy).r;
	float diffuse = clamp(dot(lightVec, bump), 0.0, 1.0);
	float specular = pow(clamp(dot(reflect(-viewVec, bump), lightVec), 0.0, 1.0), 16.0);

	o_finalColor = colorBase * u_lightCol * (diffuse * base + 0.7 * specular);// * u_lightAttenuation;
	o_finalColor.a = 1.0;
}
