//OpenGL Bible sec 2 - drawing a triangle

#version 450 core

out vec4 color;

//built in variable gl_FragCoord contains the position of the frag in the window

void main()
{
    color = vec4(0.0, 0.8, 1.0, 1.0);
}
