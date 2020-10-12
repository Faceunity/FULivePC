#pragma once

#include <memory>
#include "gl3w.h"
#include "glfw3.h"
#include <stdio.h>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>

#define logError(x) printf("%s\n",x)
#define logWarning(x) printf("%s\n",x)

static const uint32_t kMaxPossible = -1;
enum class Type
{
	Buffer,                
	Texture1D,             
	Texture2D,             
	Texture3D,             
	TextureCube,           
	Texture2DMultisample,  
};

class Texture
{
public:
	using SharedPtr = std::shared_ptr<Texture>;

	void create(uint32_t width, uint32_t height, unsigned char* pixels);

	static SharedPtr create2D(uint32_t width, uint32_t height,  uint32_t arraySize = 1, uint32_t mipLevels = kMaxPossible, const void* pInitData = nullptr);

	static Texture::SharedPtr genError(const std::string & errMsg, const std::string & filename);

	static Texture::SharedPtr createTextureFromFile(const std::string filename, bool generateMips);

	static Texture::SharedPtr createTextureFromData(uint32_t width, uint32_t height, unsigned char* pixels);

	static bool AddSearchPath(const char *path);

	bool RemoveSearchPath(const char *path);

	static std::string GetFileFullPathFromeSearchPath(const char* name);

	~Texture();

	void* getData() { if (!this) throw std::runtime_error("Unable to open texture source"); return pixels; };

	void* getTextureID() { if (!this) throw std::runtime_error("Unable to open texture source"); return (void *)(intptr_t)mTextureID; };

	unsigned int m_width, m_height;
protected:
private:
	Texture() = default;
	GLuint mTextureID;
	unsigned char* pixels;
	static std::map<std::string, SharedPtr> mTextureMap;
	static std::vector<std::string> m_searchPath;
};
