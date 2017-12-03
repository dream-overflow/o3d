// fond 2d fragment program
#version 330

precision highp float;

uniform vec4 u_ambient;
uniform sampler2D u_ambientMap;

smooth in vec2 io_texCoords1;

out vec4 o_finalColor;

void main()
{
    o_finalColor = u_ambient * texture(u_ambientMap, io_texCoords1.xy).r;
}
