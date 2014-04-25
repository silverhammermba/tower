class Dude
{
	const Sprite& sprite;
	glm::vec3 pos;
	unsigned int width;
	unsigned int height;

	public:

	Dude(const Sprite& _sprite) : sprite(_sprite), pos(0.f, 0.f, 0.f)
	{
		// TODO hardcoded from main
		width = 28;
		height = 21;
	}

	void move(const glm::vec2& dir)
	{
		// TODO not frame-dependent
		glm::vec2 nd = glm::normalize(dir);
		pos += glm::vec3(nd, 0.f);
	}

	void draw()
	{
		sprite.draw(glm::translate(glm::mat4(1.f), pos - glm::vec3(width / 2.f, height / 2.f, 0.f)));
	}
};
