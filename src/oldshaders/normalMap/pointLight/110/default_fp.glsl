// point light normal mapping fragment program
#version 110

uniform sampler2D u_texDiffuse;
uniform sampler2D u_texNormal;

varying vec2 io_texCoords;

//varying vec3 io_eyeVec;
varying vec3 io_lightVec;

uniform vec4 u_ambient;
uniform vec4 u_specular;

uniform float u_shininess;

uniform vec4 u_lightPos;
uniform vec4 u_lightDiffuse;
uniform vec3 u_lightAtt;

void main()
{
	vec3 lightVec = normalize(io_lightVec);

	vec3 normal = normalize(texture2D(u_texNormal, io_texCoords.xy).rgb * 2.0 - 1.0);
	normal.y = -normal.y;
	float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

	vec4 finalColor = u_ambient;
	
	if (NdotL > 0.0)
	{
		// diffuse
		vec4 diffuse = texture2D(u_texDiffuse, io_texCoords.xy);

		finalColor += u_lightDiffuse * diffuse * NdotL;

		// specular
//		vec3 eyeVec = normalize(io_eyeVec);

//		vec3 reflectVec = reflect(-lightVec, normal);
//		float specular = pow(max(dot(reflectVec, eyeVec), 0.0), u_shininess);
		
//		finalColor += u_specular * specular;
	}

	gl_FragColor = finalColor;	
}
