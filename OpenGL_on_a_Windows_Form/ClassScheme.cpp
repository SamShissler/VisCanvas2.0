#include "stdafx.h"
#include "pch.h"
#include "ClassScheme.h"
#include <msclr\marshal_cppstd.h>

// ===loadData===
// Desc: Takes current data and finds all keys to display.
System::Void CppCLRWinformsProjekt::ClassScheme::loadData()
{
	//Init scheme map and keys vector:
	schemeMap = new std::unordered_map<std::string, std::string>;
	keys = new std::vector<std::string>;

	try
	{
		//Itterate over rows:
		for (int i = 0; i <= numRows; i++)
		{
			DataGridViewRow^ curRow = this->passedData->Rows[i];

			String^ toMarshal;
			
			//Get Class Data:
			toMarshal = curRow->Cells["class"]->Value->ToString();
			std::string curVal = msclr::interop::marshal_as<std::string>(toMarshal);

			//If the key is not in our scheme map:
			if (schemeMap->find(curVal) == schemeMap->end())
			{
				schemeMap->insert({ curVal, "" });
				keys->push_back(curVal);
				this->numUnique++;
			}
		}

		buildSchemeTable();
	}
	catch (...)
	{
		return;
	}

}

// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::ClassScheme::buildSchemeTable()
{

	//Init table size and columns:
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Key";
	this->dataGridView->Columns[1]->Name = "Value";

	this->dataGridView->RowCount = this->keys->size();

	for (int i = 0; i < numUnique; i++)
	{
		std::string s = this->keys->at(i);
		this->dataGridView->Rows[i]->Cells["Key"]->Value = gcnew String(s.c_str());
		this->dataGridView->Rows[i]->Cells["Value"]->Value = "";
	}

}


// ===updateScheme===
// Desc: Updates unordered map with new value for kvpair.
System::Void CppCLRWinformsProjekt::ClassScheme::updateScheme(System::Object^ sender, System::EventArgs^ e)
{
	//Itterate over rows:
	for (int i = 0; i < numUnique; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];

		String^ valM = this->dataGridView->Rows[i]->Cells["Value"]->Value->ToString();
		String^ keyM = this->dataGridView->Rows[i]->Cells["Key"]->Value->ToString();

		std::string val = msclr::interop::marshal_as<std::string>(valM);
		std::string key = msclr::interop::marshal_as<std::string>(keyM);

		auto it = this->schemeMap->find(key);
		if (it != schemeMap->end())
		{
			it->second = val;
		}

	}
	this->Close();
}

// ===getSchemeMap===
// Desc: Getter for scheme map.
std::unordered_map<std::string, std::string>* CppCLRWinformsProjekt::ClassScheme::getSchemeMap()
{
	return this->schemeMap;
}