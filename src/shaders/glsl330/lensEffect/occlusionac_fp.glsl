// Minimalistic fragment progam that wrote nothing in color.
#version 330
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_shader_atomic_counters : enable

precision highp float;

// enable early depth test
layout(early_fragment_tests) in;

// and support an atomic counter for counting passing fragments
layout(binding = 0, offset = 0) uniform atomic_uint visibles;

void main()
{
    atomicCounterIncrement(visibles);
}
