// Simple picking fragment program
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 0) uniform uint u_picking;
// layout(location = 0) in flat uint io_picking;
layout(location = 0) out uint o_picking;  // r

void main()
{
    // o_picking = io_picking;
    o_picking = u_picking;
}
