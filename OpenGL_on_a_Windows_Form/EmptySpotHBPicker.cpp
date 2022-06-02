#include "stdafx.h"
#include "pch.h"
#include "EmptySpotHBPicker.h"
#include <iostream>
#include <fstream> 
#include <msclr\marshal_cppstd.h>
#include <vector>
#include "KVPair.h"
#include "Scheme.h"
#include "ClassScheme.h"
#include <cstdlib>


System::Void CppCLRWinformsProjekt::EmptySpotHBPicker::drawTables() {

	//Read data:
	std::stringstream test(*esHBs);
	std::string segment;
	std::vector<std::string> seglist;

	numRows = -1;
	numDimensions = 0;
	int curColumn = 0;
	this->dataGridView->RowCount = 1;
	this->dataGridView->ColumnCount = 1;

	//Get num of columns:
	getline(test, segment);
	for (std::size_t i = 0; i < segment.length(); i++) if (segment[i] == ',') numDimensions++;

	//Add columns:
	this->dataGridView->ColumnCount = numDimensions;

	//Name columns:
	int pos = 0;
	int curCell = 0;
	std::string token;
	std::vector<std::string> colNames = std::vector<std::string>();
	while ((pos = segment.find(",")) != std::string::npos)
	{
		//Get cur token:
		token = segment.substr(0, pos);
		this->dataGridView->Columns[curCell]->Name = gcnew String(token.c_str());
		colNames.push_back(token);
		//Erase used data and move to next cell:
		segment.erase(0, pos + 1);
		curCell++;
	}

	//this->dataGridView->Columns[numDimensions + 1]->Name = ("class");
	
	//Generate rows:
	while (getline(test, segment))
	{
		this->dataGridView->Rows->Add();
		numRows++;
	}

	std::stringstream test1(*esHBs);
	// strip column names from data
	getline(test1, segment);
	//int rowSpan = 0;
	int esCase = -1;
	int maxInd = -1;
	double max = -1.0;
	int start = 0;
	int prev = -1;
	std::vector<int> recInds = std::vector<int>();
	//Fill rows:
	for (int i = 0; i <= numRows; i++)
	{
		//Get data from current line:
		getline(test1, segment);

		//Identify row:
		DataGridViewRow^ r = this->dataGridView->Rows[i];

		//Parse String and set values:
		int pos = 0;
		int curCell = 0;
		std::string token;
		while ((pos = segment.find(",")) != std::string::npos)
		{
			//Get cur token:
			token = segment.substr(0, pos);
			if (curCell == 0 && esCase != std::stoi(token)) {
				esCase = std::stoi(token);
			}
			r->Cells[gcnew String(colNames[curCell].c_str())]->Value = gcnew String(token.c_str());
			//Erase used data and move to next cell:
			segment.erase(0, pos + 1);
			curCell++;
		}
	
		// Checks ESCase index.  If it changes add saved rows to dataGridView with highlighted recommendations.
		if (curCell == 1 && esCase > -1 ) {
			prev = esCase;
			// Converts ES Case to int and saves in esCase
			try {
				esCase = std::stoi(msclr::interop::marshal_as<std::string>(r->Cells[0]->Value->ToString()));
			}
			catch (...) {
				// First row of each ESCase should be just a number but in case it has a space at the end.
				std::string tmp = msclr::interop::marshal_as<std::string>(r->Cells[0]->Value->ToString());
				tmp.resize(tmp.length() - 1);
				esCase = std::stoi(tmp);
			}

			// Colors first row of each ESCase
			for (int j = 0; j < dataGridView->ColumnCount; j++) {
				r->Cells[j]->Style->BackColor = Color::LightGray;
				
			}
			if (maxInd > -1) {
				recInds.push_back(maxInd);
				for (int j = 0; j < dataGridView->ColumnCount; j++) {
					this->dataGridView->Rows[maxInd]->Cells[j]->Style->BackColor = Color::LightGreen;
				}
			}
			max = -1.0;
			maxInd = -1;
		} // If ESCase does not change, check compatability and update recommendation
		else {
			double compat = std::stod(msclr::interop::marshal_as<std::string>(r->Cells[" Compatibility %"]->Value->ToString()));
			if (std::stoi(msclr::interop::marshal_as<std::string>(r->Cells[" ES Case Class"]->Value->ToString())) !=
				std::stoi(msclr::interop::marshal_as<std::string>(r->Cells[" HB Dominant Class"]->Value->ToString()))) {
				// Multiply HB purity by an arbitrary threshold to favor higher purity over higher compatability. 
				// 1.0 would signify no bias.
				compat += 100 - .7 * std::stod(msclr::interop::marshal_as<std::string>(r->Cells[" HB Dominant Class Purity %"]->Value->ToString()));
			}
			else {
				compat += std::stod(msclr::interop::marshal_as<std::string>(r->Cells[" HB Dominant Class Purity %"]->Value->ToString()));
			}
			if (compat > max) {
				max = compat;
				maxInd = i;
			}
		}
	}
	// Highlight recommended for last ESCase
	if (maxInd > -1) {
		recInds.push_back(maxInd);
		for (int j = 0; j < dataGridView->ColumnCount; j++) {
			this->dataGridView->Rows[maxInd]->Cells[j]->Style->BackColor = Color::LightGreen;
		}
	}
}

