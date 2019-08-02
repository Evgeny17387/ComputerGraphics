#version 330 core

#define NLS 10

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

layout(location = 3) in vec2 vertexUVPlanar;
layout(location = 4) in vec2 vertexUVSpherical;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

uniform int numberLightSources;
uniform vec3 LightPosition_worldspace[NLS];
uniform int lightSourceType[NLS];

uniform int textureFlag;

out Data
{
	vec3 EyeDirection_cameraspace;
	vec3 Normal_cameraspace;
	vec3 LightDirection_cameraspace[NLS];
	vec2 UV;
	vec4 viewSpace;
};

void main(){

	gl_Position =  MVP * vec4(vertexPosition_modelspace, 1);

	EyeDirection_cameraspace = vec3(0, 0, 0) - (V * M * vec4(vertexPosition_modelspace, 1)).xyz;

	Normal_cameraspace = (transpose(inverse(V * M)) * vec4(vertexNormal_modelspace, 0)).xyz;

	for (int i = 0; i < numberLightSources; i++){

		LightDirection_cameraspace[i] = lightSourceType[i] == 0 ? (V * vec4(LightPosition_worldspace[i], 1)).xyz + EyeDirection_cameraspace : LightPosition_worldspace[i];

	}

	switch (textureFlag){
		case 1:
    		UV = vertexUV;
    	break;
		case 2:
    		UV = vertexUVPlanar;
    	break;
		case 3:
    		UV = vertexUVSpherical;
    	break;
	}

	viewSpace = V * M * vec4(vertexPosition_modelspace, 1);

}
