#version 330 core

#define NLS 10

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform int numberLightSources;
uniform vec3 lightSourceColor[NLS];
uniform int lightSourceShiness[NLS];

uniform vec3 ambientLight;
uniform vec3 ambientModel;
uniform vec3 diffuseModel;
uniform vec3 specularModel;

uniform int textureFlag;

uniform int fogFlag;
uniform float FogDensity;

uniform int lightSourceFlag;
uniform vec3 sourceColor;

uniform sampler2D myTextureSampler0;

uniform vec3 eye_position;

in Data
{
	vec3 EyeDirection_cameraspace;
	vec3 Normal_cameraspace;
	vec3 LightDirection_cameraspace[NLS];
	vec2 UV;
	vec4 viewSpace;
};

const vec3 fogColor = vec3(0.5, 0.5,0.5);

uniform int depthFlag;

float near = 0.1; 
float far  = 20.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main(){

	vec3 temp;

	if(lightSourceFlag == 1){

		temp = sourceColor;

	}else{

		temp = ambientLight * ambientModel;

		vec3 n = normalize(Normal_cameraspace);

		for (int i = 0; i < numberLightSources; i++){

			vec3 l = -normalize(LightDirection_cameraspace[i]);
			float cosTheta = clamp(dot(n, -l), 0, 1);

			vec3 E = normalize(EyeDirection_cameraspace);
			vec3 R = reflect(l, n);
			float cosAlpha = clamp(dot(E, R), 0, 1);

			temp = temp + lightSourceColor[i] * (cosTheta*diffuseModel + pow(cosAlpha, lightSourceShiness[i])*specularModel);

		}

		if (textureFlag == 1 || textureFlag == 2 || textureFlag == 3){
	    	temp = (texture(myTextureSampler0, UV)*vec4(temp, 1.0)).rgb;
		}

	}

	if (fogFlag == 1){

		  float dist = length(viewSpace);

		  float fogFactor = 1.0 /exp( (dist * FogDensity)* (dist * FogDensity));
		  fogFactor = clamp( fogFactor, 0.0, 1.0 );

		  temp = mix(fogColor, temp, fogFactor);

	}

	if (depthFlag == 1){
		float depth = LinearizeDepth(gl_FragCoord.z) / far;
		temp = vec3(depth);
	}

    float brightness = dot(temp, vec3(0.2126, 0.7152, 0.0722));

	if(lightSourceFlag == 1){

	    if(brightness > 1.0)
	        BrightColor = vec4(temp, 1.0);

	}else{

	    if(brightness > 1.0)
	        BrightColor = vec4(temp, 1.0);
	    else
	        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);

	}

	FragColor = vec4(temp, 1.0);

}
