// a lightweight wrapper around GL textures
class Texture
{
	static GLuint bound_texture;

	GLuint texture;
	int w;
	int h;

	public:

	Texture(const std::string& name)
	{
		// load file
		SDL_Surface* surface = IMG_Load(name.c_str());
		if (surface == nullptr)
			throw std::runtime_error("IMG_Load failed: " + std::string(IMG_GetError()));

		// store dimensions
		w = surface->w;
		h = surface->h;

		// create texture
		glGenTextures(1, &texture);

		// set active
		glBindTexture(GL_TEXTURE_2D, texture);

		// load surface to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
		SDL_FreeSurface(surface);

		// set parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	~Texture()
	{
		glDeleteTextures(1, &texture);
	}

	inline GLuint gl() const
	{
		return texture;
	}

	inline int width() const
	{
		return w;
	}

	inline int height() const
	{
		return h;
	}

	void bind() const
	{
		if (texture != bound_texture)
		{
			bound_texture = texture;
			glBindTexture(GL_TEXTURE_2D, texture);
		}
	}
};

GLuint Texture::bound_texture;
