class Dude
{
	const Sprite& sprite;
	glm::vec3 pos;
	glm::vec3 vel;
	unsigned int width;
	unsigned int height;
	float speed;

	public:

	Dude(const Sprite& _sprite) : sprite(_sprite), pos(0.f), vel(0.f)
	{
		// TODO hardcoded from main
		width = 28;
		height = 21;
		speed = 80.f;
	}

	void move(const glm::vec2& dir)
	{
		if (dir != glm::vec2(0.f))
		{
			glm::vec2 nd = glm::normalize(dir);
			vel.x = nd.x;
			vel.y = nd.y;
		}
		else
		{
			vel.x = 0.f;
			vel.y = 0.f;
		}
	}

	void jump()
	{
		vel.z = 8.f;
	}

	void step()
	{
		vel.z -= (gravity * time_step) / 1000.f;
		pos += vel * ((time_step * speed) / 1000.f);
		if (pos.z < 0)
			pos.z = 0;
	}

	void draw()
	{
		// center sprite, elevate slightly
		sprite.draw(glm::translate(glm::mat4(1.f), pos + glm::vec3(width / -2.f, height / -2.f, 5.f)));
	}
};
