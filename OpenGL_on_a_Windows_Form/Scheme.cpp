#include "stdafx.h"
#include "pch.h"
#include "Scheme.h"
#include "OrderScheme.h"
#include <msclr\marshal_cppstd.h>

// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::Scheme::buildSchemeTable()
{

	//Init table size and columns:
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Key";
	this->dataGridView->Columns[1]->Name = "Value";

	this -> dataGridView->RowCount = this->keys->size();

	for (int i = 0; i < numUnique; i++)
	{
		std::string s = this->keys->at(i);
		this->dataGridView->Rows[i]->Cells["Key"]->Value = gcnew String(s.c_str());
		this->dataGridView->Rows[i]->Cells["Value"]->Value = "";
	}

}

// ===generateValues===
// Desc: Generates values for Nominal and Ordinal.
System::Void  CppCLRWinformsProjekt::Scheme::orderWindowLaunch(System::Object^ sender, System::EventArgs^ e)
{
	//Create a new dialog window to load the scheme.
	this->Hide();
	OrderScheme^ orderGoupingWindow = gcnew OrderScheme(this->dataGridView, this->numUnique, this->numDimensions, this->attributeValue);
	orderGoupingWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	orderGoupingWindow->ShowDialog();

	this->Show();
	
	//Get the scheme map from generation to update the table.
	this->schemeMap = orderGoupingWindow->getSchemeMap();
	
	updateTable();
}

// ===updateScheme===
// Desc: Updates unordered map with new value for kvpair.
System::Void CppCLRWinformsProjekt::Scheme::updateScheme(System::Object^ sender, System::EventArgs^ e)
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

// ===updateTable===
// Desc: Updates tabel.
System::Void CppCLRWinformsProjekt::Scheme::updateTable()
{
	for (int i = 0; i < numUnique; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];
		String^ keyM = this->dataGridView->Rows[i]->Cells["Key"]->Value->ToString();
		std::string key = msclr::interop::marshal_as<std::string>(keyM);

		string val = schemeMap->at(key);

		curRow->Cells["Value"]->Value = gcnew String(val.c_str());
	}
}

System::Void CppCLRWinformsProjekt::Scheme::keepValues(System::Object^ sender, System::EventArgs^ e)
{
	for (int i = 0; i < numUnique; i++)
	{
		//Get value from datagrid view key column:
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];
		String^ keyM = this->dataGridView->Rows[i]->Cells["Key"]->Value->ToString();

		//Place in value column:
		curRow->Cells["Value"]->Value = keyM;	
	}
}


// ===getSchemeMap===
// Desc: Getter for scheme map.
std::unordered_map<std::string, std::string>* CppCLRWinformsProjekt::Scheme::getSchemeMap()
{
	return this->schemeMap;
}