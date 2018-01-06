// fond 2d fragment program
#version 450

precision highp float;

uniform vec4 u_ambient;
uniform sampler2D u_ambientMap;

layout(location = 0) smooth in vec2 io_texCoords1;

layout(location = 0) out vec4 o_finalColor;

void main()
{
    o_finalColor = u_ambient * texture(u_ambientMap, io_texCoords1.xy).r;
}
