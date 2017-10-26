// fragment program
#version 110

uniform sampler2D u_cloud;
uniform sampler2D u_noise;			// Perlin noise centered on 0.5 in [0:1.0]
uniform sampler2D u_normal;

uniform vec3 u_lightDirection;		// Direction from camera to the light source
uniform vec3 u_cloudColor;

// The vector 'noiseParams' contains information used to generate a high frequency noise:
// 0 - first frequency of the noise
// 1 - second frequency of the noise
// 2 - third frequency of the noise
// 3 - amplitude of the second frequency
uniform vec4 u_noiseParams;

// The vector 'intensityParams' contains information used to compute the intensity of the cloud, that is its transparency
// 0 - a positive coefficient in the exponential decrease
// 1 - a real coefficient used to scale the intensity of noise
// 2 - a real coefficient used to scale the initial cloud intensity
// 3 - not used
uniform vec4 u_intensityParams;

// The vector 'colorParams' contains information used to control how cloud color changes with intensity, that is its thickness
// 0 - the first parameters in the hermite interpolation below which the intensity has no effect on the color.
//      A very slim cloud won't affect the cloud color.
// 1 - the second parameters in the hermite interpolation above which the cloud color is black
// 2 - quantity of noise add on color (in [0;1])
// 3 - coefficient that defines how much the init cloud color can be changed (in [0;1] default 1)
uniform vec4 u_colorParams;
uniform vec4 u_lightParams;

varying vec2 io_texCoords;

varying vec4 io_fragPosition;		// Position of the processed fragment from the camera (in km)
varying float io_occluded;

#ifdef ALPHA_TEST_REF
void alphaTest(float alpha)
{
#ifdef ALPHA_FUNC_GREATER
	if (alpha <= ALPHA_TEST_REF)
		discard;
#endif
#ifdef ALPHA_FUNC_LESS
	if (alpha >= ALPHA_TEST_REF)
		discard;
#endif
}
#endif

void main()
{
	// Generates a high frequency perlin noise from a perlin texture
	float noise1 = texture2D(u_noise, u_noiseParams[0] * io_texCoords).r - 0.5;
	float noise2 = texture2D(u_noise, u_noiseParams[1] * io_texCoords).r - 0.5;
	float noise3 = texture2D(u_noise, u_noiseParams[2] * io_texCoords).r - 0.5;
	
	// A little less noise on intensity
	float noiseIntensity = noise1 + u_noiseParams[3]*noise2;
	float noiseColor = noiseIntensity + u_noiseParams[3]*u_noiseParams[3]*noise3;
	
	// The farthest clouds are, the less visible they are.
	float blend = exp(- u_intensityParams[0] * length(io_fragPosition));
	
	// Get and compute the cloud intensity by adding noise
	float intensity = texture2D(u_cloud, io_texCoords).r;
	intensity = (u_intensityParams[1] * noiseIntensity + u_intensityParams[2] * intensity) * blend;
	
	#ifdef ALPHA_TEST_REF
	alphaTest(intensity);
	#endif
	
	// Modify the color of the cloud based on its thickness called here 'intensity'
	vec3 color = (1.0 - u_colorParams[3]*mix(smoothstep(u_colorParams[0], u_colorParams[1], intensity), noiseColor, u_colorParams[2])) * io_occluded * u_cloudColor;
	
	// Retrieve the normal
	vec4 normal = 2.0 * texture2D(u_normal, io_texCoords) - 1.0;
	
	// Lightning
	color *= u_lightParams[0] + u_lightParams[1] * smoothstep(u_lightParams[2], u_lightParams[3], dot(u_lightDirection, normal.xyz));
	
	gl_FragColor = vec4(color.r, color.g, color.b, intensity);
}