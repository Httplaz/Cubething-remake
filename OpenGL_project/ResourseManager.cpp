#include "ResourseManager.h"

uint32_t ResourseManager::pixelAtlasSize = 0;
uint32_t ResourseManager::tileSize = 0;
uint32_t ResourseManager::tileAtlasSize = 0;
uint32_t ResourseManager::tileChannels = 0;
uint32_t ResourseManager::atlasLayers = 0;
vector<uint32_t> ResourseManager::atlases;
ResourseManager::TextureAtlasProcess* ResourseManager::atlasProcess;
vector<pair<uint32_t, string>> ResourseManager::layerInfo;


ResourseManager::TextureAtlasProcess::TextureAtlasProcess()
{
	data = new uint8_t * [atlasLayers];
	glTextures = new uint32_t[atlasLayers];
	for (int i = 0; i < atlasLayers; i++)
		data[i] = new uint8_t[pixelAtlasSize * pixelAtlasSize * 4];
}

ResourseManager::TextureAtlasProcess::~TextureAtlasProcess()
{
	for (int i = 0; i < atlasLayers; i++)
		delete[] data[i];
	delete[] data;
	delete glTextures;
}

void ResourseManager::TextureAtlasProcess::finish()
{
	for (int i = 0; i < atlasLayers; i++) 
	{
		glGenTextures(1, &(glTextures[i]));
		glBindTexture(GL_TEXTURE_2D, glTextures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pixelAtlasSize, pixelAtlasSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]);
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, -1);
		uint32_t pixelSizeC = pixelAtlasSize;
		uint8_t* dataC;
		uint8_t* prevDataC = data[i];
		Texture::saveImage(("aboba" + to_string(i) + " " + to_string(0) + ".png").c_str(), data[i], pixelSizeC, pixelSizeC, 4);

		for (int j = 1; j < log(pixelSizeC); j++)
		{
			pixelSizeC /= 2;
			dataC = new uint8_t[pixelSizeC * pixelSizeC * 4];
			for (int x = 0; x < pixelSizeC; x++)
				for (int y = 0; y < pixelSizeC; y++)
					for (int c = 0; c < 4; c++)
						dataC[(y * pixelSizeC + x) * 4 + c] = prevDataC[((y * 2) * pixelSizeC*2 + (x*2)) * 4 + c];
			prevDataC = dataC;
			//glTexImage2D(GL_TEXTURE_2D, j, GL_RGBA, pixelSizeC, pixelSizeC, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataC);
			//Texture::saveImage(("aboba" + to_string(i) + " " + to_string(j) + ".png").c_str(), dataC, pixelSizeC, pixelSizeC, 4);

		}

		ResourseManager::atlases.push_back(glTextures[i]);
		//Texture::saveImage(("aboba" + to_string(i) + ".png").c_str(), data[i], pixelAtlasSize, pixelAtlasSize, 4);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ResourseManager::TextureAtlasProcess::finishTile()
{
	mX++;
	if (mX >= tileAtlasSize)
	{
		mX = 0, mY++;
	}
}

bool ResourseManager::TextureAtlasProcess::isFull()
{
	return mY >= tileAtlasSize;
}

void ResourseManager::TextureAtlasProcess::addTile(uint8_t* tileData, uint32_t channel, uint32_t channelsCount)
{
	if (tileData == nullptr)
		return;
	std::cout << pixelAtlasSize << endl;
	for (int c = channel; c < channel+channelsCount; c++)
	{
		int layer = c / 4, relChannel = c % 4;
		//std::cout << "filling " << layer << " " << relChannel << " " << tileSize << "\n";
		int ded = 0;
		//if (relChannel == 1 || relChannel == 3)
			//ded = 255;
		//for (int i = 0; i < pixelAtlasSize; i++)
			//for (int j = 0; j < pixelAtlasSize; j++)
				//data[layer][(j * pixelAtlasSize + i) * 4 + relChannel] = ded;// , data[layer][(j * pixelAtlasSize + i) * 4 + 1] = 255, data[layer][(j * pixelAtlasSize + i) * 4 + 3] = 255;
	}
	for (int x=0; x<tileSize; x++)
		for (int y = 0; y < tileSize; y++)
		{
			int ax = mX * tileSize + x;
			int ay = mY * tileSize + y;
			int dataIndex = (y * tileSize + x) * channelsCount;
			int atlasIndex = (ay * pixelAtlasSize + ax) * 4;
			for (int c = channel; c < channel+channelsCount; c++)
			{
				int layer = c / 4, relChannel = c % 4;
				data[layer][atlasIndex + relChannel] = tileData[dataIndex + c - channel];
				//if (relChannel == 3)
					//data[layer][atlasIndex + relChannel] = 255;
				//data[layer][atlasIndex + 3] = 255;
			}
		}
}

void ResourseManager::loadBlockList(string dataPath)
{

}

void ResourseManager::configure(uint32_t pixelTileSize, uint32_t channels, vector<pair<uint32_t, string>> subPaths)
{
	pixelAtlasSize = GL_MAX_TEXTURE_SIZE;

	int sizeG;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &sizeG);

	tileChannels = channels;
	tileSize = pixelTileSize;
	pixelAtlasSize = 1024 * 2;
	tileAtlasSize = 2;
	atlasLayers = (channels + 3) / 4;
	atlasProcess = nullptr;
	layerInfo = subPaths;
}

void ResourseManager::loadTile(string path)
{
	if (*(path.rbegin()) != '/')
		path += '/';
	if (atlasProcess == nullptr)
		atlasProcess = new TextureAtlasProcess();
	else if (atlasProcess->isFull())
	{
		atlasProcess->finish();
		for (int i = 0; i < tileAtlasSize; i++)
			atlases.push_back(atlasProcess->glTextures[i]);
		delete atlasProcess;
		atlasProcess = new TextureAtlasProcess();
	}
	uint32_t channelOffset = 0;
	for (int i = 0; i < layerInfo.size(); i++)
		atlasProcess->addTile(Texture::loadImage(path + layerInfo[i].second), channelOffset, layerInfo[i].first), channelOffset += layerInfo[i].first;
}

void ResourseManager::loadBlock(const string path, const uint8_t sides)
{
}

void ResourseManager::finishLoading()
{
	atlasProcess->finish();
}
