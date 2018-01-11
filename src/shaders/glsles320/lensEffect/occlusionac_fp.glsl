// Minimalistic fragment progam that wrote nothing in color.
#version 320 es

precision highp float;

// enable early depth test
layout(early_fragment_tests) in;

// and support an atomic counter for counting passing fragments
layout(binding = 0, offset = 0) uniform atomic_uint visibles;

void main()
{
    atomicCounterIncrement(visibles);
}
