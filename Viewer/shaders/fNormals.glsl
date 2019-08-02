#version 330 core

out vec4 FragColor;

in VS_OUT_Color {
    vec3 color;
} gs_out_Color;

void main()
{
    FragColor = vec4(gs_out_Color.color, 1.0);
}
