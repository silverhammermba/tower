#include <unordered_map>

class TextureManager
{
	std::unordered_map<std::string, Texture*> textures;

	public:

	TextureManager()
	{
	}

	~TextureManager()
	{
		for (auto& pair : textures)
			delete pair.second;
	}

	const Texture& operator [](const std::string& name)
	{
		if (textures.count(name))
		{
			return *textures[name];
		}
		else
		{
			auto texture = new Texture(name);
			textures[name] = texture;

			return *texture;
		}
	}
};
