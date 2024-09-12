//Passthrough shader that converts triangles into points
//runs once per input primitive (int this case per triangle)
//output of geom shader is similar to vertex shader

#version 450 core

layout (triangles) in; //expecting triangles as inputs
layout (points, max_vertices = 3) out; //produce points - max produces is 3

void main(void)
{
    int i;
    for (i = 0; i < gl_in.length(); i++) //gl_in[] array length will be 3 (processing triangles)
        {
            gl_Position = gl_in[i].gl_Position;
            EmitVertex();
        }
}