// default fragment program
#version 320 es

precision highp float;

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

#ifdef SHINE_MAP
uniform sampler2D u_shineMap;
#else
uniform float u_shine;
#endif

uniform sampler2D u_bumpMap;

layout(location = 4) smooth in vec2 io_texCoords1;
layout(location = 0) smooth in vec3 io_position;
smooth in vec3 io_lightVec;

#ifdef POINT_LIGHT
uniform vec4 u_lightPos;
uniform vec3 u_lightAtt;
#endif

#ifdef SPOT_LIGHT
uniform float u_lightCosCutOff;
uniform float u_lightExponent;
uniform vec4 u_lightPos;
uniform vec3 u_lightDir;
uniform vec3 u_lightAtt;
#endif

#ifdef DIRECTIONAL_LIGHT
uniform vec3 u_lightDir;
#endif

uniform vec3 u_eyePos;

layout(location = 0) out vec4 o_finalColor;

void main()
{
	vec4 finalColor = vec4(0);

#ifdef POINT_LIGHT
	vec3 lightVec = normalize(io_lightVec);

	vec3 normal = normalize(texture(u_bumpMap, io_texCoords1.xy).rgb * 2.0 - 1.0);
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
		vec4 diffuse = u_lightDiffuse * texture(u_diffuseMap, io_texCoords1.xy);
	#else
		vec4 diffuse = u_diffuse;
	#endif
		finalColor += diffuse * NdotL * attenuation;

		// specular
	#ifdef SPECULAR_MAP
		vec4 specular = u_lightSpecular * texture(u_specularMap, io_texCoords1.xy);
	#else
		vec4 specular = u_specular;
	#endif

		// shine
	 #ifdef SHINE_MAP
		 float shine = texture(u_shineMap, io_texCoords1.xy);
	 #else
		 float shine = u_shine;
	 #endif

		vec3 dirToEye = normalize(u_eyePos - io_position);
		vec3 reflectVec = reflect(-lightVec, normal);

		float specularIntensity = pow(max(dot(reflectVec, dirToEye), 0.0), shine);

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

		vec3 normal = normalize(texture(u_bumpMap, io_texCoords1.xy).rgb * 2.0 - 1.0);
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
			vec4 diffuse = u_lightDiffuse * texture(u_diffuseMap, io_texCoords1.xy);
		#else
			vec4 diffuse = u_diffuse;
		#endif
			finalColor += diffuse * NdotL * attenuation;

			// specular
		#ifdef SPECULAR_MAP
			vec4 specular = u_lightSpecular * texture(u_specularMap, io_texCoords1.xy);
		#else
			vec4 specular = u_specular;
		#endif

            // shine
         #ifdef SHINE_MAP
             float shine = texture(u_shineMap, io_texCoords1.xy);
         #else
             float shine = u_shine;
         #endif

			vec3 dirToEye = normalize(u_eyePos - io_position);
			vec3 reflectVec = reflect(-lightVec, normal);

			float specularIntensity = pow(max(dot(reflectVec, dirToEye), 0.0), shine);

			finalColor += specular * specularIntensity * attenuation;
		}
	}
#endif

#ifdef DIRECTIONAL_LIGHT
	vec3 lightVec = normalize(io_lightVec);

	vec3 normal = normalize(texture(u_bumpMap, io_texCoords1.xy).rgb * 2.0 - 1.0);
	//normal.y = -normal.y;

	float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

	if (NdotL > 0.0)
	{
		// diffuse
	#ifdef DIFFUSE_MAP
		vec4 diffuse = u_lightDiffuse * texture(u_diffuseMap, io_texCoords1.xy);
	#else
		vec4 diffuse = u_diffuse;
	#endif
		finalColor += diffuse * NdotL;

		// specular
	#ifdef SPECULAR_MAP
		vec4 specular = u_lightSpecular * texture(u_specularMap, io_texCoords1.xy);
	#else
		vec4 specular = u_specular;
	#endif

        // shine
     #ifdef SHINE_MAP
         float shine = texture(u_shineMap, io_texCoords1.xy);
     #else
         float shine = u_shine;
     #endif

		vec3 dirToEye = normalize(u_eyePos - io_position);
		vec3 reflectVec = reflect(-lightVec, normal);

		float specularIntensity = pow(max(dot(reflectVec, dirToEye), 0.0), shine);

		finalColor += specular * specularIntensity;
	}
#endif

	o_finalColor = finalColor;
}
