#version 330 core

in vec4 Color;
in vec2 UV;

uniform bool useSampler;
uniform sampler2D mainSampler;
out vec4 Frag_color;

void main()
{
	if(useSampler) Frag_color = texture(mainSampler, UV) * Color;
	else Frag_color = Color;
}
