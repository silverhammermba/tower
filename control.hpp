class KeyControls
{
	public:
	enum repeat_t {PRESS, RELEASE, REPEAT, HOLD};

	private:
	class Command
	{
		repeat_t repeat;
		public:
		bool active {false};
		bool ready {true};

		Command(repeat_t _repeat = REPEAT)
			: repeat(_repeat)
		{
		}

		inline repeat_t get_repeat() const
		{
			return repeat;
		}
	};

	std::unordered_map<SDL_Keycode, std::string> binds;
	std::unordered_map<std::string, Command> commands;

	public:

	KeyControls()
	{
	}

	KeyControls& bind(const std::string& command, SDL_Keycode key, repeat_t repeat)
	{
		commands[command] = Command(repeat);
		binds[key] = command;
		return *this;
	}

	// update control state given (key) event
	void update(const SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN)
		{
			auto it = binds.find(event.key.keysym.sym);
			if (it != binds.end())
			{
				Command& c = commands[it->second];
				switch (c.get_repeat())
				{
					case PRESS:
						if (!event.key.repeat)
							c.active = true;
						break;
					case REPEAT:
					case HOLD:
						c.active = true;
						c.active = true;
						break;
				}
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			auto it = binds.find(event.key.keysym.sym);
			if (it != binds.end())
			{
				Command& c = commands[it->second];
				switch (c.get_repeat())
				{
					case RELEASE:
						c.active = true;
						break;
					case HOLD:
						c.active = false;
						break;
				}
			}
		}
	}

	bool operator[](const std::string& command)
	{
		Command& c = commands[command];
		bool active = c.active;

		if (c.get_repeat() != HOLD)
			c.active = false;

		return active;
	}
};
