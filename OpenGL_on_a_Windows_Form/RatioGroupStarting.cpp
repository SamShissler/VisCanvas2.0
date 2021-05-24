#include "stdafx.h"
#include "pch.h"
#include "RatioGroupStarting.h"
#include "OrderScheme.h"
#include <msclr\marshal_cppstd.h>

// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::RatioGroupStarting::buildTable()
{

	//Init table size and columns:
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Starting Point";
	this->dataGridView->Columns[1]->Name = "Interval To First";

	this->dataGridView->RowCount = this->numGroups;

	this->dataGridView->Rows[0]->Cells["Starting Point"]->Value = "";
	this->dataGridView->Rows[0]->Cells["Interval To First"]->Value = "";

}

// ===handleClose===
// Desc: builds the group starting point vector.
System::Void CppCLRWinformsProjekt::RatioGroupStarting::handleClose(System::Object^ sender, System::EventArgs^ e)
{
	this->startingPoints = new std::vector<std::string>();
	this->IntervalToFirst = new std::vector<std::string>();

	//Itterate over rows:
	for (int i = 0; i < this->numGroups; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];

		String^ startP = this->dataGridView->Rows[i]->Cells["Starting Point"]->Value->ToString();
		String^ intervalT = this->dataGridView->Rows[i]->Cells["Interval To First"]->Value->ToString();

		std::string sp = msclr::interop::marshal_as<std::string>(startP);
		std::string tf = msclr::interop::marshal_as<std::string>(intervalT);

		this->startingPoints->push_back(sp);
		this->IntervalToFirst->push_back(tf);

	}
	this->Close();
}

// ===getStartingPoints===
// Desc: returns the vector of strings of starting points.
std::vector<std::string>* CppCLRWinformsProjekt::RatioGroupStarting::getIntervalToFirst()
{
	return this->IntervalToFirst;
}


// ===getStartingPoints===
// Desc: returns the vector of strings of starting points.
std::vector<std::string>* CppCLRWinformsProjekt::RatioGroupStarting::getStartingPoints()
{
	return this->startingPoints;
}