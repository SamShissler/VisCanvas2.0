#include "stdafx.h"
#include "pch.h"
#include "NSGGroupAssingment.h"
#include <unordered_map>
#include <msclr\marshal_cppstd.h>

// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::NSGGroupAssingment::buildSchemeTable()
{
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Key (Value of Coordinate)";
	this->dataGridView->Columns[1]->Name = "Group of Values";

	for (int i = 0; i < this->passedGroups->size(); i++)
	{
		this->dataGridView->Rows->Add();
		std::string s = this->passedGroups->at(i).groupName;
		this->dataGridView->Rows[i]->Cells["Key (Value of Coordinate)"]->Value = gcnew String(s.c_str());
		this->dataGridView->Rows[i]->Cells["Group of Values"]->Value = "";
	}

}

// ===updateScheme===
// Desc: Updates unordered map with new value for kvpair.
System::Void CppCLRWinformsProjekt::NSGGroupAssingment::updateScheme(System::Object^ sender, System::EventArgs^ e)
{
	vector<string>nameCheck;
	boolean groupExists;
	unordered_map<string, string> counts; //Vector to count how many groups there are.


	//Iterate over rows and find number of groups:
	for (int i = 0; i < this->dataGridView->RowCount; i++)
	{

		if (this->dataGridView->Rows[i]->Cells["Group of Values"]->Value != "")
		{
			String^ groupNameM = this->dataGridView->Rows[i]->Cells["Group of Values"]->Value->ToString();
			std::string groupName = msclr::interop::marshal_as<std::string>(groupNameM);
			if (counts.find(groupName) == counts.end())
			{
				this->numGroups++;
			}
     	}

	}

	//Iterate over rows:
	for (int i = 0; i < this->dataGridView->RowCount; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];

		//Get the data for current key and group.
		String^ keyM = this->dataGridView->Rows[i]->Cells["Key (Value of Coordinate)"]->Value->ToString();
		String^ groM = this->dataGridView->Rows[i]->Cells["Group of Values"]->Value->ToString();

		std::string key = msclr::interop::marshal_as<std::string>(keyM);
		std::string group = msclr::interop::marshal_as<std::string>(groM);

		//If the group is not given, pass over the value.
		if (group == "") continue;

		//Check to see if we have used the key already.
		groupExists = false;
		for (int i = 0; i < nameCheck.size(); i++)
		{
			if (nameCheck.at(i) == group)
			{
				groupExists = true;
			}
		}


		if (!groupExists)
		{
			//Create the group.
			NSG newGroup = NSG(group);

			//Iterate over prev group and add values.
			for (int j = 0; j < passedGroups->size(); j++)
			{
				if (passedGroups->at(j).groupName == key)
				{
					for (int k = 0; k < passedGroups->at(j).values.size(); k++)
					{
						newGroup.values.push_back(passedGroups->at(j).values.at(k));
					}
				}
			}

			toReturn->push_back(newGroup);
			nameCheck.push_back(group);
		}
		else
		{
			//Go over current groups.
			for (int j = 0; j < this->toReturn->size(); j++)
			{
				//Find the group we want.
				if (this->toReturn->at(j).groupName == group)
				{
					//Go over passed groups.
					for (int k = 0; k < this->passedGroups->size(); k++)
					{
						//Find the key and assign all values.
						if (passedGroups->at(k).groupName == key)
						{
							for (int m = 0; m < passedGroups->at(k).values.size(); m++)
							{
								toReturn->at(j).values.push_back(passedGroups->at(k).values.at(m));
							}
						}
						
					}
				}
			}
		}

	}

	this->Close();
}

// ===getSchemeMap===
// Desc: Getter for scheme map.
std::vector<NSG>* CppCLRWinformsProjekt::NSGGroupAssingment::getNominalVector()
{
	return this->toReturn;
}

// ===getNumGroups===
// Desc: Getter for the number of groups created.
int CppCLRWinformsProjekt::NSGGroupAssingment::getNumGroups()
{
	return this->numGroups;
}