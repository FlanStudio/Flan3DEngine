#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include <string>
#include <vector>

struct File
{
	std::string name;
	int64_t lastModTime = 0.0f;

	bool operator == (File other)
	{
		return (name == other.name) && (lastModTime == other.lastModTime);
	}
	bool operator != (File other)
	{
		return !(*(this) == other);
	}
};

struct Directory
{
	std::string fullPath;
	std::string name;
	std::vector<File> files;
	std::vector<Directory> directories;

	~Directory()
	{
		name.clear();
		files.clear();
		directories.clear();
	}

	void getFullPaths(std::vector<std::string>& fullpaths) const
	{
		for (int i = 0; i < files.size(); ++i)
			fullpaths.push_back(fullPath + "/" + files[i].name);

		for (int i = 0; i < directories.size(); ++i)
			directories[i].getFullPaths(fullpaths);
	}

	void getFiles(std::vector<File>& _files) const
	{
		for (int i = 0; i < files.size(); ++i)
			_files.push_back(files[i]);

		for (int i = 0; i < directories.size(); ++i)
			directories[i].getFiles(_files);
	}

	bool operator == (Directory other)
	{
		bool ret = true;

		if (name != other.name || files.size() != other.files.size() || directories.size() != other.directories.size())
			ret = false;

		if (ret)
		{
			for (int i = 0; i < files.size() && ret; ++i)
			{
				if (files[i] != other.files[i])
					ret = false;
			}

			if (ret)
			{
				for (int i = 0; i < directories.size() && ret; ++i)
				{
					if (directories[i] != other.directories[i])
						ret = false;
				}
			}
		}
		return ret;
	}

	bool operator != (Directory other)
	{
		return !(*this == other);
	}
};
#endif