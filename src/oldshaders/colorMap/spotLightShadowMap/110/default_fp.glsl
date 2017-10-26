// default fragment program
#version 110

uniform sampler2D u_texDiffuse;

uniform vec4 u_lightDiffuse;

varying vec2 io_texCoords;

varying vec3 io_normal;
varying vec3 io_eyeVec;
varying vec3 io_vertex;

uniform vec4 u_ambient;
uniform vec4 u_specular;

uniform float u_shininess;
uniform float u_lightCosCutOff;
uniform float u_lightExponent;
uniform vec4 u_lightPos;
uniform vec3 u_lightDir;
uniform vec3 u_lightAtt;

uniform sampler2D u_shadowMap;
varying vec4 io_shadowCoords;

void main()
{
	vec3 lightToVertex = u_lightPos.xyz - io_vertex;
	vec3 lightVec = normalize(lightToVertex);

	vec4 finalColor = u_ambient;
	float DdotL = dot(-lightVec, u_lightDir);
	
	if (DdotL > u_lightCosCutOff)
	{
		float spotIntensity = pow((clamp(DdotL, u_lightCosCutOff, 1.0) - u_lightCosCutOff)/(1.0 - u_lightCosCutOff), u_lightExponent);

		vec3 normal = normalize(io_normal);
		float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

		NdotL = NdotL * spotIntensity;

		if (NdotL > 0.0)
		{
			// distance beetwen the light and the fragment
			float dist = length(lightToVertex);

			// quadratic attenuation
			float attenuation = 1.0 / (u_lightAtt[0] + u_lightAtt[1] * dist + u_lightAtt[2] * dist * dist);

			// shadow
			vec4 shadowCoordinateWdivide = io_shadowCoords / io_shadowCoords.w;

			// used to lower moire pattern and self-shadowing
			shadowCoordinateWdivide.z += 0.0005;
	
			float distanceFromLight = texture2D(u_shadowMap, shadowCoordinateWdivide.st).z;
	
			float shadow = 1.0;
			if (io_shadowCoords.w > 0.0)
				shadow = distanceFromLight < shadowCoordinateWdivide.z ? 0.5 : 1.0;

			// define the color material
			vec4 diffuse = u_lightDiffuse * texture2D(u_texDiffuse, io_texCoords.xy);

			finalColor += attenuation * shadow * (diffuse * NdotL);
	
			// specular
			vec3 eyeVec = normalize(io_eyeVec);
			vec3 reflectVec = reflect(-lightVec, normal);

			float specular = pow(max(dot(reflectVec, eyeVec), 0.0), u_shininess);

			finalColor += u_specular * specular * attenuation * shadow;
		}
	}

	gl_FragColor = finalColor;
}
