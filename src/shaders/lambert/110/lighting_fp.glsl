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

#ifdef SHINE_MAP
uniform sampler2D u_shineMap;
#else
uniform float u_shine;
#endif

#if defined(DIFFUSE_MAP) || defined(SPECULAR_MAP) || defined(SHINE_MAP)
varying vec2 io_texCoords1;
#endif

varying vec3 io_normal;
varying vec3 io_positon;

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

void main()
{
	vec4 finalColor = vec4(0);

#ifdef POINT_LIGHT
	vec3 normal = normalize(io_normal);

	vec3 lightToVertex = u_lightPos.xyz - io_vertex;
	vec3 lightVec = normalize(lightToVertex);

	float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

	if (NdotL > 0.0)
	{
		// distance beetwen the light and the fragment
		float dist = length(lightToVertex);

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

        // shine
	#ifdef SHINE_MAP
		float shine = texture(u_shineMap, io_texCoords1.xy);
	#else
		float shine = u_shine;
	#endif

        vec3 dirToEye = normalize(u_eyePos - io_position);
        vec3 reflectVec = reflect(-normalize(u_lightPos.xyz), normal);

		float specularIntensity = pow(max(dot(reflectVec, dirToEye), 0.0), shine);

		finalColor += specular * specularIntensity * attenuation;
	}
#endif

#ifdef SPOT_LIGHT
	vec3 lightToVertex = u_lightPos.xyz - io_vertex;
	vec3 lightVec = normalize(lightToVertex);

	float DdotL = dot(-lightVec, u_lightDir);

	if (DdotL > u_lightCosCutOff)
	{
		float spotIntensity = pow((clamp(DdotL, u_lightCosCutOff, 1.0) -
				       	u_lightCosCutOff) / (1.0 - u_lightCosCutOff), u_lightExponent);

		vec3 normal = normalize(io_normal);
		float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

		NdotL = NdotL * spotIntensity;

		if (NdotL > 0.0)
		{
			// distance beetwen the light and the fragment
			float dist = length(lightToVertex);

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

          // shine
    	#ifdef SHINE_MAP
	    	float shine = texture(u_shineMap, io_texCoords1.xy);
    	#else
	    	float shine = u_shine;
    	#endif
   
            vec3 dirToEye = normalize(u_eyePos - io_position);
	    	vec3 reflectVec = reflect(u_lightDir, normal);

			float specularIntensity = pow(max(dot(reflectVec, dirToEye), 0.0), shine);

			finalColor += specular * specularIntensity * attenuation;
		}
	}
#endif

#ifdef DIRECTIONAL_LIGHT
	vec3 normal = normalize(io_normal);
	float NdotL = clamp(dot(normal, -u_lightDir), 0.0, 1.0);

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

        // shine
	#ifdef SHINE_MAP
		float shine = texture(u_shineMap, io_texCoords1.xy);
	#else
		float shine = u_shine;
	#endif

        vec3 dirToEye = normalize(u_eyePos - io_position);
		vec3 reflectVec = reflect(u_lightDir, normal);

		float specularIntensity = pow(max(dot(reflectVec, dirToEye), 0.0), shine);   

		finalColor += specular * specularIntensity;
	}
#endif
/*
#ifdef DIFFUSE_MAP
	finalColor *= texture2D(u_diffuseMap, io_texCoords1.xy).a;
#else
	finalColor *= u_diffuse.a;
#endif*/

	gl_FragColor = finalColor;
}
