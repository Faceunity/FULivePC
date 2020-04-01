#include "Loader.h"
std::vector<std::string> Loader::m_searchPath;
Loader::Loader()
{
}

Loader::~Loader()
{
}
bool Loader::Init()
{
	return true;
}
bool Loader::Shutdown()
{
	m_searchPath.clear();
	return true;
}
bool Loader::AddSearchPath(const char *path)
{
	std::vector<std::string>::iterator itr = m_searchPath.begin();
	while (itr != m_searchPath.end())
	{
		if (!(*itr).compare(path))
		{
			return true;
		}
		itr++;
	}
	m_searchPath.push_back(path);
	return true;
}
bool Loader::RemoveSearchPath(const char *path)
{
	std::vector<std::string>::iterator src = m_searchPath.begin();

	while (src != m_searchPath.end()) {
		if (!(*src).compare(path)) {
			m_searchPath.erase(src);
			return true;
		}
		src++;
	}
	return true;
}
char* Loader::Read(const char *filePath, int32_t &length)
{
	FILE* fp = (FILE*)OpenFileFromeSearchPath(filePath);

	if (!fp) {
		fprintf(stderr, "Error opening file '%s'\n", filePath);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);
	length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char *data = new char[length + 1];
	fread(data, 1, length, fp);
	data[length] = '\0';

	fclose(fp);

#ifdef DEBUG
	fprintf(stderr, "Read file '%s', %d bytes\n", filePath, length);
#endif
	return data;
}
bool Loader::Free(char* ref)
{
	delete[] ref;
	return true;
}
bool Loader::FileExists(const char *filePath)
{
	void* fp = OpenFileFromeSearchPath(filePath);
	if (fp != NULL) {
		fclose((FILE*)fp);
		return true;
	}
	return false;
}

void * Loader::OpenFileFromeSearchPath(const char * name)
{
	FILE *fp = NULL;
	// loop N times up the hierarchy, testing at each level
	std::string upPath;
	std::string fullPath;
	for (int32_t i = 0; i < 10; i++) {
		std::vector<std::string>::iterator src = m_searchPath.begin();
		bool looping = true;
		while (looping) {
			fullPath.assign(upPath);  // reset to current upPath.
			if (src != m_searchPath.end()) {
				//sprintf_s(fullPath, "%s%s/assets/%s", upPath, *src, filePath);
				fullPath.append(*src);
				fullPath.append("/assets/");
				src++;
			}
			else {
				//sprintf_s(fullPath, "%sassets/%s", upPath, filePath);
				fullPath.append("assets/");
				looping = false;
			}
			fullPath.append(name);

#ifdef DEBUG
			//fprintf(stderr, "Trying to open %s\n", fullPath.c_str());
#endif
#ifdef _WIN32
			if ((fopen_s(&fp, fullPath.c_str(), "rb") == 0) || (fp != NULL))
				return (void*)fp;
#else
            fp = fopen(fullPath.c_str(), "rb");
            if(fp)
            {
                return (void*)fp;
            }
#endif
		}

		upPath.append("../");
	}
	return nullptr;
}

int64_t Loader::GetFileSize(void *file)
{
	FILE* fp = (FILE*)file;

	size_t pos = ftell(fp);
	fseek(fp, 0, SEEK_END);
	size_t length = ftell(fp);
	fseek(fp, pos, SEEK_SET);

	return length;
}