// ===submitOrderButton_Click===:
// Desc: Submits selections/changes made to table.
System::Void CppCLRWinformsProjekt::EmptySpotHBPicker::submitButton_Click(System::Object^ sender, System::EventArgs^ e)
{
	Int32 selectedRowCount = this->dataGridView->SelectedRows->Count;
	if (selectedRowCount > 0) {
		// Display Error message for too few selections. No selections were made.
	}
	else if (selectedRowCount <= emptys->size()) {
		// Display Error Message for too many selections
		//MessageBox.show("Some Text", "Some title", MessageBoxButtons.OK, MessageBoxIcon.Error);
	}
	else
	{
		int hbind = 0;
		int esind = 0;
		std::vector<int> esinds;

		for (int i = 0; i < selectedRowCount; i++)
		{
			// index of selected row mod (number of hbs + 1) gives index of empty spot
			esind = (int)this->dataGridView->SelectedRows[i]->Index % ((int)this->dataGridView->RowCount / emptys->size());
			// (index of selected row mod number of cases) - 1 gives index of hyperblock
			hbind = ((int)this->dataGridView->SelectedRows[i]->Index % emptys->size()) - 1;
			if (hbind >= 0 && std::find(esinds.begin(),esinds.end(),esind) != esinds.end()) {
				(*emptyhbs)[esind] = hbind;
			}
		}
		// Return New HBs

		// Close window
		//this.Close();
		
		//Need to find somewhere else for this
		//bool done = TRUE;
		//for (int i = 0; i < (*emptyhbs).size(); i++) {
		//	if ((*emptyhbs)[i] < 0) {
		//		done = FALSE;
		//		break;
		//	}
		//}
		//if (done) {
		//	return;
		//}
	}
}

// ===descendingOrderButton_Click===:
// Desc: Handles sorting in descending order.
System::Void CppCLRWinformsProjekt::EmptySpotHBPicker::visualizeButton_Click(System::Object^ sender, System::EventArgs^ e)
{
	//Get input from user:
	//String^ inputM = this->columnInputFeild->Text;

	//Sort:
	//sortDesending(inputM);
}

