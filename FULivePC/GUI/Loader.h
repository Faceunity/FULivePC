#pragma once
#include <string>
#include <vector>

static class Loader
{
public:
	Loader();
	bool Init();
	bool Shutdown();
	static bool AddSearchPath(const char *path);
	bool RemoveSearchPath(const char *path);
	static char* Read(const char *filePath, int32_t &length);
	bool Free(char* ref);
	bool FileExists(const char *filePath);
	static void* OpenFileFromeSearchPath(const char* name);
	int64_t GetFileSize(void*);
	~Loader();

private:
	static std::vector<std::string> m_searchPath;
};


