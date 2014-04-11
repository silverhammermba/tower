#version 150

in vec3 position;
in vec2 tex_coord;

out vec2 TexCoord;

uniform mat4 perspective;
uniform vec3 camera;

void main()
{
	TexCoord = tex_coord;

	gl_Position = perspective * vec4(position - camera, 1.0);
}
