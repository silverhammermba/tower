class Floor
{
	float width;
	float height;
	float depth;
	unsigned int width_c;
	unsigned int height_c;

	bool* tiles;

	const Sprite& sprite;

	public:

	Floor(float _width, float _height, float _depth, const Sprite& _sprite)
		: sprite(_sprite)
	{
		width = _width;
		height = _height;
		depth = _depth;

		// XXX 40 hard coded from texture size
		width_c = std::max((int)_width / 40, 1);
		height_c = std::max((int)_height / 40, 1);

		tiles = new bool[width_c * height_c];

		for (int i = 0; i < width_c * height_c; ++i)
			tiles[i] = (std::rand() % 2 == 0);
	}

	~Floor()
	{
		delete[] tiles;
	}

	float get_depth() const
	{
		return depth;
	}

	void draw() const
	{
		glm::mat4 id(1.f);

		for (int i = 0; i < width_c * height_c; ++i)
		{
			if (tiles[i])
				sprite.draw(glm::translate(id, glm::vec3(width / -2.f + (i % width_c) * (width / width_c), height / -2.f + (i / width_c) * (height / height_c), depth)));
		}
	}
};
