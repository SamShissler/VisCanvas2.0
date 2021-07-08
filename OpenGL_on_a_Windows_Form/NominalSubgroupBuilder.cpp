#include "stdafx.h"
#include "pch.h"
#include "NominalSubGroupBuilder.h"


// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::NominalSubGroupBuilder::buildTable()
{
	try {
		//Set up subCat box.
		this->subCatBox->ReadOnly = true;
		this->subCatBox->Text = "Group Sub-Categories: \n";

		for (int i = 0; i < this->group->subCats.size(); i++)
		{
			this->subCatBox->Text += gcnew String(this->group->subCats.at(i).groupName.c_str()) + "\n";
		}

		if (this->group->subCats.size() == 0)
		{
			this->subCatBox->Text += "None.";
		}

		//Add columns
		if (this->group->groupName == "Initial Group")
		{
			//Dont show inital data in groups because it is repetitive.
			this->dataGridView->ColumnCount = 1;
			this->dataGridView->Columns[0]->Name = this->Name;
		}
		else
		{
			this->dataGridView->ColumnCount = 2;
			this->dataGridView->Columns[0]->Name = this->Name;
			this->dataGridView->Columns[1]->Name = "List of Values in Group";
		}

		this->dataGridView->RowCount = 0;

		//Add rows to how many subgroups there are.
		for (int i = 0; i < this->group->subGroups.size(); i++)
		{
			this->dataGridView->Rows->Add();
		}

		//Add values to the rows.
		for (int i = 0; i < this->group->subGroups.size(); i++)
		{
			//Get the current group.
			NSG curG = this->group->subGroups.at(i);

			//Assign values.
			this->dataGridView->Rows[i]->Cells[this->Name]->Value = gcnew String(curG.groupName.c_str());


			//Only do if this is not the initial group.
			if (this->group->groupName != "Initial Group")
			{
				//Create list string:
				string combinedList = "";

				for (int j = 0; j < curG.values.size(); j++)
				{

					combinedList += curG.values.at(j);

					if (j != curG.values.size() - 1)
					{
						combinedList += ", \n";
					}

				}


				this->dataGridView->Rows[i]->Cells["List of Values in Group"]->Value = gcnew String(combinedList.c_str());
			}

		}

	}
	catch (...)
	{
		//To Catch GUI errors.
	}
}

System::Void CppCLRWinformsProjekt::NominalSubGroupBuilder::addSubGroup(System::Object^ sender, System::EventArgs^ e)
{
	//Ask User for Name Of SubGroup.
	UserInputPopUp^ popup = gcnew UserInputPopUp("Category To Create", "What Subgroup Category do you want to create?");
	popup->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	popup->ShowDialog();

	String^ resultFromUserM = popup->getResult();
	string resultFromUser = msclr::interop::marshal_as<std::string>(resultFromUserM);

	//Create the new group.
	NSG* createdGroup = new NSG(resultFromUser, group->values, group->subGroups);

	//Store previous group.
	NSG* prev = this->group;
	prevS->push(prev);

	//Make new group the current group.
	this->group = createdGroup;

	//Allow user to make the group using the group values from this current group.
	NSGGroupAssingment^ nominalGroupingWindow = gcnew NSGGroupAssingment(this->dataGridView, &group->values, &group->subGroups);
	nominalGroupingWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	nominalGroupingWindow->ShowDialog(); 

	//Get the new groups and set them as sub groups to our group.
	vector<NSG>* groupsData = nominalGroupingWindow->getNominalVector();
	group->subGroups = *groupsData;

	//Update curent groups scheme map.

	//Go over subgroups.
	for (int i = 0; i < group->subGroups.size(); i++)
	{
		//Go over subgroup values.
		for (int j = 0; j < group->subGroups.at(i).values.size(); j++)
		{
			group->schemeMap.push_back(group->subGroups.at(i).groupName); //Group (key)
			group->schemeMap.push_back(group->subGroups.at(i).values.at(j)); //Value
		}

	}

	//Add prev groups sub cat.
	prev->subCats.push_back(*this->group);

	//Set name of window to group and display data.
	this->Name = gcnew String(group->groupName.c_str());
	this->Text = gcnew String(group->groupName.c_str()) + " (Category)";
	buildTable();
}

System::Void CppCLRWinformsProjekt::NominalSubGroupBuilder::selectGroup(System::Object^ sender, System::EventArgs^ e)
{
	//Retrive the name of the subgroup the user wants to use.
	UserInputPopUp^ popup = gcnew UserInputPopUp("Group To Select", "What subgroup do you want to use to create binary\nattribtes?");
	popup->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	popup->ShowDialog();

	String^ resultFromUserM = popup->getResult();
	string resultFromUser = msclr::interop::marshal_as<std::string>(resultFromUserM);

	if (this->group->groupName == resultFromUser)
	{
		finalScheme = &group->schemeMap;
	}
	else
	{
		//Seach for the correct scheme map.
		while (!prevS->empty())
		{
			if (prevS->top()->groupName == resultFromUser)
			{
				finalScheme = &prevS->top()->schemeMap;
				break;
			}
			else
			{
				prevS->pop();
			}

		}

	}

	//Close The Window.
	this->Close();

}

System::Void CppCLRWinformsProjekt::NominalSubGroupBuilder::back(System::Object^ sender, System::EventArgs^ e)
{

	NSG *toChange = prevS->top();
	prevS->pop();

	this->group = toChange;

	this->Name = gcnew String(group->groupName.c_str());
	this->Text = gcnew String(group->groupName.c_str());

	buildTable();

	return;

}

System::Void CppCLRWinformsProjekt::NominalSubGroupBuilder::view(System::Object^ sender, System::EventArgs^ e)
{

	//Retrive the name of the subgroup the user wants to use.
	UserInputPopUp^ popup = gcnew UserInputPopUp("Category To View", "What category do you want to view?");
	popup->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	popup->ShowDialog();

	String^ resultFromUserM = popup->getResult();
	string resultFromUser = msclr::interop::marshal_as<std::string>(resultFromUserM);

	//Iterate over sub cats and search for the one the user requested.
	for (int i = 0; i < group->subCats.size(); i++)
	{

		if (group->subCats.at(i).groupName == resultFromUser)
		{
			//Store previous group.
			NSG* prev = this->group;
			prevS->push(prev);

			this->group = &prev->subCats.at(i);

			this->Name = gcnew String(group->groupName.c_str());
	        this->Text = gcnew String(group->groupName.c_str());

			//Display searched group.
			buildTable();
			break;

		}


	}

	return;
}


std::vector<std::string> CppCLRWinformsProjekt::NominalSubGroupBuilder::getSchemeMap()
{
	return *finalScheme;
}
	