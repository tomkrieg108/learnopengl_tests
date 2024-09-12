//OpenGL Bible Sec 3 - Passing data to vertex shader and from stage to stage

#version 450 core

//sb7 listing 3.6

in VS_OUT //this needs to match name of interface block given in vertex shader
{
    vec4 color;     // receive color from previous stage
} fs_in;

/*
same as:
in vec4 vs_color
*/

out vec4 color;
void main(void)
{
    color = fs_in.color;
}