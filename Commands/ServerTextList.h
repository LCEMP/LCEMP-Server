#pragma once
#include <set>
#include <string>
#include <fstream>

class ServerTextList
{
private:
	std::wstring filename;
	std::set<std::wstring> entries;

public:
	ServerTextList(const std::wstring& filename) : filename(filename) { load(); }

	void load()
	{
		entries.clear();
		std::wifstream file(
#ifdef __linux__
			std::string(filename.begin(), filename.end()).c_str()
#else
			filename.c_str()
#endif
		);
		if (!file.is_open()) return;
		std::wstring line;
		while (std::getline(file, line))
		{
			if (!line.empty())
				entries.insert(line);
		}
	}

	void save()
	{
		std::wofstream file(
#ifdef __linux__
			std::string(filename.begin(), filename.end()).c_str()
#else
			filename.c_str()
#endif
		);
		if (!file.is_open()) return;
		for (std::set<std::wstring>::const_iterator it = entries.begin(); it != entries.end(); ++it)
			file << *it << L"\n";
	}

	bool contains(const std::wstring& entry) const
	{
		return entries.find(entry) != entries.end();
	}

	void add(const std::wstring& entry)
	{
		entries.insert(entry);
		save();
	}

	void remove(const std::wstring& entry)
	{
		entries.erase(entry);
		save();
	}

	const std::set<std::wstring>& getEntries() const { return entries; }
	int size() const { return (int)entries.size(); }
};
