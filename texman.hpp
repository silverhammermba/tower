#include <unordered_map>

class TextureManager
{
	std::unordered_map<std::string, GLuint> textures;

	public:

	TextureManager()
	{
	}

	~TextureManager()
	{
		for (auto& pair : textures)
			glDeleteTextures(1, &(pair.second));
	}

	GLuint operator [](const std::string& name)
	{
		if (textures.count(name))
		{
			return textures[name];
		}
		else
		{
			// load file
			SDL_Surface* surface = IMG_Load(name.c_str());
			if (surface == nullptr)
				throw std::runtime_error("IMG_Load failed: " + std::string(IMG_GetError()));

			// create texture
			GLuint texture;
			glGenTextures(1, &texture);
			textures[name] = texture;

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

			return texture;
		}
	}
};
