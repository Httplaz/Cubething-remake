#pragma once
#include <vector>
#include <string>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Texture.h"
//#include <glm/glm.hpp>

using namespace std;

class ResourseManager
{
	struct TextureAtlasProcess
	{
		uint32_t* glTextures;
		uint32_t mX = 0, mY = 0;
		uint8_t** data;
		TextureAtlasProcess();
		~TextureAtlasProcess();
		void finish();
		void finishTile();
		bool isFull();
		void addTile(uint8_t* data, uint32_t channel, uint32_t channelsCount);
	};
public:
	static void loadBlockList(string dataPath);
	static void configure(uint32_t tileSize, uint32_t channels, vector<pair<uint32_t, string>> subPaths);
	static void loadTile(string path);
	static void loadBlock(string path, const uint8_t sides);
	static void finishLoading();
	static uint32_t tileAtlasSize;
//private:
	static uint32_t pixelAtlasSize;
	static uint32_t tileSize;
	static uint32_t tileChannels;
	static uint32_t atlasLayers;
	static vector<uint32_t> atlases;
	static TextureAtlasProcess* atlasProcess;
	static vector<pair<uint32_t, string>> layerInfo;
};

