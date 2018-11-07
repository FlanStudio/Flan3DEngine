#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include <string>
#include <vector>

struct Directory
{
	std::string fullPath;
	std::string name;
	std::vector<std::string> files;
	std::vector<Directory> directories;

	~Directory()
	{
		name.clear();
		files.clear();
		directories.clear();
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