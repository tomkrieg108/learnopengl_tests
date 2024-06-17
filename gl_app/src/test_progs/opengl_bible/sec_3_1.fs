//OpenGL Bible Sec 3 - Passing data to vertex shader and from stage to stage

#version 450 core

in VS_OUT
{
    vec4 color;     // Send color to the next stage
} fs_in;

out vec4 color;
void main(void)
{
    //color = vec4(0.0, 0.8, 1.0, 1.0);
    color = fs_in.color;
}