// ===sortAscending===:
// Desc: sort.
System::Void CppCLRWinformsProjekt::EmptySpotHBPicker::submit(System::String^ column)
{
	KVPair** arrKV = new KVPair * [numRows + 1];
	std::string temp;
	std::string notM;

	//Create array:
	for (int i = 0; i < numRows + 1; i++)
	{
		arrKV[i] = new KVPair();
	}

	//Go over rows:
	for (int i = 0; i <= numRows; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];
		temp += msclr::interop::marshal_as<std::string>(curRow->Cells["ID"]->Value->ToString());
		for (int j = 1; j <= numDimensions; j++)
		{
			temp += "," + msclr::interop::marshal_as<std::string>(curRow->Cells["X" + j]->Value->ToString());
		}

		notM = msclr::interop::marshal_as<std::string>(curRow->Cells[column]->Value->ToString());
		arrKV[i]->key += notM;
		arrKV[i]->rowData = temp;
		temp = "";
	}

	//Sort:
	for (int i = 0; i <= numRows - 1; i++)
	{
		for (int j = 0; j < numRows - i; j++)
		{
			float keyj1 = stof(arrKV[j]->key);
			float keyjp1 = stof(arrKV[j + 1]->key);

			if (keyj1 > keyjp1)
			{
				KVPair* temp = arrKV[j];
				arrKV[j] = arrKV[j + 1];
				arrKV[j + 1] = temp;
			}
		}
	}

	//Set Rows Equal:
	for (int i = 0; i <= numRows; i++)
	{
		std::string curKey = arrKV[i]->key;
		std::string curValue = arrKV[i]->rowData;
		DataGridViewRow^ r = this->dataGridView->Rows[i];

		//Parse String and set values:
		int pos = 0;
		int curCell = 0;
		std::string token;
		while ((pos = curValue.find(",")) != std::string::npos)
		{
			//Get cur token:
			token = curValue.substr(0, pos);

			//Save to sorting table:
			sortingTable[i][curCell] = token;

			//Add to cell:
			if (curCell == 0)
			{
				r->Cells["ID"]->Value = gcnew String(token.c_str());
			}
			else
			{
				r->Cells["X" + curCell]->Value = gcnew String(token.c_str());
			}

			//Erase used data and move to next cell:
			curValue.erase(0, pos + 1);
			curCell++;
		}

		//Account for last:
		r->Cells["X" + curCell]->Value = gcnew String(curValue.c_str());
	}

}

// ===sortdescending===:
// Desc: sort.
System::Void CppCLRWinformsProjekt::EmptySpotHBPicker::visualize(System::String^ column)
{
	KVPair** arrKV = new KVPair * [numRows + 1];
	std::string temp;
	std::string notM;

	//Create array:
	for (int i = 0; i < numRows + 1; i++)
	{
		arrKV[i] = new KVPair();
	}

	//Go over rows:
	for (int i = 0; i <= numRows; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];
		temp += msclr::interop::marshal_as<std::string>(curRow->Cells["ID"]->Value->ToString());
		for (int j = 1; j <= numDimensions + 1; j++)
		{
			temp += "," + msclr::interop::marshal_as<std::string>(curRow->Cells["X" + j]->Value->ToString());
		}

		notM = msclr::interop::marshal_as<std::string>(curRow->Cells[column]->Value->ToString());
		arrKV[i]->key += notM;
		arrKV[i]->rowData = temp;
		temp = "";
	}

	//Sort:
	for (int i = 0; i <= numRows - 1; i++)
	{
		for (int j = 0; j < numRows - i; j++)
		{
			float keyj1 = stof(arrKV[j]->key);
			float keyjp1 = stof(arrKV[j + 1]->key);

			if (keyj1 < keyjp1)
			{
				KVPair* temp = arrKV[j];
				arrKV[j] = arrKV[j + 1];
				arrKV[j + 1] = temp;
			}
		}
	}

	//Set Rows Equal:
	for (int i = 0; i <= numRows; i++)
	{
		std::string curKey = arrKV[i]->key;
		std::string curValue = arrKV[i]->rowData;
		DataGridViewRow^ r = this->dataGridView->Rows[i];

		//Parse String and set values:
		int pos = 0;
		int curCell = 0;
		std::string token;
		while ((pos = curValue.find(",")) != std::string::npos)
		{
			//Get cur token:
			token = curValue.substr(0, pos);

			//Save to sorting table:
			sortingTable[i][curCell] = token;

			//Add to cell:
			if (curCell == 0)
			{
				r->Cells["ID"]->Value = gcnew String(token.c_str());
			}
			else
			{
				r->Cells["X" + curCell]->Value = gcnew String(token.c_str());
			}

			//Erase used data and move to next cell:
			curValue.erase(0, pos + 1);
			curCell++;
		}

		//Account for last:
		r->Cells["X" + curCell]->Value = gcnew String(curValue.c_str());
	}

}