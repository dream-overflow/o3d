// Process the infinite directionnal light on the GBuffer
#version 450

precision highp float;

uniform vec2 u_screenSize;

uniform vec3 u_lightDir;
uniform vec4 u_lightDiffuse;
uniform vec4 u_lightSpecular;
uniform vec3 u_eyeVec;

uniform sampler2D u_diffuseMap;
uniform sampler2D u_specularMap;
uniform sampler2D u_positionMap;
uniform sampler2D u_normalMap;
//uniform sampler2D u_depthMap;

layout(location = 0) out vec4 o_finalColor;

vec2 getTexCoord()
{
    return gl_FragCoord.xy / u_screenSize;
}

vec4 lighting(vec3 pos, vec3 normal, vec4 diffuse, vec4 specular, float shine)
{
    vec3 lightDir = -u_lightDir; // normalized

	float NdotL = clamp(dot(normal, lightDir), 0.0, 1.0);

    if (NdotL <= 0.0)
        discard;

    // diffuse
    vec4 diff = NdotL * diffuse * u_lightDiffuse;

    // specular
    vec3 reflectVec = normalize(reflect(-lightDir, normal));
    vec3 dirToEye = normalize(u_eyeVec - pos);

    float specularIntensity = pow(max(dot(reflectVec, dirToEye), 0.0), shine);

    vec4 spec = specularIntensity * u_lightSpecular * specular;

    return diff+spec;
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
