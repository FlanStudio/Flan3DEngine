#ifndef __DIRECTORY_H__
#define __DIRECTORY_H__

#include <string>
#include <vector>

struct File
{
	std::string name;
	int64_t lastModTime = 0.0f;

	uint bytesToSerialize() const
	{
		return sizeof(uint) + name.size() + sizeof(int64_t);
	}

	void Serialize(char*& cursor) const
	{
		uint bytes = sizeof(uint);
		uint nameLenght = name.size();
		memcpy(cursor, &nameLenght, bytes);
		cursor += bytes;

		bytes = nameLenght;
		memcpy(cursor, name.c_str(), bytes);
		cursor += bytes;

		bytes = sizeof(int64_t);
		memcpy(cursor, &lastModTime, bytes);
		cursor += bytes;
	}

	void DeSerialize(char*& cursor)
	{
		uint bytes = sizeof(uint);
		uint nameLenght;
		memcpy(&nameLenght, cursor, bytes);
		cursor += bytes;


		name.resize(nameLenght);
		bytes = nameLenght;
		memcpy((char*)name.c_str(), cursor, bytes);
		cursor += bytes;

		bytes = sizeof(int64_t);
		memcpy(&lastModTime, cursor, bytes);
		cursor += bytes;
	}

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

	void Serialize(char*& cursor) const
	{
		uint fullPathLength = fullPath.size();

		uint bytes = sizeof(uint);
		memcpy(cursor, &fullPathLength, bytes);
		cursor += bytes;

		bytes = fullPathLength;
		memcpy(cursor, fullPath.c_str(), bytes);
		cursor += bytes;

		bytes = sizeof(uint);
		uint nameLenght = name.size();
		memcpy(cursor, &nameLenght, bytes);
		cursor += bytes;

		bytes = nameLenght;
		memcpy(cursor, name.c_str(), bytes);
		cursor += bytes;

		uint numFiles = files.size(), numDirs = directories.size();
		bytes = sizeof(uint);

		memcpy(cursor, &numFiles, bytes);
		cursor += bytes;
		
		memcpy(cursor, &numDirs, bytes);
		cursor += bytes;

		for (int i = 0; i < files.size(); ++i)
		{
			files[i].Serialize(cursor);
		}

		for (int i = 0; i < directories.size(); ++i)
		{
			directories[i].Serialize(cursor);
		}
	}

	void DeSerialize(char*& cursor)
	{
		uint bytes = sizeof(uint);
		uint fullPathLenght;
		memcpy(&fullPathLenght, cursor, bytes);
		cursor += bytes;

		fullPath.resize(fullPathLenght);
		memcpy((char*)fullPath.c_str(), cursor, fullPathLenght);
		cursor += fullPathLenght;

		bytes = sizeof(uint);
		uint nameLenght;
		memcpy(&nameLenght, cursor, bytes);
		cursor += bytes;

		name.resize(nameLenght);
		bytes = nameLenght;
		memcpy((char*)name.c_str(), cursor, bytes);

		cursor += bytes;

		uint numFiles;
		bytes = sizeof(uint);

		memcpy(&numFiles, cursor, bytes);
		cursor += bytes;

		uint numDirs;
		memcpy(&numDirs, cursor, bytes);
		cursor += bytes;

		files.resize(numFiles);
		directories.resize(numDirs);

		for (int i = 0; i < files.size(); ++i)
		{
			files[i].DeSerialize(cursor);
		}

		for (int i = 0; i < directories.size(); ++i)
		{
			directories[i].DeSerialize(cursor);
		}
	}

	uint bytesToSerialize() const
	{
		uint ret =
			sizeof(uint) + fullPath.size() +	//Fullpath lenght + fullpath
			sizeof(uint) + name.size() +		//Name lenght + Name
			sizeof(uint) * 2;					//Num files / directories

		for (int i = 0; i < files.size(); ++i)
		{
			ret += files[i].bytesToSerialize();
		}

		for (int i = 0; i < directories.size(); ++i)
		{
			ret += directories[i].bytesToSerialize();
		}
		return ret;
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