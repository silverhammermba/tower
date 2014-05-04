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
	outColor = texture(sprite, TexCoord);
	// TODO perhaps optimize using step or something
	if (outColor.a < 1.0)
		gl_FragDepth = 1.0;
	else
		gl_FragDepth = gl_FragCoord.z;
}
