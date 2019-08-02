#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
    vec3 color;
} gs_in[];

out VS_OUT_Color {
    vec3 color;
} gs_out_Color;

float MAGNITUDE;

uniform int axis;

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    gs_out_Color.color = gs_in[index].color;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    gs_out_Color.color = gs_in[index].color;
    EmitVertex();
    EndPrimitive();
}

void main()
{

	if (axis == 1){
		MAGNITUDE = 1;
	}else{
		MAGNITUDE = 0.2;
	}

    GenerateLine(0); // first vertex normal
    GenerateLine(1); // second vertex normal
    GenerateLine(2); // third vertex normal

}
