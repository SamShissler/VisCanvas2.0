#include "stdafx.h"
#include "NonMonotonicSelector.h"

System::Void CppCLRWinformsProjekt::NonMonotonicSelector::onConfirm(System::Object^ sender, System::EventArgs^ e)
{
	this->selectedCoordinates = new vector<int>();
	string s = "X";

	for (int i = 0; i < this->dataGridView->RowCount; i++)
	{
		if (this->dataGridView->Rows[i]->Cells["Selected / Unselected"]->Value != nullptr)
		{
				this->selectedCoordinates->push_back(avalibleCoords->at(i));
		}
	}

	this->Close();
}

void CppCLRWinformsProjekt::NonMonotonicSelector::buildTable()
{
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Dimension";
	this->dataGridView->Columns[1]->Name = "Selected / Unselected";

	this->dataGridView->RowCount = avalibleCoords->size();

	for (int i = 0; i < avalibleCoords->size(); i++)
	{
		this->dataGridView->Rows[i]->Cells["Dimension"]->Value = avalibleCoords->at(i);
	}
}

vector<int>* CppCLRWinformsProjekt::NonMonotonicSelector::getSelectedCoordinates()
{
	return this->selectedCoordinates;
}