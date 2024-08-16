#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitangent;
layout(location = 5) in ivec4 boneIds; 
layout(location = 6) in vec4 weights;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

const int MAX_BONES = 100;
const int MAX_BONE_INFLUENCE = 4;
uniform mat4 finalBonesMatrices[MAX_BONES];

out vec2 TexCoords;

void main()
{
    mat4 BoneTransform = finalBonesMatrices[boneIds[0]] * weights[0];
    BoneTransform     += finalBonesMatrices[boneIds[1]] * weights[1];
    BoneTransform     += finalBonesMatrices[boneIds[2]] * weights[2];
    BoneTransform     += finalBonesMatrices[boneIds[3]] * weights[3];

    gl_Position =  projection * view * model * BoneTransform * vec4(pos, 1.0);
	TexCoords = tex;
}
