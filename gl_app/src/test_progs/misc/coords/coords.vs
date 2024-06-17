
#version 450 core

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec4 a_col;

out vec4 col;

uniform mat4 u_model, u_view, u_proj;

void main()
{
	gl_Position = u_proj * u_view * u_model * vec4(a_pos, 1.0);
	col = a_col;
};

