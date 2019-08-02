#version 330 core

out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube myTextureSampler0;

void main()
{    
    FragColor = texture(myTextureSampler0, TexCoords);
}
