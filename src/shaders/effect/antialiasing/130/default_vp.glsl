// anti-aliasing post effect
#version 130

uniform mat4 u_modelViewProjectionMatrix;

in vec4 a_vertex;

void main()
{
    gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
