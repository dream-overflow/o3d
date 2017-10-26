// normal mapping fragment program 'test version'
#version 110

uniform sampler2D u_normalMap;
uniform sampler2D u_diffuseMap;
uniform sampler2D u_occlMap;
uniform vec4 u_lightCol;

varying vec3 io_lightVec;
varying vec3 io_viewVec;
varying vec2 io_texCoords;

void main()
{
	vec3 lightVec = normalize(io_lightVec);
	vec3 fvnormal = normalize(texture2D(u_normalMap, io_texCoords.xy).rgb * 2.0 - 1.0);
	float fNDotL = dot(fvnormal, lightVec);

	vec3 reflection = normalize(((2.0 * fvnormal) * fNDotL) - lightVec);
	vec3 viewVec = normalize(io_viewVec);
	float fRDotV = max(0.0,dot(reflection, viewVec));

	float fOcclusionPower = 0.5;

	vec4 colorBase = texture2D(u_diffuseMap, io_texCoords.xy) * (1.0 + fOcclusionPower * (texture2D(u_occlMap, io_texCoords.xy) - 1.0));

	vec4 totalAmbiant = /*ambient * */0.6 * colorBase;
	float diffuse = /*diffuse * */0.8 * fNDotL * colorBase;
	float specular = /*specular * */0.3 * (pow(fRDotV,5.0/*fSpecularPower*/));

	gl_FragColor = totalAmbiant + colorBase * u_lightCol * diffuse;// + specular;
	gl_FragColor.a = 1.0;
}
