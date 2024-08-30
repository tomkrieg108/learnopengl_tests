//OpenGL Bible sec 2 - drawing a triangle

#version 450 core

void main()
{
    //All variables starting with gl_ are built into GLSL and connect shders to each other
    //gl_Position is the coord of vertex outpout in clip space (before perspective divide i.e. div by z)
    // x,y,z => [-1,1]

    //gl_Position = vec4(0.0, 0.0, 1.5, 2.0); // for a point
    gl_Position = vec4(0.0, 0.0, -1.0f, 1.0); // for a point - point still displayed in this case

    // Declare a hard-coded array of positions
    /*
     const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),
                                      vec4(-0.25, -0.25, 0.5, 1.0),
                                      vec4(0.25, 0.25, 0.5, 1.0));
                                      */

      // Index into our array using gl_VertexID - is incremented for each run of the vertex shader see sb7 loc 1969
      //counts up from value given in call to glDrawArrays
     //gl_Position = vertices[gl_VertexID];
}