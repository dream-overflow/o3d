// default picking fragment program
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) uniform uint u_picking;

#ifdef OPACITY_MAP
layout(location = 4) smooth in vec2 io_texCoords1;
uniform sampler2D u_opacityMap;
#endif

layout(location = 0) out uint o_final;

void main()
{
    // Calculating The Final Color
#ifdef OPACITY_MAP
    o_final = floor(texture(u_opacityMap, io_texCoords1).r) * u_picking;
#else
    o_final = u_picking;
#endif
}
