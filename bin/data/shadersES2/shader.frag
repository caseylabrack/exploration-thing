precision highp float;

// these are our textures
uniform sampler2D tex0;//this is the background texture
uniform sampler2D maskTex;

// this comes from the vertex shader
varying vec2 texCoordVarying;

void main()
{
	// Get color values from the background and foreground
	vec4 back = texture2D(tex0, texCoordVarying);

	// get alpha from mask
	float mask = texture2D(maskTex, texCoordVarying).a;
	
	//mix colors from background and foreground based on the mask value
	gl_FragColor = mix(back, vec4(0,0,0,0), mask);
}
