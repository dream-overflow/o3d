// vertex program
#version 330

uniform mat4 u_modelViewProjectionMatrix;

uniform float u_invScale;				// Scale is ratio of cloud texture size over the dome length (in [0;1])
uniform float u_time;					// Time in second
uniform float u_planetRadius;			// Planet radius (same unit as <a_vertex>) (0.0 if occlusion is disabled)
uniform float u_layerAltitude;			// Altitude of the cloud layer (same unit as <u_planetRadius>)
uniform vec3 u_lightDirection;			// Direction from camera to the light source

uniform vec2 u_velocity;				// Translation velocity of clouds (in <texture coordinates>/sec)

layout(location = 0) in vec4 a_vertex;
layout(location = 5) in vec2 a_texCoords;

layout(location = 0) smooth out vec4 io_fragPosition;  // Position of the processed fragment
layout(location = 1) smooth out vec2 io_texCoords;
layout(location = 2) smooth out float io_occluded;  // Occlusion rate (in [0;1] where 0 means totaly occluded)

void main()
{
	gl_Position = u_modelViewProjectionMatrix * a_vertex;

	io_fragPosition = a_vertex;
	io_texCoords = u_invScale*a_texCoords + u_time*u_velocity;
	
	if (u_lightDirection.y < 0.0)
	{
		vec3 position = vec3(0.0, u_planetRadius, 0.0) + a_vertex.xyz;
		vec3 normal = cross(u_lightDirection, position);
		vec3 s = normalize(cross(normal, u_lightDirection));
			
		// io_occluded = step(u_planetRadius, dot(position, s));
		io_occluded = smoothstep(u_planetRadius, u_planetRadius + u_layerAltitude, dot(position, s));
	}
	else
	{
		io_occluded = 1.0;
	}
}
