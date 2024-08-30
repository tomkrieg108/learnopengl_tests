//https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map


#version 450 core

out vec4 FragColor;

in float Height;

void main()
{
    float h = (Height + 16)/64.0f;	// shift and scale the height in to a grayscale value
    FragColor = vec4(h, h, h, 1.0);
}