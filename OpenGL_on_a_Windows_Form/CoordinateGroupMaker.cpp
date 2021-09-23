#include "stdafx.h"
#include "CoordinateGroupMaker.h"

System::Void CppCLRWinformsProjekt::CoordinateGroupMaker::onConfirm(System::Object^ sender, System::EventArgs^ e)
{
	this->selectedCoordinates = new vector<int>();
	string s = "X";

	for (int i = 0; i < this->dataGridView->RowCount; i++)
	{
		if (this->dataGridView->Rows[i]->Cells["Selected / Unselected"]->Value != nullptr)
		{
			if (this->dataGridView->Rows[i]->Cells["Selected / Unselected"]->Value->ToString() == "X")
			{
				this->selectedCoordinates->push_back(avalibleCoords->at(i));
			}
		}
	}

	this->Close();
}

void CppCLRWinformsProjekt::CoordinateGroupMaker::buildTable()
{
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Avalible Coordinate";
	this->dataGridView->Columns[1]->Name = "Selected / Unselected";

	this->dataGridView->RowCount = avalibleCoords->size();

	for (int i = 0; i < avalibleCoords->size(); i++)
	{
		this->dataGridView->Rows[i]->Cells["Avalible Coordinate"]->Value = avalibleCoords->at(i);
	}
}

vector<int>* CppCLRWinformsProjekt::CoordinateGroupMaker::getSelectedCoordinates()
{
	return this->selectedCoordinates;
}