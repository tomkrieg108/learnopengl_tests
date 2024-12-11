#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;

out vec3 WorldPos;

void main()
{
  WorldPos = aPos;

	mat4 rotView = mat4(mat3(view)); //remove the translation component
	vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;

	//Doing this will display small cube at world origin with the hdr image mapped to each side of the cube
	//gl_Position = projection * view * vec4(WorldPos, 1.0);
}