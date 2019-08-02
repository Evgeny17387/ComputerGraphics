#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 eye_position;
uniform samplerCube myTextureSampler0;

void main()
{    
    vec3 I = normalize(Position - eye_position);
    vec3 R = reflect(I, normalize(Normal));
    FragColor = vec4(texture(myTextureSampler0, R).rgb, 1.0);
}
