#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
using namespace std;
class TextureManager
{
	static unordered_map<string, sf::Texture> textures;
	public:
		static void LoadTexture(string textureName);
		static sf::Texture& GetTexture(string textureName);
		static void Clear();
};

