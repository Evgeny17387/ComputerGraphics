#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

uniform int postProcessing;

void main()
{
	
    vec3 temp;

	switch (postProcessing){

		case 0:
			temp = texture(screenTexture, TexCoords).rgb;
		break;

		case 1:
			temp = 1.0 - texture(screenTexture, TexCoords).rgb;
		break;

		case 2:{
			temp = texture(screenTexture, TexCoords).rgb;
    		float average = 0.2126 * temp.r + 0.7152 * temp.g + 0.0722 * temp.b;
		    temp = vec3(average, average, average);
		}
		break;

		case 3: case 4: case 5:{

			const float offset = 1.0 / 300.0; 

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

			float kernel[9];

			switch (postProcessing){

				case 3:

				    kernel = float[](
				        -1, -1, -1,
				        -1,  9, -1,
				        -1, -1, -1
				    );

				break;

				case 4:

				    kernel = float[](
				        1,  1,  1,
				        1, -9,  1,
				        1,  1,  1
				    );

				break;

				case 5:

				    kernel = float[](
				        1.0/16.0,  2.0/16.0,  1.0/16.0,
				        2.0/16.0,  4.0/16.0,  2.0/16.0,
				        1.0/16.0,  2.0/16.0,  1.0/16.0
				    );

				break;
			
			}
		    
		    vec3 sampleTex[9];
		    for(int i = 0; i < 9; i++)
		    {
		        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
		    }
		    temp = vec3(0.0);
		    for(int i = 0; i < 9; i++)
		        temp += sampleTex[i] * kernel[i];

		}
		break;
	}

    FragColor = vec4(temp, 1.0);

}
