class Floor
{
	float width;
	float height;
	float depth;
	unsigned int width_c;
	unsigned int height_c;

	bool* tiles;
	bool* traversed;

	b2Body* body;

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
		traversed = new bool[width_c * height_c];

		for (int i = 0; i < width_c * height_c; ++i)
			tiles[i] = (std::rand() % 2 == 0);

		// physics
		b2BodyDef body_def;
		body_def.type = b2_staticBody;
		body_def.position.Set(0.f, 0.f);

		body = world->CreateBody(&body_def);

		update_body();
	}

	~Floor()
	{
		delete[] tiles;
		delete[] traversed;
	}

	inline int at(int x, int y) const
	{
		return y * width_c + x;
	}

	// update the physics body when the tiles change
	void update_body()
	{
		// TODO clear body fixtures

		// reset traversed state (we don't want to traverse set tiles)
		for (int i = 0; i < width_c * height_c; ++i)
			traversed[i] = tiles[i];

		for (int x = 0; x < width_c; ++x)
			for (int y = 0; y < height_c; ++y)
				if (!traversed[at(x, y)])
				{
					traverse(x, y);
				}
	}

	void traverse(int x, int y)
	{
		// location of corner in shape
		int cx = -1;
		int cy = -1;
		// flood shape, find corner
		traverse_recurse(x, y, &cx, &cy);
		// construct edge
		follow(cx, cy);
	}

	// recursively flood empty region, marking as traversed and returning a bottom left corner as (*cx, *cy)
	void traverse_recurse(int x, int y, int* cx, int* cy)
	{
		// mark as traversed
		traversed[at(x, y)] = true;

		// check for corner
		if (*cx < 0 && is_corner(x, y))
		{
			*cx = x;
			*cy = y;
		}

		// traverse neighbors
		if (x > 0 && !traversed[at(x - 1, y    )]) traverse_recurse(x - 1, y,     cx, cy);
		if (y > 0 && !traversed[at(x,     y - 1)]) traverse_recurse(x,     y - 1, cx, cy);
		if (x + 1 <  width_c && !traversed[at(x + 1, y    )]) traverse_recurse(x + 1, y,     cx, cy);
		if (y + 1 < height_c && !traversed[at(x,     y + 1)]) traverse_recurse(x,     y + 1, cx, cy);
	}

	// check if (x, y) (which is assumed empty) is a bottom left corner of an empty region
	inline bool is_corner(int x, int y) const
	{
		return (x == 0 || tiles[at(x - 1, y)]) && (y == 0 || tiles[at(x, y - 1)]);
	}

	// check if (x, y) is outside grid
	inline bool is_wall(int x, int y) const
	{
		return x < 0 || x >= width_c || y < 0 || y >= height_c;
	}

	// starting at bottom left corner (cx, cy) construct a chain shape for the region and add it to the body
	// TODO this doesn't work if a nonempty tile region is completetly detached from the walls so make such tiles collapse
	void follow(int cx, int cy)
	{
		cerr << "Corner at (" << cx << "," << cy << ")\n";

		// position
		int x = cx;
		int y = cy;
		// direction
		int dx = 1;
		int dy = 0;

		std::vector<b2Vec2> shape;
		shape.push_back(b2Vec2(width / -2.f + x * (width / width_c), height / -2.f + y * (height / height_c)));

		int i = 0;

		// we're done when we reach the starting corner going down
		while (!(dy == -1 && x == cx && y == cy))
		{
			// coords of next steps to the left and right
			int lx = x + dx;
			int ly = y + dy;
			int rx = lx + dy;
			int ry = ly - dx;
			// coords of edge to add (if not going straight)
			int ex = x + (dx + dy > 0 ? 1 : 0);
			int ey = y + (dx - dy < 0 ? 1 : 0);

			if (is_wall(lx, ly) || tiles[at(lx, ly)])
			{
				// turn left
				int temp = dy;
				dy = dx;
				dx = -temp;

				cerr << "(" << ex << "," << ey << ")\n";
				shape.push_back(b2Vec2(width / -2.f + ex * (width / width_c), height / -2.f + ey * (height / height_c)));
				// position stays the same
			}
			else if (is_wall(rx, ry) || tiles[at(rx, ry)])
			{
				// go straight
				x += dx;
				y += dy;
			}
			else
			{
				// turn right
				// TODO something is going wrong here
				int temp = dx;
				dx = dy;
				dy = -temp;

				cerr << "(" << ex << "," << ey << ")\n";
				shape.push_back(b2Vec2(width / -2.f + ex * (width / width_c), height / -2.f + ey * (height / height_c)));

				x += dx + dy;
				y += dy - dx;
			}

			if (++i > 50)
			{
				cerr << "Abort\n";
				break;
			}
		}
		cerr << "Done chain\n";

		/*
		b2ChainShape chain;
		chain.CreateLoop(shape.data(), shape.size());

		b2FixtureDef fixture;
		fixture.shape = &chain;
		fixture.friction = 0.f;

		body->CreateFixture(&fixture);
		*/
	}

	float get_depth() const
	{
		return depth;
	}

	void draw() const
	{
		glm::mat4 id(1.f);

		for (int x = 0; x < width_c; ++x)
			for (int y = 0; y < height_c; ++y)
				if (tiles[at(x, y)])
					sprite.draw(glm::translate(id, glm::vec3(
						width / -2.f + x * (width / width_c),
						height / -2.f + y * (height / height_c),
						depth
					)));
	}
};
