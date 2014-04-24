class Dude
{
	const Sprite& sprite;
	glm::vec3 pos;
	unsigned int width;
	unsigned int height;

	public:

	Dude(const Sprite& _sprite) : sprite(_sprite), pos(0.f, 0.f, 0.f)
	{
		// XXX hardcoded from main
		width = 28;
		height = 21;
	}

	void draw()
	{
		sprite.draw(glm::translate(glm::mat4(1.f), pos));
	}
};
