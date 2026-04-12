#pragma once
#include "const.h"
struct SectionInfo
{
	SectionInfo() {}
	~SectionInfo() { _section_datas.clear(); }
	std::map<std::string, std::string> _section_datas;

	SectionInfo(const SectionInfo& s)
	{
		_section_datas = s._section_datas;
	}

	SectionInfo& operator= (const SectionInfo& s)
	{
		if (&s == this)	return *this;
		this->_section_datas = s._section_datas;
		return *this;
	}

	
	//重载运算符
	std::string operator[](const std::string& key)
	{
		//没有找到
		if (_section_datas.find(key) == _section_datas.end())
			return "";
		return _section_datas[key];	
	}
};

class ConfigMgr
{
public:
	~ConfigMgr()
	{
		_config_map.clear();
	}

	SectionInfo operator[](const std::string& section)
	{
		if (_config_map.find(section) == _config_map.end())
			return SectionInfo();	//没有找到返回一个空的section
		return _config_map[section];
	}

	ConfigMgr();

	ConfigMgr(const ConfigMgr& s)
	{
		_config_map = s._config_map;
	}

	ConfigMgr& operator=(const ConfigMgr& s)
	{
		if (&s == this)	return *this;
		_config_map = s._config_map;
		return *this;
	}


private:
	std::map<std::string, SectionInfo> _config_map;	//管理所有的section
	
};

