// default fragment program
#version 110

#ifdef DIFFUSE_MAP
uniform sampler2D u_diffuseMap;
uniform vec4 u_lightDiffuse;
#else
uniform vec4 u_diffuse;
#endif

#ifdef SPECULAR_MAP
uniform sampler2D u_specularMap;
uniform vec4 u_lightSpecular;
#else
uniform vec4 u_specular;
#endif

uniform sampler2D u_bumpMap;

uniform float u_shininess;

#if defined(DIFFUSE_MAP) || defined(SPECULAR_MAP)
varying vec2 io_texCoords1;
#endif

varying vec3 io_lightVec;
varying vec3 io_eyeVec;

#ifdef POINT_LIGHT
varying vec3 io_vertex;

uniform vec3 u_lightAtt;
#endif

#ifdef SPOT_LIGHT
varying vec3 io_vertex;

uniform float u_lightCosCutOff;
uniform float u_lightExponent;
uniform vec3 u_lightDir;
uniform vec3 u_lightAtt;
#endif

void main()
{
	vec4 finalColor = vec4(0);

#ifdef POINT_LIGHT
	vec3 lightVec = normalize(io_lightVec);

	vec3 normal = normalize(texture2D(u_bumpMap, io_texCoords1.xy).rgb * 2.0 - 1.0);
	//normal.y = -normal.y;

	float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

	if (NdotL > 0.0)
	{
		// distance beetwen the light and the fragment
		float dist = 0.0;//length(lightToVertex);

		// quadratic attenuation
		float attenuation = 1.0 / (u_lightAtt[0] + u_lightAtt[1] * dist + u_lightAtt[2] * dist * dist);

		// diffuse
	#ifdef DIFFUSE_MAP
		vec4 diffuse = u_lightDiffuse * texture2D(u_diffuseMap, io_texCoords1.xy);
	#else
		vec4 diffuse = u_diffuse;
	#endif
		finalColor += diffuse * NdotL * attenuation;

		// specular
	#ifdef SPECULAR_MAP
		vec4 specular = u_lightSpecular * texture2D(u_specularMap, io_texCoords1.xy);
	#else
		vec4 specular = u_specular;
	#endif
		vec3 eyeVec = normalize(io_eyeVec);
		vec3 reflectVec = reflect(-lightVec, normal);

		float specularIntensity = pow(max(dot(reflectVec, eyeVec), 0.0), u_shininess);

		finalColor += specular * specularIntensity * attenuation;
	}
#endif

#ifdef SPOT_LIGHT
	vec3 lightVec = normalize(io_lightVec);

	float DdotL = dot(-lightVec, u_lightDir);

	if (DdotL > u_lightCosCutOff)
	{
		float spotIntensity = pow((clamp(DdotL, u_lightCosCutOff, 1.0) -
				       	u_lightCosCutOff) / (1.0 - u_lightCosCutOff), u_lightExponent);

		vec3 normal = normalize(texture2D(u_bumpMap, io_texCoords1.xy).rgb * 2.0 - 1.0);
		//normal.y = -normal.y;

		float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

		NdotL = NdotL * spotIntensity;

		if (NdotL > 0.0)
		{
			// distance beetwen the light and the fragment
			float dist = 0.0;//length(lightToVertex);

			// quadratic attenuation
			float attenuation = 1.0 / (u_lightAtt[0] + u_lightAtt[1] * dist + u_lightAtt[2] * dist * dist);

			// diffuse
		#ifdef DIFFUSE_MAP
			vec4 diffuse = u_lightDiffuse * texture2D(u_diffuseMap, io_texCoords1.xy);
		#else
			vec4 diffuse = u_diffuse;
		#endif
			finalColor += diffuse * NdotL * attenuation;

			// specular
		#ifdef SPECULAR_MAP
			vec4 specular = u_lightSpecular * texture2D(u_specularMap, io_texCoords1.xy);
		#else
			vec4 specular = u_specular;
		#endif
			vec3 eyeVec = normalize(io_eyeVec);
			vec3 reflectVec = reflect(-lightVec, normal);

			float specularIntensity = pow(max(dot(reflectVec, eyeVec), 0.0), u_shininess);

			finalColor += specular * specularIntensity * attenuation;
		}
	}
#endif

#ifdef DIRECTIONAL_LIGHT
	vec3 lightVec = normalize(io_lightVec);

	vec3 normal = normalize(texture2D(u_bumpMap, io_texCoords1.xy).rgb * 2.0 - 1.0);
	//normal.y = -normal.y;

	float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

	if (NdotL > 0.0)
	{
		// diffuse
	#ifdef DIFFUSE_MAP
		vec4 diffuse = u_lightDiffuse * texture2D(u_diffuseMap, io_texCoords1.xy);
	#else
		vec4 diffuse = u_diffuse;
	#endif
		finalColor += diffuse * NdotL;

		// specular
	#ifdef SPECULAR_MAP
		vec4 specular = u_lightSpecular * texture2D(u_specularMap, io_texCoords1.xy);
	#else
		vec4 specular = u_specular;
	#endif
		vec3 eyeVec = normalize(io_eyeVec);
		vec3 reflectVec = reflect(-lightVec, normal);

		float specularIntensity = pow(max(dot(reflectVec, eyeVec), 0.0), u_shininess);

		finalColor += specular * specularIntensity;
	}
#endif

	gl_FragColor = finalColor;
}
