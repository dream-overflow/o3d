// Process the spot light on the GBuffer
#version 450

precision highp float;

uniform vec2 u_screenSize;

uniform vec4 u_lightPos;
uniform vec3 u_lightDir;
uniform vec4 u_lightDiffuse;
uniform vec4 u_lightSpecular;
uniform vec3 u_lightAtt;
uniform float u_lightCosCutOff;
uniform float u_lightExponent;
uniform vec3 u_eyeVec;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_positionMap;
uniform sampler2D u_normalMap;
//uniform sampler2D u_depthMap;

out vec4 o_finalColor;

vec2 getTexCoord()
{
    return gl_FragCoord.xy / u_screenSize;
}

vec4 lighting(vec3 pos, vec3 normal, vec4 diffuse, vec4 specular, float shine)
{
	vec3 lightToVertex = u_lightPos.xyz - pos;
	vec3 lightVec = normalize(lightToVertex);

	float DdotL = dot(-lightVec, u_lightDir);

    if (DdotL <= u_lightCosCutOff)
        discard;

	float spotIntensity = pow((clamp(DdotL, u_lightCosCutOff, 1.0) -
       	u_lightCosCutOff) / (1.0 - u_lightCosCutOff), u_lightExponent);

	float NdotL = clamp(dot(normal, lightVec), 0.0, 1.0);

	NdotL = NdotL * spotIntensity;

	if (NdotL <= 0.0)
        discard;

	// distance beetwen the light and the fragment
	float dist = length(lightToVertex);

	// quadratic attenuation
	float attenuation = 1.0 / (u_lightAtt[0] + u_lightAtt[1] * dist + u_lightAtt[2] * dist * dist);

	// diffuse
    vec4 diff = NdotL * diffuse * u_lightDiffuse;

    // specular
    vec3 dirToEye = normalize(u_eyeVec - pos);
    vec3 reflectVec = reflect(u_lightDir.xyz, normal);

    float specularIntensity = pow(max(dot(reflectVec, dirToEye), 0.0), shine);

    vec4 spec = u_lightSpecular * specular * specularIntensity;

    return (diff + spec) * attenuation;
}

void main()
{
    vec2 texCoord = getTexCoord();

    vec3 pos = texture(u_positionMap, texCoord).rgb;

    vec4 normalShine = texture(u_normalMap, texCoord);
    vec3 normal = normalize(normalShine.xyz);

	vec4 diffuse = texture(u_diffuseMap, texCoord); 
	vec4 specular = texture(u_specularMap, texCoord);

    // shine in specular A
    float shine = normalShine.a;

	o_finalColor = lighting(pos, normal, diffuse, specular, shine);
}

