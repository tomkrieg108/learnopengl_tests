//OpenGL Bible sec 2 - drawing a triangle

#version 450 core

void main()
{
    //All variables starting with gl_ are built into GLSL and connect shders to each other
    //gl_Position is the coord of vertex outpout in clip space (before perspective divide i.e. div by z)
    // x,y,z => [-1,1]

    //NOTE: ACCORDING TO OPENgl BIBLE BOOK (Cha 3 - Clipping & Viewport Transformation), Z IS IN [0,1] DOESN'T APPEAR TO BE THE CASE WHEN CHECKING HERE
    //Apparently in viewport space it changes to [0,1]
    //Also checlout glDepthRange
    //https://www.reddit.com/r/opengl/comments/grryir/confusion_about_the_range_of_ndc/
    //https://www.songho.ca/opengl/gl_transform.html

    //gl_Position = vec4(0.0, 0.0, 1.5, 2.0); // for a point
    gl_Position = vec4(0.0, 0.0, -1.0f, 1.0); // for a point - point still displayed in this case

}