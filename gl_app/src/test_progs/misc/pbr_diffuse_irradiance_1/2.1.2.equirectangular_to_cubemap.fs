#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform sampler2D equirectangularMap;

//since image is flipped i.e. stbi_set_flip_vertically_on_load(true) u,v = (0.0) is at bottom left of image instead of top lef.  As such v = (phi/PI + 0.5) instead of (-phi/PI + 0.5).  If dont't flip image, then need to use the latter mapping => use -0.3183 as second component in invAtan.  Get the same result.
const vec2 invAtan = vec2(0.1591, 0.3183); //(1/2Pi, 1/Pi)
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y)); // u:[-PI, PI], v:[-PI/2, PH/2]
    uv *= invAtan; 
    uv += 0.5; // u:[0,1], v: [0,1]
    return uv;
}

void main()
{		
    vec2 uv = SampleSphericalMap(normalize(WorldPos));
    vec3 color = texture(equirectangularMap, uv).rgb;
    
    FragColor = vec4(color, 1.0);
}

/*
From comments in 'Diffuse Irradiance' chapter

also look at https://en.wikipedia.org/wiki/UV_mapping

If someone is interested how this trigonometry magic below works

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
uv *= invAtan;
uv += 0.5;
return uv;
}

I can add a few notes here:
The idea is to sample that odd image at the right spot, so it can be mapped on cubemap faces.
The odd image is a mapping of points from a sphere on uv-space.
It's possible to project points from a sphere onto a cubemap.
So if we're able to map our flat image back onto a sphere and then project from the sphere to our cubemap - the job is done.
Note that it can be done by going from the other end. Having a point on a cubemap we can project it onto a sphere and then map from the sphere onto our uv-space. This is the approach used in this tutorial.

Mapping from cubemap to a sphere is trivial - you just normalize the position vector that points a point on your cubemap face. This is done before calling SampleSphericalMap:


normalize(localPos)

To map from a sphere onto uv-space, we first project from a sphere onto cylinder border, and then cut and unwrap the cylinder.
Having a point on a sphere you can find phi and theta angles and using them you can find a projected point on a cylinder.
This line of code finds you phi and theta angles correspondingly:

vec2 uv = vec2(atan(v.z, v.x), asin(v.y));

(also, this was new to me, atan overloaded version that takes 2 parameters is able to detect the quadrant the angle is in ! the range of this atan is [-Pi, Pi] and not [-Pi/2,Pi/2] as in math)
(remember that Y is up, not Z, like in most formulas on the internet for spherical/cylindrical coordinates)
Mapping to cylinder and unwrapping is more like step for better visualisation, technically it's not needed since what matters is angles and for point on a sphere and the same point but projected on cylinder the angles are the same !
So, you can go directly map your phi and theta angles in range between 0 and 1 and call it a day, you'll get a point on uv-space projected from a sphere which in it's turn projected from a cubemap.
Not sure why it's named invAtan

const vec2 invAtan = vec2(0.1591, 0.3183);

because this is basically {1/(2Pi), 1/Pi} the coefficients used for linear mapping from your phi and theta angles to range 0 to 1 (or in other words - the slope of your line).
SampleSphericalMap mathematically looks like this:

u = (1/(2Pi)) * phi + 1/2
v = (1/Pi) * theta + 1/2

Just a basic linear maping from [0,2Pi] to [0,1] for U and [-Pi/2,Pi/2] to [0,1] for V.

This comment is messy, but I hope it can provide some bits and pieces for those interested why that code works.

*/