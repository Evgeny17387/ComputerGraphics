#version 330 core

#define NLS 10

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in Data
{
    vec3 EyeDirection_cameraspace;
    vec3 Normal_cameraspace;
    vec3 LightDirection_cameraspace[NLS];
    vec2 UV;
    vec4 viewSpace;
} Data_in[];

out Data
{
    vec3 EyeDirection_cameraspace;
    vec3 Normal_cameraspace;
    vec3 LightDirection_cameraspace[NLS];
    vec2 UV;
    vec4 viewSpace;
} Data_out;

uniform float time;

vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude; 
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {    

    vec3 normal = GetNormal();

    Data_out.EyeDirection_cameraspace = Data_in[0].EyeDirection_cameraspace;
    Data_out.Normal_cameraspace = Data_in[0].Normal_cameraspace;
    Data_out.LightDirection_cameraspace = Data_in[0].LightDirection_cameraspace;
    Data_out.UV = Data_in[0].UV;
    Data_out.viewSpace = Data_in[0].viewSpace;
    gl_Position = explode(gl_in[0].gl_Position, normal);
    EmitVertex();

    Data_out.EyeDirection_cameraspace = Data_in[1].EyeDirection_cameraspace;
    Data_out.Normal_cameraspace = Data_in[1].Normal_cameraspace;
    Data_out.LightDirection_cameraspace = Data_in[1].LightDirection_cameraspace;
    Data_out.UV = Data_in[1].UV;
    Data_out.viewSpace = Data_in[1].viewSpace;
    gl_Position = explode(gl_in[1].gl_Position, normal);
    EmitVertex();

    Data_out.EyeDirection_cameraspace = Data_in[2].EyeDirection_cameraspace;
    Data_out.Normal_cameraspace = Data_in[2].Normal_cameraspace;
    Data_out.LightDirection_cameraspace = Data_in[2].LightDirection_cameraspace;
    Data_out.UV = Data_in[2].UV;
    Data_out.viewSpace = Data_in[2].viewSpace;
    gl_Position = explode(gl_in[2].gl_Position, normal);
    EmitVertex();

    EndPrimitive();

}
