
#version 450 core

out vec4 frag_color;


const int SIZE = 32;
const int ROWS = 3;

vec4 color_data[SIZE*ROWS];
vec4 position_data[SIZE*ROWS];


void SetColorData()
{
	const float SIZE_FL = float(SIZE);
	const float color_incr = 1.0f/SIZE_FL;
	
	//reverse gamma corrected (top)
	float col = color_incr;
	for(int i=0; i<SIZE; i++)
	{
		float col_rgc = pow(col,2.2f);
		color_data[i] = vec4(0.0f,0.0f,col_rgc,1);
		col += color_incr;
	}

	//linear (middle)
	col = color_incr;
	for(int i=SIZE; i<SIZE*2; i++)
	{
		color_data[i] = vec4(0.0f,0.0f,col,1);
		col += color_incr;
	}

	// gamma corrected (bottom)
	col = color_incr;
	for(int i=SIZE*2; i<SIZE*3; i++)
	{
		float col_gc = pow(col,1.0f/2.2f);
		color_data[i] = vec4(0.0f,0.0f,col_gc,1);
		col += color_incr;
	}
}


void SetPositions(float y)
{
	const float z = 0.0f;
	float pos_incr = 0.041f;

	float x = -0.5f;
	for(int i=0; i<SIZE; i++)
	{
		position_data[i] = vec4(x,y+0.1f,z,1);
		x += pos_incr;
	}

	x = -0.5f;
	for(int i=SIZE; i<SIZE*2; i++)
	{
		position_data[i] = vec4(x,y,z,1);
		x += pos_incr;
	}

	x = -0.5f;
	for(int i=SIZE*2; i<SIZE*3; i++)
	{
		position_data[i] = vec4(x,y-0.1f,z,1);
		x += pos_incr;
	}
}

void main()
{
	SetColorData();
	SetPositions(0.0f);

  gl_Position = position_data[gl_VertexID];
	frag_color = color_data[gl_VertexID];
}

