// default fragment program
#version 110

uniform sampler2D u_texDiffuse;

uniform vec4 u_lightDiffuse;

varying vec2 io_texCoords;

varying vec3 io_normal;
varying vec3 io_eyeVec;

uniform vec4 u_ambient;
uniform vec4 u_specular;

uniform vec3 u_lightDir;
uniform float u_shininess;

void main()
{
	// Scaling The Input Vector To Length 1
	vec3 normal = normalize(io_normal);
	float NdotL = clamp(dot(normal, -u_lightDir), 0.0, 1.0);

	vec4 finalColor = u_ambient;
	
	if (NdotL > 0.0)
	{
		// define the color material
		vec4 diffuse = u_lightDiffuse * texture2D(u_texDiffuse, io_texCoords.xy);

		finalColor += diffuse * NdotL;

		// specular
		vec3 eyeVec = normalize(io_eyeVec);
		vec3 reflectVec = reflect(u_lightDir, normal);

		float specular = pow(max(dot(reflectVec, eyeVec), 0.0), u_shininess);

		finalColor += u_specular * specular;		
	}

	gl_FragColor = finalColor;	
}
