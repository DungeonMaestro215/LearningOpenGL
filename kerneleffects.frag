#version 330 core

//out vec4 FragColor;
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform float offset;
uniform bool blur;
//const float offset = 1.0 / 30.0;

void main() { 
	vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    // Sharpen
    //float kernel[9] = float[](
        //-1, -1, -1,
        //-1,  9, -1,
        //-1, -1, -1
    //);

    // Blur
    float kernel[9];
    if (blur) {
		kernel = float[](
			1.0 / 16, 2.0 / 16, 1.0 / 16,
			2.0 / 16, 4.0 / 16, 2.0 / 16,
			1.0 / 16, 2.0 / 16, 1.0 / 16  
		);
    }
    else {
		// Edge detection
		kernel = float[](
			1,  1,  1,
			1, -8,  1,
			1,  1,  1
        );
    }

    vec3 sampleTex[9];
    for (int i = 0; i < 9; i++)
        sampleTex[i] = vec3(texture(screenTexture, TexCoords + offsets[i]));

    vec3 col = vec3(0.0f);
    for (int i = 0; i < 9; i++) 
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0f);

    //vec3 average = vec3(0.30 * col.x + 0.59 * col.y + 0.11 * col.z);
    //FragColor = vec4(average, 1.0f);
}
