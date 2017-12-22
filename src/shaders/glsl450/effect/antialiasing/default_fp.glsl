// anti-aliasing post effect
#version 450

precision highp float;

uniform vec2 u_screenSize;

uniform vec2 u_pixelSize;
uniform float u_weight;

uniform sampler2D u_depthMap;
uniform sampler2D u_normalMap;
uniform sampler2D u_colorMap;

out vec4 o_finalColor;

vec2 getTexCoord()
{
	return gl_FragCoord.xy / u_screenSize;
}
/*
vec2 getTexCoord(vec2 p)
{
	return (gl_FragCoord.xy + p) / u_screenSize;
}

const vec2 offsets[9] = vec2[9](
	vec2( 0, 0),  // center
	vec2(-1,-1),  // top-left
	vec2( 0,-1),  // top
	vec2( 1,-1),  // top-right
	vec2( 1, 0),  // right
	vec2( 1, 1),  // bottom-right
	vec2( 0, 1),  // bottom
	vec2(-1, 1),  // bottom-left
	vec2(-1, 0)   // left
);

float getDepth(vec2 uv)
{
	return texture(u_depthMap, uv).x;
}

vec3 getNormal(vec2 uv)
{
	return texture(u_normalMap, uv).xyz;
}

float getEdgeWeight(vec2 screenPos)
{
    float depth[9];
    vec3 normal[9];

    // retrieve normal and depth data for all neighbors
    for(int i = 0; i < 9; ++i)
    {
        vec2 uv = screenPos + offsets[i] * u_pixelSize;
        depth[i] = getDepth(uv);
        normal[i]= getNormal(uv);
    }

    // compute deltas in depth
    vec4 deltas1;
    vec4 deltas2;

    deltas1.x = depth[1];
    deltas1.y = depth[2];
    deltas1.z = depth[3];
    deltas1.w = depth[4];
    deltas2.x = depth[5];
    deltas2.y = depth[6];
    deltas2.z = depth[7];
    deltas2.w = depth[8];

    // compute absolute gradients from center
    deltas1 = abs(deltas1 - depth[0]);
    deltas2 = abs(depth[0] - deltas2);

    // find min and max gradient, ensuring min != 0
    vec4 maxDeltas = max(deltas1, deltas2);
    vec4 minDeltas = max(min(deltas1, deltas2), 0.00001);

    // Compare change in gradients, flagging ones that change significantly.
    // How severe the change must be to get flagged is a function of the
    // minimum gradient. It is not resolution dependent. The constant
    // number here would change based on how the depth values are stored
    // and how sensitive the edge detection should be.
    vec4 depthResults = step(minDeltas * 25.0, maxDeltas);

    // compute change in the cosine of the angle between normals
    deltas1.x = dot(normal[1], normal[0]);
    deltas1.y = dot(normal[2], normal[0]);
    deltas1.z = dot(normal[3], normal[0]);
    deltas1.w = dot(normal[4], normal[0]);
    deltas2.x = dot(normal[5], normal[0]);
    deltas2.y = dot(normal[6], normal[0]);
    deltas2.z = dot(normal[7], normal[0]);
    deltas2.w = dot(normal[8], normal[0]);
    deltas1 = abs(deltas1 - deltas2);

    // Compare change in the cosine of the angles, flagging changes
    // above some constant threshold. The cosine of the angle is not a
    // linear function of the angle, so to have the flagging be
    // independent of the angles involved, an arccos function would be
    // required.
    vec4 normalResults = step(0.4, deltas1);
    normalResults = max(normalResults, depthResults);

    return (normalResults.x + normalResults.y +
            normalResults.z + normalResults.w) * 0.25;
}

void main()
{
    vec2 texCoord = getTexCoord();
    float w = getEdgeWeight(texCoord) * u_weight;

    if (w < 0.0001)
    {
        o_finalColor = texture(u_colorMap, texCoord);
        return;
    }

    // Smoothed color
    // (a-c)*w + c = a*w + c(1-w)
    vec2 offset = texCoord * (1.0 - w);

    vec4 s0 = texture(u_colorMap, offset + (offsets[1]*u_pixelSize) * w);
    vec4 s1 = texture(u_colorMap, offset + (offsets[3]*u_pixelSize) * w);
    vec4 s2 = texture(u_colorMap, offset + (offsets[5]*u_pixelSize) * w);
    vec4 s3 = texture(u_colorMap, offset + (offsets[7]*u_pixelSize) * w);

    o_finalColor = (((s0 + s1 + s2 + s3) / 4.0));
}
*/

void main()
{
	const vec2 delta[8] = vec2[8](
		vec2(-1,1), vec2(1,-1), vec2(-1,1),  vec2(1,1),
		vec2(-1,0), vec2(1,0), vec2(0,-1), vec2(0,1)
	);

	vec2 texCoord = getTexCoord();
	vec3 normal = texture(u_normalMap, texCoord).xyz;
	float factor = 0.0;

	for (int i = 0; i < 4; ++i)
	{
		vec3 t = texture(u_normalMap, texCoord + delta[i]*u_pixelSize).xyz;
		t -= normal;
		factor += dot(t,t);
	}

	factor = min(1.0, factor) * u_weight;

	vec4 color = vec4(0);

	for (int i = 0; i < 8; ++i)
	{
        color += texture(u_colorMap, texCoord + delta[i]*u_pixelSize*factor);
	}

	color += 2.0 * texture(u_colorMap, texCoord);

	o_finalColor = color * (1.0/10.0);
}

