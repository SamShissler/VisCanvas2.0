#include "stdafx.h"
#include "pch.h"
#include "NominalGrouping.h"
#include <msclr\marshal_cppstd.h>

// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::NominalGrouping::buildSchemeTable()
{
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Key";
	this->dataGridView->Columns[1]->Name = "Group";

	for (int i = 0; i < this->ids->size(); i++)
	{
		this->dataGridView->Rows->Add();
		std::string s = this->ids->at(i);
		this->dataGridView->Rows[i]->Cells["Key"]->Value = gcnew String(s.c_str());
		this->dataGridView->Rows[i]->Cells["Group"]->Value = "";
	}

}

// ===updateScheme===
// Desc: Updates unordered map with new value for kvpair.
System::Void CppCLRWinformsProjekt::NominalGrouping::updateScheme(System::Object^ sender, System::EventArgs^ e)
{
	toAdd = new std::vector<string>();

	int numGroups = 0;

	//Iterate over rows and find number of groups:
	for (int i = 0; i < this->dataGridView->RowCount; i++)
	{
		String^ groM = this->dataGridView->Rows[i]->Cells["Group"]->Value->ToString();
		std::string group = msclr::interop::marshal_as<std::string>(groM);
		int valGroup = stoi(group);
		if (valGroup >= numGroups)
		{
			numGroups = valGroup;
		}
	}

	//Start each group at one.
	std::vector<int> v;
	for (int i = 0; i < numGroups; i++)
	{
		v.push_back(1);
	}

	//Iterate over rows:
	for (int i = 0; i < this->dataGridView->RowCount; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];

		//Get the data for current key and group.
		String^ keyM = this->dataGridView->Rows[i]->Cells["Key"]->Value->ToString();
		String^ groM= this->dataGridView->Rows[i]->Cells["Group"]->Value->ToString();

		std::string key = msclr::interop::marshal_as<std::string>(keyM);
		std::string group = msclr::interop::marshal_as<std::string>(groM);

		//Get group num:
		int valGroup = stoi(group);

		//Get current number that is being used:
		int curToBinary = v.at(valGroup - 1);

		//increment for next use:
		v.at(valGroup - 1)++;

		this->toAdd->push_back(key);
		this->toAdd->push_back(toBinary(curToBinary) + "b");

	}

	//At this point, there are all calculated binary values for each key. 
	//Now we want to add preceding zeros.
	
	int largestBinaryLength = 0;

	//Go over binary vals to find largest in size
	for (int i = 1; i < this->toAdd->size(); i += 2)
	{
		string s = this->toAdd->at(i);
		if (s.length() > largestBinaryLength)
		{
			largestBinaryLength = s.length();
		}
	}

	//Go over values again and add preceding zeros.
	for (int i = 1; i < this->toAdd->size(); i += 2)
	{
		string s = this->toAdd->at(i);
		if (s.length() < largestBinaryLength)
		{
			int diff = largestBinaryLength - s.length();

			for (int j = 0; j < diff; j++)
			{
				s = "0" + s;
			}

			this->toAdd->at(i) = s;
		}
	}

	this->Close();
}

// ===getSchemeMap===
// Desc: Getter for scheme map.
std::vector<std::string>* CppCLRWinformsProjekt::NominalGrouping::getNominalVector()
{
	return this->toAdd;
}

//===toBinary===
// Desc: Converts to binary.
std::string CppCLRWinformsProjekt::NominalGrouping::toBinary(int n)
{
	std::string r;
	while (n != 0) { r = (n % 2 == 0 ? "0" : "1") + r; n /= 2; }
	return r;
}