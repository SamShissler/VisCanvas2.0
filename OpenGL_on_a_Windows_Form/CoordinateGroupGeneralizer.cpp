#include "stdafx.h"
#include "CoordinateGroupGeneralizer.h"

System::Void CppCLRWinformsProjekt::CoordinateGroupGeneralizer::onConfirm(System::Object^ sender, System::EventArgs^ e)
{
	string s = "X";
	generalizedGroupCoordinates = new vector<int>();
	groupsUsed = new vector<string>();

	for (int i = 0; i < this->dataGridView->RowCount; i++)
	{
		if (this->dataGridView->Rows[i]->Cells["Selected / Unselected"]->Value != nullptr)
		{
			if (this->dataGridView->Rows[i]->Cells["Selected / Unselected"]->Value->ToString() == "X")
			{
				groupsUsed->push_back(groupNames->at(i));
				vector<int> curGroupCords = this->coordinateGroups->at(i);
				for (int j = 0; j < curGroupCords.size(); j++)
				{
					generalizedGroupCoordinates->push_back(curGroupCords.at(j));
				}
			}
		}
	}

	this->Close();
}

void CppCLRWinformsProjekt::CoordinateGroupGeneralizer::buildTable()
{
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Group Name";
	this->dataGridView->Columns[1]->Name = "Selected / Unselected";

	this->dataGridView->RowCount = groupNames->size();

	for (int i = 0; i <groupNames->size(); i++)
	{
		string sname = groupNames->at(i);
		String^ name = gcnew String(sname.c_str());
		this->dataGridView->Rows[i]->Cells["Group Name"]->Value = name;
	}
}

vector<int>* CppCLRWinformsProjekt::CoordinateGroupGeneralizer::getSelectedCoordinates()
{
	return this->generalizedGroupCoordinates;
}

vector<string>* CppCLRWinformsProjekt::CoordinateGroupGeneralizer::getGroupsUsed()
{
	return this->groupsUsed;
}