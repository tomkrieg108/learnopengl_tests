//From OGLDEV youtube
#version 330

in vec3 world_pos; //world space pos of vertex
out vec4 frag_color;

uniform float u_grid_min_pixels_between_cells = 2.0f;
//uniform float u_grid_cell_size = 0.025;
uniform float u_grid_cell_size = 0.05;
uniform vec4 u_grid_color_thin = vec4(0.5,0.5,0.5,1.0);
uniform vec4 u_grid_color_thick = vec4(0.0,0.0,0.0,1.0);
uniform vec3 u_camera_world_pos;

float log10(float x)
{
  float f = log(x) / log(10.0);
  return f;
}

vec2 satv(vec2 val)
{
  vec2 res;
  res.x = clamp(val.x, 0.0, 1.0);
  res.y = clamp(val.y, 0.0, 1.0);
  return res;
}

float satf(float val)
{
  return clamp(val, 0.0, 1.0);
}

float max2(vec2 vec)
{
  if(abs(vec.x) > abs(vec.y))
    return abs(vec.x);
  return abs(vec.y);  
}

void main()
{
  vec2 dvx = vec2(dFdx(world_pos.x), dFdy(world_pos.x));
  vec2 dvy = vec2(dFdx(world_pos.z), dFdy(world_pos.z));
  float ly = length(dvy);
  float lx = length(dvx);
  vec2 dudv = vec2(lx,ly);
  float l = length(dudv);
  float LOD = max(0.0, log10(l * u_grid_min_pixels_between_cells / u_grid_cell_size) + 1.0);
  float grid_cell_size_lod0 = u_grid_cell_size * pow(10.0, floor(LOD));
  float grid_cell_size_lod1 =  grid_cell_size_lod0 * 10; 
  float grid_cell_size_lod2 =  grid_cell_size_lod1 * 10; 

  dudv *= 4.0f;

  //float Lod0a = mod(world_pos.z, u_grid_cell_size) / ly;
  //float Lod0a = mod(world_pos.z, u_grid_cell_size) / (dFdy(world_pos.z)*4.0f);
  //float Lod0a = mod(world_pos.z, u_grid_cell_size);

  //float Lod0a = mod(world_pos.x, u_grid_cell_size) / (dFdx(world_pos.x)*4.0f);

  //float Lod0a = max2(mod(world_pos.x, u_grid_cell_size) / dudv));

  //float Lod0a = max2(vec2(1.0) - mod(world_pos.x, u_grid_cell_size) / dudv);

  //vec2 mod_div_dudv = mod(world_pos.xz, u_grid_cell_size) / dudv;
  vec2 mod_div_dudv = mod(world_pos.xz, grid_cell_size_lod0) / dudv;
  float Lod0a = max2(vec2(1.0) - abs(satv(mod_div_dudv)*2.0f - vec2(1.0f)));
  //float Lod0a = max2(vec2(1.0) - abs((mod_div_dudv)*2.0f - vec2(1.0f)));

  mod_div_dudv = mod(world_pos.xz, grid_cell_size_lod1) / dudv;
  float Lod1a = max2(vec2(1.0) - abs(satv(mod_div_dudv)*2.0f - vec2(1.0f)));
  //float Lod1a = max2(vec2(1.0) - abs((mod_div_dudv)*2.0f - vec2(1.0f)));

  mod_div_dudv = mod(world_pos.xz, grid_cell_size_lod2) / dudv;
  float Lod2a = max2(vec2(1.0) - abs(satv(mod_div_dudv)*2.0f - vec2(1.0f)));
  //float Lod2a = max2(vec2(1.0) - abs((mod_div_dudv)*2.0f - vec2(1.0f)));

  float LOD_fade = fract(LOD); //fractional part of LOD

  vec4 color;

  if(Lod2a > 0.0) {
    color = u_grid_color_thick;
    color.a *= Lod2a;
  }
  else if (Lod1a > 0.0) {
    color = mix(u_grid_color_thick, u_grid_color_thin, LOD_fade);
    color.a *= Lod1a;
  }
  else {
    color = u_grid_color_thin;
    color.a *= (Lod0a * (1.0 - LOD_fade));
  }

  float opacity_falloff = (1.0 - satf(length(world_pos.xz - u_camera_world_pos.xz) / u_grid_cell_size));

  color.a *= opacity_falloff;

  frag_color = color;
}