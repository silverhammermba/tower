class Floor
{
	float width;
	float height;
	unsigned int width_c;
	unsigned int height_c;

	bool* tiles;

	Sprite sprite;

	public:

	Floor(GLint program, float _width, float _height, GLuint texture)
		: sprite(program, _width / std::max((int)_width / 40, 1), _height / std::max((int)_height / 40, 1), texture)
	{
		// XXX 40 hard coded from texture size
		width = _width;
		height = _height;
		width_c = std::max((int)_width / 40, 1);
		height_c = std::max((int)_height / 40, 1);

		tiles = new bool[width_c * height_c];

		for (int i = 0; i < width_c * height_c; ++i)
			tiles[i] = true;
	}

	~Floor()
	{
		delete[] tiles;
	}

	void draw() const
	{
		glm::mat4 id(1.f);

		for (int i = 0; i < width_c * height_c; ++i)
		{
			if (tiles[i])
				sprite.draw(glm::translate(id, glm::vec3(width / -2.f + (i % width_c) * (width / width_c), height / -2.f + (i / width_c) * (height / height_c), 0.f)));
		}
	}
};
