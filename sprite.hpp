class Sprite
{
	GLuint vao;
	GLuint vbo;

	const Texture& texture;

	float w;
	float h;

	GLint model_u;

	public:

	Sprite(GLint program, const Texture& _texture, float _width = -1.f, float _height = -1.f, unsigned int wrapw = 1, unsigned int wraph = 1)
		: texture {_texture}, w {_width}, h {_height}
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		if (w <= 0.f)
			w = texture.width();
		if (h <= 0.f)
			h = texture.height();

		GLfloat verts[] = {
			0.f,   h, 0.f,         0.f,         0.f,
			0.f, 0.f, 0.f,         0.f, 1.f * wraph,
			  w, 0.f, 0.f, 1.f * wrapw, 1.f * wraph,
			  w,   h, 0.f, 1.f * wrapw,         0.f
		};

		bind_vao(vao);

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

	~Sprite()
	{
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	inline float width() const
	{
		return w;
	}

	inline float height() const
	{
		return h;
	}

	void draw(const glm::mat4& model) const
	{
		glUniformMatrix4fv(model_u, 1, GL_FALSE, glm::value_ptr(model));

		texture.bind();
		bind_vao(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}
};
