// default fragment program
#version 110

varying vec3 io_normal;
varying vec3 io_eyeVec;

uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_specular;

uniform float u_shininess;

uniform vec3 u_lightDir;

void main()
{
	vec3 normal = normalize(io_normal);
	
	float NdotL = clamp(dot(normal, -u_lightDir), 0.0, 1.0);

	vec4 finalColor = u_ambient;

	if (NdotL > 0.0)
	{
		// ambient and diffuse
  		finalColor += u_diffuse * NdotL;	

		// specular
		vec3 eyeVec = normalize(io_eyeVec);
		vec3 reflectVec = reflect(u_lightDir, normal);

		float specular = pow(max(dot(reflectVec, eyeVec), 0.0), u_shininess);

		finalColor += u_specular * specular;
	}

	gl_FragColor = finalColor;	
}
