#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

layout(location = 3) in vec2 vertexUVPlanar;
layout(location = 4) in vec2 vertexUVSpherical;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 lightSpaceMatrix;

uniform int textureFlag;

void main()
{
    vs_out.FragPos = vec3(M * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(M))) * aNormal;
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);

    gl_Position = P * V * M * vec4(aPos, 1.0);

	switch (textureFlag){
		case 1:
    		vs_out.TexCoords = aTexCoords;
    	break;
		case 2:
    		vs_out.TexCoords = vertexUVPlanar;
    	break;
		case 3:
    		vs_out.TexCoords = vertexUVSpherical;
    	break;
	}

}
