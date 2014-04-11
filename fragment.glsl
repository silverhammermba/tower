#version 150

in vec2 TexCoord;
in vec2 WorldPos;

out vec4 outColor;

uniform uint time;
uniform sampler2D sprite;

void main()
{
	outColor = texture(sprite, TexCoord);
}
