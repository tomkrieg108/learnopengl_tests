
//Vertex shader runs once per control point (essentially once per vertex)
// Results passed to TCS in batch of vertices equal to the number of control points - in this case, 3
#version 450 core
 void main(void)
{
    // Declare a hard-coded array of positions
     const vec4 vertices[3] = vec4[3](vec4(0.25, -0.25, 0.5, 1.0),
                                      vec4(-0.25, -0.25, 0.5, 1.0),
                                      vec4(0.25, 0.25, 0.5, 1.0));

      // Index into our array using gl_VertexID - is incremented for each run
     gl_Position = vertices[gl_VertexID];
}