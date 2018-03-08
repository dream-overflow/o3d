// GBuffer color and depth rendering
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

precision highp float;

layout(location = 4) smooth in vec2 io_texCoords1;

uniform sampler2D u_colorMap;
uniform sampler2D u_depthMap;

layout(location = 0) out vec4 o_finalColor;

void main()
{
	o_finalColor = texture(u_colorMap, io_texCoords1.xy);

    // depth map
	float depth = texture(u_depthMap, io_texCoords1.xy).r;
    gl_FragDepth = depth;
}
