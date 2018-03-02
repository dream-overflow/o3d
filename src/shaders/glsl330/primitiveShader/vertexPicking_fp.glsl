// Simple picking fragment program
#version 330

layout(location = 0) in flat uint io_picking;
layout(location = 0) out uint o_picking;  // r

void main()
{
    o_picking = io_picking;
}
