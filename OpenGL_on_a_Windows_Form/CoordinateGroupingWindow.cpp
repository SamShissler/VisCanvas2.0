#include "stdafx.h"
#include <msclr\marshal_cppstd.h>
#include "CoordinateGroupingMainWindow.h"
#include "CoordinateGroupMaker.h"
#include "CoordinateGroupGeneralizer.h"
#include "UserInputPopUp.h"


void CppCLRWinformsProjekt::CoordinateGroupingMainWindow::setUpData()
{
	coordianteGroups = new vector<vector<int>>();
	groupNames = new vector<string>();
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Group";
	this->dataGridView->Columns[1]->Name = "Coordinates";
}

System::Void CppCLRWinformsProjekt::CoordinateGroupingMainWindow::generateRules(System::Object^ sender, System::EventArgs^ e)
{
	this->Close();
}

System::Void CppCLRWinformsProjekt::CoordinateGroupingMainWindow::createGroup(System::Object^ sender, System::EventArgs^ e)
{
	//Ask User for Name Of Group
	UserInputPopUp^ popup = gcnew UserInputPopUp("Group to Create", "What would you like to name this group?");
	popup->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	popup->ShowDialog();

	String^ resultFromUserM = popup->getResult();

	CoordinateGroupMaker^ groupMaker = gcnew CoordinateGroupMaker(this->avalibleCoordinates);
	groupMaker->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	groupMaker->ShowDialog();

	vector<int> retrivedSelectedCoords = *groupMaker->getSelectedCoordinates();

	//Remove dimensions from avalible.
	bool removed = true;
	while (removed)
	{
		removed = false;

		for (auto it = avalibleCoordinates->begin(); it != avalibleCoordinates->end(); it++)
		{
			for (int i = 0; i < retrivedSelectedCoords.size(); i++)
			{
				if (*it == retrivedSelectedCoords.at(i))
				{
					avalibleCoordinates->erase(it);
					removed = true;
					break;
				}
			}
			if (removed) break;
		}

	}

	//Add group to overall group.
	this->groupNames->push_back(msclr::interop::marshal_as<std::string>(resultFromUserM));
	this->dataGridView->Rows->Add();
	this->dataGridView->Rows[this->dataGridView->RowCount - 1]->Cells["Group"]->Value = resultFromUserM;
	String^ coordinates;
	for (int i = 0; i < retrivedSelectedCoords.size() - 1; i++)
	{
		coordinates += "" + retrivedSelectedCoords.at(i) + ", ";
	}
	coordinates += "" + retrivedSelectedCoords.at(retrivedSelectedCoords.size() - 1);
	this->dataGridView->Rows[this->dataGridView->RowCount - 1]->Cells["Coordinates"]->Value = coordinates;
	this->coordianteGroups->push_back(retrivedSelectedCoords);
}

System::Void CppCLRWinformsProjekt::CoordinateGroupingMainWindow::generalizeGroups(System::Object^ sender, System::EventArgs^ e)
{
	//Ask User for Name Of Group
	UserInputPopUp^ popup = gcnew UserInputPopUp("Generalized Group Creation", "What would you like to name this group?");
	popup->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	popup->ShowDialog();

	String^ resultFromUserM = popup->getResult();

	CoordinateGroupGeneralizer^ generalizer = gcnew CoordinateGroupGeneralizer(this->coordianteGroups, this->groupNames);
	generalizer->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	generalizer->ShowDialog();

	vector<int> selectedCoordsFromGroups = *generalizer->getSelectedCoordinates();
	vector<string>* groupsUsed = generalizer->getGroupsUsed();
	
	//Remove groups used.
	bool removed = true;
	while (removed)
	{
		removed = false;

		auto itcg = coordianteGroups->begin();
		for (auto it = groupNames->begin(); it != groupNames->end(); it++)
		{
			for (int i = 0; i < groupsUsed->size(); i++)
			{
				if (*it == groupsUsed->at(i))
				{
					groupNames->erase(it);
					coordianteGroups->erase(itcg);
					removed = true;
					break;
				}
			}
			if (removed) break;
			itcg++;
		}
	}
	
	//Add group and re build table:
	dataGridView->RowCount = 0; // clear.

	groupNames->push_back(msclr::interop::marshal_as<std::string>(resultFromUserM));
	coordianteGroups->push_back(selectedCoordsFromGroups);
	dataGridView->RowCount = groupNames->size();

	for(int i = 0; i < groupNames->size(); i++)
	{
		string names = groupNames->at(i);
		String^ name = gcnew String(names.c_str());
		this->dataGridView->Rows[i]->Cells["Group"]->Value = name;
		
		String^ coordinates = "";

		for (int j = 0; j < coordianteGroups->at(i).size() - 1; j++)
		{
			coordinates += "" + coordianteGroups->at(i).at(j) + ", ";
		}
		coordinates += "" + coordianteGroups->at(i).at(coordianteGroups->at(i).size() - 1);

		this->dataGridView->Rows[i]->Cells["Coordinates"]->Value = coordinates;
	}
}

vector<vector<int>>* CppCLRWinformsProjekt::CoordinateGroupingMainWindow::getCoordinateGroups()
{
	return this->coordianteGroups;
}