/*
	For ChatGPT:

	Hello again.  Still trying to solidify my understanding of this, so I ran an little experiment in openGL!  I drew a set of horizontal points (40 pixel size to see them easily) in shades of blue with the blue RGB component incrementing linearly with each subsequent point. i.e. something like:

	const float color_incr = 1.0f/32.0f;
	col = color_incr;
	for(int i=0; i<32; i++)
	{
		color_data[i] = vec4(0.0f,0.0f,col,1);
		col += color_incr;
	}

	This produces a blue strip that seems more-or-less linear on the screen.  So if I understand this correctlsy, the linear RGB values are setup and then sent to the GPU memory where they will also be stored linearly.  When they are actually rendered on the screen, however, the monitor/graphics card applies it's gamma to these linearly stored RGB values (so the blue component effectively is raised to the power of 2.2).  So the result of this is that the physical amount of light coming from the blue strip on the screen is now non-linear (i.e.follows a curve that is 'below' the linear relation).  But my eyes perceive it as linear.

	I rendered another strip below the first where I do the following inside the for loop:

	float col_gc = pow(col,1.0f/2.2f);
	color_data[i] = vec4(0.0f,0.0f,col_gc,1);

	As I understand it, this effectively translates the blue strip into sRGB space - the RGB values stored in GPU memory are non-linear according to the power function applied - i.e. follow the power curve that is 'above' the linear relation.  When rendered, the monitor again applies it's gamma to these values, effectively cancelling the power function applied in code.  The resulting blue strip displayed on the screen is now linear in terms of the intensity of physical light emitted by the monitor, but my eyes percieve it to be too bright, and with seeemingling smaller increases in brightness towards the bright end (even though the physical increments remain constant).
	
	Now, when it comes to making an image/texture on a monitor by eye, say you 'author' a blue strip on the screen and you try to make it look to have a linear increase in intensity.  Again, Even though you perceive your strip as being linear, due to your non-linear perception of light intensity, the physical intensity of the light emitted from the strip will not be linear.  However, in GPU memory, the RGB data values of your strip will in fact be roughy linear.  When they are rendered by the monitor, the monitor applies its gamma so that the phisical intensity of your strip is non-linear, but you perceive it as linear.  Pretty much the same as the above situation where you instead set up your strip in code. If you wanted to save you 'authored' blue strip to a file (e.g. JPEG or whatever), the RGB data in the file would need to be linear so that when the file is displayed on the monitor it also appears linear after the monitors gamma is applied.

	Can you confirm this is what's happening?  

	Now to my second clarification:
	When you take a photo, the camera will record variations in light intensity detected on it's sensor linearly.  However when the image from the camera is saved in a file (JPEG or PNG etc), it will normally apply gamma 'compression' i.e. effectively apply the function pow(col,1/2.2.2) to each pixel before saving the data in the file (so the color format/space is sRGB).  When this file is then displayed on a monitor, the monitor again applies its gamma, effectively via pow(col,2.2) and brings back the image data to linear space.  So the physical intensity of light emitted by the monitor pretty much equals the physical intensity of the light originally received at the camera sensor, and also the physical amount of light that would have entered your eyes if you were looking at the scene directly.  So the image on the monitor therefore accurately displays what you would have directly perceived with your eyes if you were viewing the scene directly.  So, in this situation, the fact that your perception of physical light intensity is non-linear is really irrelevant.  It's just that the image is first encoded to roughly mimick your eyes perception which allows image tones to be stored more effeciently, then decoded by the monitor back to linear space  so that the physical intensity of the light emitted from the monitor is roughly the same as what you would have entered your eyes if you were looking directly at the scene. does my understanding of this sound about right to you? 
	
	Pretty long winded!  I hope you don't crash!

	
	Response:

Your understanding is impressively thorough, and your experiments are a fantastic way to grasp the nuances of gamma correction and color spaces. Let’s carefully analyze and confirm the points you've raised.

First Scenario: Rendering Linear vs Gamma-Encoded Strips
Linear Blue Strip:

You correctly set up linear RGB values (col) and sent them to the GPU.
The GPU stored these linear values, but when rendered on the monitor:
The monitor’s inherent gamma (e.g., 
𝛾
=
2.2
γ=2.2) is applied.
The blue strip displayed has non-linear physical light intensities (with brighter values compressed and darker values expanded).
Perception: The strip appears perceptually linear to your eyes due to the non-linear perception of brightness.
Gamma-Corrected (sRGB) Blue Strip:

By applying 
col_gc
=
pow(col, 1/2.2)
col_gc=pow(col, 1/2.2) before sending the data to the GPU:
The RGB values stored in GPU memory are now in sRGB space (non-linear).
The monitor again applies its gamma during rendering, effectively canceling out the non-linear encoding you applied in code.
Physical Light Output: The strip’s physical intensity of light is linear.
Perception: Since human perception is non-linear, the strip appears too bright, with brightness increments seemingly compressed toward the bright end.
Your conclusions here are accurate:

A linear blue strip (no encoding) appears perceptually linear after the monitor applies its gamma.
A gamma-encoded (sRGB) blue strip appears perceptually incorrect without additional adjustments.
"Authored" Blue Strip on the Screen
When you author an image by eye to look perceptually linear:

The RGB values authored are non-linear in terms of physical light intensity.
However, the authored values in GPU memory will typically already approximate sRGB encoding because most authoring environments assume a gamma-corrected display.
When displayed, the monitor applies its gamma curve, ensuring that the physical light intensity emitted is non-linear. This compensates for the non-linear perception of brightness, making the strip appear perceptually linear.
If you save this authored strip to a file:

The RGB data stored will usually be sRGB-encoded (non-linear), as this is the standard for image formats like JPEG and PNG.
When re-displayed, the monitor applies its gamma, making the strip appear perceptually consistent with the authored image.
Second Scenario: Camera Image Encoding
You are almost entirely correct here, but let’s refine a few details:

Camera Sensor Behavior:

Camera sensors record light intensity linearly, capturing physical light directly as it falls on the sensor.
Gamma Compression for Storage:

Before saving the image to a file (e.g., JPEG, PNG), the camera applies a gamma compression curve (
pow(col, 1/2.2)
pow(col, 1/2.2)).
This step encodes the data into the sRGB color space, optimizing tonal detail for human perception and storage efficiency.
Display and Perception:

When the sRGB-encoded image is displayed on a monitor, the monitor applies its gamma curve (
pow(col, 2.2)
pow(col, 2.2)).
This cancels out the encoding applied by the camera, restoring the original linear light intensities.
Physical Output: The monitor’s emitted light intensity closely matches the light intensities originally captured by the camera sensor.
Perception: Because human vision is inherently non-linear, the restored linear light intensities now look perceptually correct to you, resembling the real-world scene.
Key Takeaways
Gamma Encoding and Decoding:

Encoding compresses tonal values to save data efficiently and match human perception.
Decoding by the monitor restores linear light intensities for accurate physical reproduction.
Human Perception:

In the case of authored or camera-captured images, the non-linear perception of brightness is indirectly addressed:
Encoding adjusts for perceptual differences.
Decoding restores physical fidelity, ensuring consistency with real-world light intensities.
Importance of Linear Space in Graphics:

All intermediate calculations in shaders or image processing should happen in linear space to avoid errors.
Final output should be encoded back to sRGB for display.
Your detailed experiment and understanding are spot on. Keep up the great work—you're well on your way to mastering these concepts! Let me know if you'd like further examples or deeper dives into related topics. 🚀

*/