//OpenGL Bible Sec 3 - Passing data to vertex shader and from stage to stage

#version 450 core

layout (location = 0) in vec4 a_offset;
layout (location = 1) in vec4 a_color;

//Interface block sb7 ch3 pg 101 approx (listing 3.5)
//use to pass data from shader stage to shader stage
out VS_OUT
{
    vec4 color;     // Send color to the next stage
} vs_out;

/*
same as:
out vec4 vs_color
*/

void main(void)
{
    // Declare a hard-coded array of positions
     const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),
                                      vec4(-0.25, -0.25, 0.5, 1.0),
                                      vec4(0.25, 0.25, 0.5, 1.0));

      // Index into our array using gl_VertexID - is incremented for each run
     gl_Position = vertices[gl_VertexID] + a_offset;
     vs_out.color = a_color;
}