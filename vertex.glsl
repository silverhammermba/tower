#version 150

in vec3 position;
in vec2 tex_coord;

out vec2 TexCoord;

uniform mat4 perspective;
uniform mat4 camera;
uniform mat4 model;

void main()
{
	TexCoord = tex_coord;

	mat4 mo = mat4(1.0);
	mo[0] = vec4(0.0, 1.0, 0.0, 0.0);
	mo[1] = vec4(-1.0, 0.0, 0.0, 0.0);
	mo[2] = vec4(0.0, 0.0, 1.0, 0.0);

	gl_Position = perspective * camera * model * vec4(position, 1.0);
}
