#include "stdafx.h"
#include "pch.h"
#include "RatioGrouping.h"
#include "RatioGroupStarting.h"
#include <msclr\marshal_cppstd.h>
#include <list>
#include <cliext/vector>
using namespace System::Collections::Generic;

// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::RatioGrouping::buildSchemeTable()
{
	this->infoBox->Text = "For Ratio data you need to supply 4 different values.\n1.) Group number.\n2.) " +
	"Position in group\n3.) Interval to next datapoint.\n4.)Starting Point (non-arbitrary zero)\nNumber 4 will be collected on the next page. "+
		"\n\nA sample input would be:\nGroup number: 1\n" +
		"Position: 3\nInterval to Next: 13\nFor a data point that is in group 1, at position 3, and is 13 units " +
		"away from data point at position 4.\n\nSince the last datapoint in each group does not need an interval, you " +
		"can leave it blank."; 

	this->infoBox->ReadOnly = true;

	toAdd = new std::vector<std::string>;

	this->dataGridView->ColumnCount = 3;
	this->dataGridView->Columns[0]->Name = "Key";
	this->dataGridView->Columns[1]->Name = "Position";
	this->dataGridView->Columns[2]->Name = "Interval to Next";


	for (int i = 0; i < this->ids->size(); i++)
	{
		this->dataGridView->Rows->Add();
		std::string s = this->ids->at(i);
		this->dataGridView->Rows[i]->Cells["Key"]->Value = gcnew String(s.c_str());
		this->dataGridView->Rows[i]->Cells["Position"]->Value = "";
		this->dataGridView->Rows[i]->Cells["Interval to Next"]->Value = "";
	}

}

// ===updateScheme===
// Desc: Updates unordered map with new value for kvpair.
System::Void CppCLRWinformsProjekt::RatioGrouping::updateScheme(System::Object^ sender, System::EventArgs^ e)
{
	toAdd = new std::vector<string>();

	//list to order the values.
	List<RatioData^>^ ratioList = gcnew List<RatioData^>();

	//Itterate over data:
	for (int j = 0; j < this->dataGridView->RowCount; j++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[j];

		//Get the values for the current row.
		String^ keyM = this->dataGridView->Rows[j]->Cells["Key"]->Value->ToString();
		String^ posM = this->dataGridView->Rows[j]->Cells["Position"]->Value->ToString();
		String^ interM = this->dataGridView->Rows[j]->Cells["Interval to Next"]->Value->ToString();

		std::string position = msclr::interop::marshal_as<std::string>(posM);
		std::string interval = msclr::interop::marshal_as<std::string>(interM);

		int positionVal = stoi(position);
		float intervalVal = stof(interval);

		//Add the interval data to the list.
		ratioList->Add(gcnew RatioData(keyM, positionVal, intervalVal));
	}

	//Sort by position:
	for (int j = 0; j < ratioList->Count; j++)
	{

		for (int k = 0; k < ratioList->Count - j - 1; k++)
		{
			int p1 = ratioList[k]->position;
			int p2 = ratioList[k + 1]->position;

			if (p1 > p2)
			{
				RatioData^ temp = ratioList[k];
				ratioList[k] = ratioList[k + 1];
				ratioList[k + 1] = temp;
			}

		}
	}

	//Get starting point for group:
	RatioGroupStarting^ RatioStartingWindow = gcnew RatioGroupStarting();
	RatioStartingWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	RatioStartingWindow->ShowDialog();
	std::vector<std::string>* sp = RatioStartingWindow->getStartingPoints();
	std::vector<std::string>* itf = RatioStartingWindow->getIntervalToFirst();

	//Get starting point and interval to first.
	float starting = stof(sp->at(0));
	float toFirst = stof(itf->at(0));

	std::string temp;
	temp = msclr::interop::marshal_as<std::string>((starting + toFirst).ToString());
	toAdd->push_back(msclr::interop::marshal_as<std::string>(ratioList[0]->key->ToString()));
	toAdd->push_back(temp);

	float nextVal = starting + toFirst + ratioList[0]->intervalToNext;
	for (int j = 1; j < ratioList->Count; j++)
	{
		RatioData^ curData = ratioList[j];

		toAdd->push_back(msclr::interop::marshal_as<std::string>(ratioList[j]->key->ToString()));
		toAdd->push_back(msclr::interop::marshal_as<std::string>(nextVal.ToString()));

		nextVal = nextVal + curData->intervalToNext;
	}

	//Close the window:
	this->Close();
}

// ===getSchemeMap===
// Desc: Getter for scheme map.
std::vector<std::string>* CppCLRWinformsProjekt::RatioGrouping::getRatioVector()
{
	return this->toAdd;
}