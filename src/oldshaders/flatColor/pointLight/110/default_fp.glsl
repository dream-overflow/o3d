// default fragment program
#version 110

varying vec3 io_normal;
varying vec3 io_eyeVec;
varying vec3 io_vertex;

uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_specular;

uniform float u_shininess;

uniform vec4 u_lightPos;
uniform vec3 u_lightAtt;

void main()
{
	vec3 lightToVertex = u_lightPos.xyz - io_vertex;
	vec3 lightVec = normalize(lightToVertex);

	vec3 normal = normalize(io_normal);

	float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

	vec4 finalColor = u_ambient;

	if (NdotL > 0.0)
	{
		// distance beetwen the light and the fragment
		float dist = length(lightToVertex);

		// quadratic attenuation
		float attenuation = 1.0 / (u_lightAtt[0] + u_lightAtt[1] * dist + u_lightAtt[2] * dist * dist);
	
		// ambient and diffuse
  		finalColor += attenuation * (u_diffuse * NdotL);	

		// specular
		vec3 eyeVec = normalize(io_eyeVec);
		vec3 reflectVec = reflect(-lightVec, normal);

		float specular = pow(max(dot(reflectVec, eyeVec), 0.0), u_shininess);

		finalColor += u_specular * specular * attenuation;
	}

	gl_FragColor = finalColor;	
}
