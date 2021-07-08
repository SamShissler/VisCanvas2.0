#ifndef NSG_HEADER
#define NSG_HEADER

#include "pch.h"
#include <string>
#include<vector>

public class NSG {

public:
	std::string groupName;
	int numSubgroups;
	std::vector<std::string> values;
	std::vector<NSG> subGroups;
	std::vector<NSG> subCats;
	std::vector<string> schemeMap;


	NSG(std::string n)
	{
		groupName = n;
		numSubgroups = 0;
		
		vector<string> v;
		values = v;

		vector<NSG> sg;
		subGroups = sg;

		vector<string> sm;
		schemeMap = sm;
		
		vector<NSG> sc;
		subCats = sc;
	}

	NSG(std::string n, std::vector<std::string> v, std::vector<NSG> sg)
	{
		groupName = n;
		numSubgroups = 0;
		values = v;
		subGroups = sg;
		vector<string> sm;
		schemeMap = sm;

		vector<NSG> sc;
		subCats = sc;
	}

	NSG(std::string n, std::vector<std::string> v)
	{
		groupName = n;
		numSubgroups = 0;
		values = v;

		vector<NSG> sg;
		subGroups = sg;
		vector<string> sm;
		schemeMap = sm;

		vector<NSG> sc;
		subCats = sc;
	}
};

#endif