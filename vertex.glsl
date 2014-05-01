#version 150

in vec3 position;
in vec2 tex_coord;

out vec2 TexCoord;
out vec4 CamSpace;

uniform mat4 perspective;
uniform mat4 camera;
uniform mat4 model;

void main()
{
	TexCoord = tex_coord;

	CamSpace = camera * model * vec4(position, 1.0);
	gl_Position = perspective * camera * model * vec4(position, 1.0);
}
