///
// Name: Eli McCoy
// Date: 3/24/21
// File: Scheme.h
// Desc: Header file for KVPair. This class is basic and getters and setters could be added.
///

#include <string>
#include "pch.h"

public class KVPair
{
public:
	std::string key;
	std::string rowData;
	std::string type;

	KVPair()
	{
		key = "";
		rowData = "";
		type = "";
	}
};