#version 150

in vec2 TexCoord;
in vec4 CamSpace;

out vec4 outColor;

uniform uint time;
uniform float zNear;
uniform float zFar;
uniform sampler2D sprite;

void main()
{
	// TODO using alpha causes stuff to appear transparent, duh
	outColor = texture(sprite, TexCoord) * vec4(vec3(1.0), 1.0 - (-CamSpace.z - zNear) / (zFar - zNear));
}
