/**
 * Basic vertex program.
 */

<< modelViewProjection()
@uniform mat4 u_modelViewProjectionMatrix;
@in vec4 a_vertex;

@{
void modelViewProjection()
{
    gl_Position = u_modelViewProjectionMatrix * a_vertex;
}
@}
>>

<< main()
#version 130

void main()
{
    @content
}
>>
