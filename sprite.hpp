class Sprite
{
	GLuint vao;
	GLuint vbo;
	GLuint texture;

	public:

	Sprite(GLint program, float width, float height, GLuint _texture)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		texture = _texture;

		GLfloat verts[] = {
			  0.f, height, 0.f, 0.f, 0.f,
			  0.f,    0.f, 0.f, 0.f, 1.f,
			width,    0.f, 0.f, 1.f, 1.f,
			width, height, 0.f, 1.f, 0.f
		};

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

		GLint position_a = glGetAttribLocation(program, "position");
		glEnableVertexAttribArray(position_a);
		glVertexAttribPointer(position_a, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

		GLint tex_coord_a = glGetAttribLocation(program, "tex_coord");
		glEnableVertexAttribArray(tex_coord_a);
		glVertexAttribPointer(tex_coord_a, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
	}

	~Sprite()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	void draw()
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
};
