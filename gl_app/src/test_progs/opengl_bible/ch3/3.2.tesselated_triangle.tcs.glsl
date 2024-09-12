
//OpenGL bible sec 3 - tessellation control shader
//set the level of tesselation, generate data to be sent to the tesselation engine
//Tesselation breaks a 'patch' (e.g. a quad or large triangle) into smaller triangles, lines or points
//Runs once per patch?.
//Receives it's input as an array with size equal to number of vertices per patch
//gl_InvocationID is 0 based index to the vertex in the patch

#version 450 core
layout(vertices = 3) out;

void main(void)
{
    // Only if I am invocation 0 ...
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = 5.0;
        
        gl_TessLevelOuter[0] = 5.0;
        gl_TessLevelOuter[1] = 5.0;
        gl_TessLevelOuter[2] = 5.0;
    }
    // Everybody copies their input to their output
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
} 

//sb7 listing 3.7
//See also learnopergl.com tesselation chapter for explanation
