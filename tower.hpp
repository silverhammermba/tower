class Tower
{
	GLuint vao;
	GLuint vbo;
	GLuint texture;

	public:

	Tower(GLint program, float width, float height, GLuint _texture)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		texture = _texture;

		// number of times texture repeats
		unsigned int rw = 2;
		unsigned int rh = 1;

		float depth = 40.f;

		// triangle strip forming sides of a cube
		GLfloat verts[] = {
			 width / 2.f,  height / 2.f,   0.f,      0.f, rh * 1.f,
			 width / 2.f,  height / 2.f, depth,      0.f,      0.f,

			 width / 2.f, -height / 2.f,   0.f, rw * 1.f, rh * 1.f,
			 width / 2.f, -height / 2.f, depth, rw * 1.f,      0.f,

			-width / 2.f, -height / 2.f,   0.f, rw * 2.f, rh * 1.f,
			-width / 2.f, -height / 2.f, depth, rw * 2.f,      0.f,

			-width / 2.f,  height / 2.f,   0.f, rw * 3.f, rh * 1.f,
			-width / 2.f,  height / 2.f, depth, rw * 3.f,      0.f,

			 width / 2.f,  height / 2.f,   0.f, rw * 4.f, rh * 1.f,
			 width / 2.f,  height / 2.f, depth, rw * 4.f,      0.f
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

	~Tower()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	void draw()
	{
		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
	}
};
