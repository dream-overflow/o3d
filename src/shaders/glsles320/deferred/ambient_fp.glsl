// process the ambient on the GBuffer
#version 320 es

precision highp float;

uniform vec2 u_screenSize;

uniform vec4 u_ambient;

uniform sampler2D u_ambientMap;
//uniform sampler2D u_diffuseMap;

layout(location = 0) out vec4 o_finalColor;

vec2 getTexCoord()
{
    return gl_FragCoord.xy / u_screenSize;
}

void main()
{
	vec4 ambiant = texture(u_ambientMap, getTexCoord()) * u_ambient;

	// we discard empty texels
	if (ambiant.a == 0.0)
		discard;

    // color map
	o_finalColor = ambiant;
}
