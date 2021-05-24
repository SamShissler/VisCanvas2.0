#include "stdafx.h"
#include "pch.h"
#include "OrdinalGrouping.h"
#include <msclr\marshal_cppstd.h>

// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::OrdinalGrouping::buildSchemeTable()
{
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Key";
	this->dataGridView->Columns[1]->Name = "Position";

	for (int i = 0; i < this->ids->size(); i++)
	{
		this->dataGridView->Rows->Add();
		std::string s = this->ids->at(i);
		this->dataGridView->Rows[i]->Cells["Key"]->Value = gcnew String(s.c_str());
		this->dataGridView->Rows[i]->Cells["Position"]->Value = "";
	}

}

// ===updateScheme===
// Desc: Updates unordered map with new value for kvpair.
System::Void CppCLRWinformsProjekt::OrdinalGrouping::updateScheme(System::Object^ sender, System::EventArgs^ e)
{
	toAdd = new std::vector<string>();

	//Itterate over rows:
	for (int i = 0; i < this->dataGridView->RowCount; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];

		//Get the data for current key and position.
		String^ keyM = this->dataGridView->Rows[i]->Cells["Key"]->Value->ToString();
		String^ posM = this->dataGridView->Rows[i]->Cells["Position"]->Value->ToString();

		std::string key = msclr::interop::marshal_as<std::string>(keyM);
		std::string position = msclr::interop::marshal_as<std::string>(posM);
		
		this->toAdd->push_back(key);
		this->toAdd->push_back(position);

	}
	this->Close();
}

// ===getSchemeMap===
// Desc: Getter for scheme map.
std::vector<std::string>* CppCLRWinformsProjekt::OrdinalGrouping::getOrdinalVector()
{
	return this->toAdd;
}