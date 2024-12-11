//From OGLDEV youtube
#version 330

uniform mat4 u_VP; // view-proj mat (already in world coords)
uniform float u_grid_size = 100.0;
uniform vec3 u_camera_world_pos;

out vec3 world_pos; //world space pos of vertex

const vec3 pos[4] = vec3[4]
(
  vec3(-1.0, 0.0, -1.0), //bottom Left
  vec3( 1.0, 0.0, -1.0), //bottom right
  vec3( 1.0, 0.0,  1.0), //top right
  vec3(-1.0, 0.0,  1.0) //top left
);

const int indices[6] = int[6](0,2,1,2,0,3);

void main()
{
  int index = indices[gl_VertexID];

  vec3 vpos3 = pos[index] * u_grid_size;
  vpos3.x += u_camera_world_pos.x;
  vpos3.z += u_camera_world_pos.z;

  vec4 vpos4 = vec4(vpos3,1);
  gl_Position = u_VP * vpos4;

  world_pos = vpos3;

}