class Dude
{
	const Sprite& sprite;
	glm::vec3 pos;
	glm::vec3 vel;
	float look;
	unsigned int width;
	unsigned int height;
	float speed;

	public:

	Dude(const Sprite& _sprite) : sprite(_sprite), pos(0.f), vel(0.f)
	{
		look = 0.f;

		// TODO hardcoded from main
		width = 21;
		height = 28;
		speed = 80.f;
	}

	float get_depth() const
	{
		return pos.z;
	}

	// XXX takes a vec3, but we only need the x, y
	void look_at(const glm::vec3& spot)
	{
		glm::vec3 line = spot - pos;
		look = atan2f(line.y, line.x);
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
		glm::mat4 model(1.f);
		// XXX this seems like the reverse order to me, but it works...
		model = glm::translate(model, pos);
		model = model * glm::rotate(look, glm::vec3(0.f, 0.f, 1.f));
		model = glm::translate(model, glm::vec3(width / -2.f, height / -2.f, 5.f));
		sprite.draw(model);
	}
};
