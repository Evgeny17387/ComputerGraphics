#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;

out VS_OUT {
    vec3 normal;
    vec3 color;
} vs_out;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

uniform int axis;

void main()
{

    mat3 normalMatrix = mat3(transpose(inverse(V * M)));
    vs_out.normal = vec3(P * vec4(normalMatrix * aNormal, 0.0));
    gl_Position = P * V * M * vec4(aPos, 1.0); 

	if (axis == 1){
    	vs_out.color = aColor;
	}else{
		vs_out.color = vec3(1, 1, 0);
	}

}
