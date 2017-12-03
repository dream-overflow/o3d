// GBuffer color and depth rendering
#version 330

precision highp float;

smooth in vec2 io_texCoords1;

uniform sampler2D u_colorMap;
uniform sampler2D u_depthMap;

out vec4 o_finalColor;

void main()
{
	o_finalColor = texture(u_colorMap, io_texCoords1.xy);

    // depth map
	float depth = texture(u_depthMap, io_texCoords1.xy).r;
    gl_FragDepth = depth;
}
