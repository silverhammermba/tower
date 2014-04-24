class Tower
{
	GLuint vao;
	GLuint vbo;
	GLuint texture;

	GLint model_u;

	public:

	Tower(GLint program, float width, float height, GLuint _texture)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		texture = _texture;

		float depth = 40.f;

		// number of times texture repeats
		unsigned int rw = std::max((int)width / 40, 1);
		unsigned int rh = std::max((int)height / 40, 1);
		unsigned int rd = std::max((int)depth / 40, 1);

		// triangle strip forming sides of a cube
		GLfloat verts[] = {
			 width / 2.f,  height / 2.f,   0.f, (GLfloat)(rw * 0.f + rh * 0.f), rd * 1.f,
			 width / 2.f,  height / 2.f, depth, (GLfloat)(rw * 0.f + rh * 0.f),      0.f,

			 width / 2.f, -height / 2.f,   0.f, (GLfloat)(rw * 1.f + rh * 0.f), rd * 1.f,
			 width / 2.f, -height / 2.f, depth, (GLfloat)(rw * 1.f + rh * 0.f),      0.f,

			-width / 2.f, -height / 2.f,   0.f, (GLfloat)(rw * 1.f + rh * 1.f), rd * 1.f,
			-width / 2.f, -height / 2.f, depth, (GLfloat)(rw * 1.f + rh * 1.f),      0.f,

			-width / 2.f,  height / 2.f,   0.f, (GLfloat)(rw * 2.f + rh * 1.f), rd * 1.f,
			-width / 2.f,  height / 2.f, depth, (GLfloat)(rw * 2.f + rh * 1.f),      0.f,

			 width / 2.f,  height / 2.f,   0.f, (GLfloat)(rw * 2.f + rh * 2.f), rd * 1.f,
			 width / 2.f,  height / 2.f, depth, (GLfloat)(rw * 2.f + rh * 2.f),      0.f
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

		model_u = glGetUniformLocation(program, "model");
	}

	~Tower()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	void draw(GLfloat depth)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, depth));
		glUniformMatrix4fv(model_u, 1, GL_FALSE, glm::value_ptr(model));

		glBindTexture(GL_TEXTURE_2D, texture);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 10);
	}
};
