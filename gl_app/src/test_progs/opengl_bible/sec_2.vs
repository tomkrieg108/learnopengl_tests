//OpenGL Bible sec 2 - drawing a triangle

#version 450 core
 void main(void)
{
    //gl_Position = vec4(0.0, 0.0, 0.5, 1.0); // for a point

    // Declare a hard-coded array of positions
     const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),
                                      vec4(-0.25, -0.25, 0.5, 1.0),
                                      vec4(0.25, 0.25, 0.5, 1.0));

      // Index into our array using gl_VertexID - is incremented for each run
     gl_Position = vertices[gl_VertexID];
}