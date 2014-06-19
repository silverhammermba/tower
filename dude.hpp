class Dude
{
	const Sprite& sprite;
	glm::vec3 pos;
	glm::vec3 vel;
	float look;
	unsigned int width;
	unsigned int height;
	float speed;
	bool on_floor;

	b2Body* body;

	public:

	Dude(const Sprite& _sprite) : sprite(_sprite), pos(0.f), vel(0.f)
	{
		look = 0.f;

		// TODO hardcoded from main
		width = 21;
		height = 28;

		speed = 4.f;

		on_floor = true;

		b2BodyDef body_def;
		body_def.type = b2_dynamicBody;
		body_def.position.Set(pos.x, pos.y);

		body = world->CreateBody(&body_def);

		b2CircleShape circle;
		circle.m_p.Set(0.f, 0.f);
		circle.m_radius = 0.3f;

		b2FixtureDef fixture;
		fixture.shape = &circle;
		fixture.density = 1.f;
		fixture.friction = 0.f;

		body->CreateFixture(&fixture);

		// TODO create smaller circle fixture for colliding with floor edges
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

	void set_vel(const glm::vec2& dir)
	{
		if (dir != glm::vec2(0.f))
		{
			glm::vec2 nd = glm::normalize(dir) * speed;
			body->SetLinearVelocity(b2Vec2(nd.x, nd.y));
		}
		else
		{
			body->SetLinearVelocity(b2Vec2(0.f, 0.f));
		}

		vel.z -= (gravity * time_step) / 1000.f;
	}

	void jump(const std::vector<Floor*>& floors)
	{
		vel.z = 10.f;
		on_floor = false;
		floors[nearest_floor(pos.z)]->set_active(false);
	}

	// return nearest floor at or below the given z
	inline int nearest_floor(float z) const
	{
		return (int)std::floor(z / floor_depth);
	}

	void step(const std::vector<Floor*>& floors)
	{
		pos.x = body->GetPosition().x * ppm;
		pos.y = body->GetPosition().y * ppm;

		float next_z = pos.z + (vel.z * time_step * ppm) / 1000.f;

		// nearest floor
		int this_floor = nearest_floor(pos.z);
		// nearest tile
		int tile_x = std::floor((pos.x + tower_width / 2.f) / (tower_width / tower_width_c));
		int tile_y = std::floor((pos.y + tower_height / 2.f) / (tower_height / tower_height_c));

		// if we're falling through a floor
		if (this_floor > nearest_floor(next_z) && floors[this_floor]->get_tile(tile_x, tile_y))
		{
			// stop the fall
			pos.z = this_floor * floor_depth;
			vel.z = 0;
			floors[this_floor]->set_active(true);
		}
		else // carry on
			pos.z = next_z;
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
