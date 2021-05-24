#include "stdafx.h"
#include "pch.h"
#include "OrderScheme.h"
#include "NominalGrouping.h"
#include "OrdinalGrouping.h"
#include "IntervalGrouping.h"
#include "RatioGrouping.h"
#include <msclr\marshal_cppstd.h>

// ===loadData===
// Desc: Takes current data and finds all keys to display.
System::Void CppCLRWinformsProjekt::OrderScheme::loadData()
{
	//Set up info box
	this->infoBox->Text = "There are four different orders of data that this software supports. These orders are, \n" + 
		"N - Nominal, \nO - Ordinal, \nI - Interval, \nR - Ratio. \n\nThe possible entry data are (N, O, I, and R).\n\nDescriptions of each: \n \nNominal: \n" + 
		"The nominal level of measurement is the simplest level. Nominal means existing in name only. With the nominal" + 
		" level of measurement all we can do is to name or label things. \n \n" +
		"Ordinal: \nThe ordinal level of measurement is a more sophisticated scale than the nominal level. This scale " + 
		"enables us to order the items of interest using ordinal numbers. Ordinal numbers denote an item's position or" +
		" rank in a sequence: First, second, third, and so on. \n\n" + 
		"Interval: \nWith the interval level of measurement we have quantitative data. Like the ordinal level, the" + 
		" interval level has an inherent order. But, unlike the ordinal level, we do have the distance between intervals " + 
		" on the scale. The interval level, however, lacks a real, non-arbitrary zero.\n\n" + 
		"Ratio: \nmost sophisticated level of measurement is the ratio level. As with the ordinal and interval levels, "+
		"the data have an inherent order. And, like the interval level, we can measure the intervals between the ranks "+
		" with a measurable scale of values. But, unlike the interval level, we now have meaningful zero.";

	this->infoBox->ReadOnly = true;

	//Initilize values.
	schemeMap = new std::unordered_map<std::string, std::string>;
	keys = new std::vector<std::string>;

	try
	{
		buildSchemeTable();
	}
	catch (...)
	{
		return;
	}

}

// ===bildSchemeTable===
// Desc: Takes indavidual keys and builds table for value input.
System::Void CppCLRWinformsProjekt::OrderScheme::buildSchemeTable()
{
	this->dataGridView->ColumnCount = 2;
	this->dataGridView->Columns[0]->Name = "Attribute";
	this->dataGridView->Columns[1]->Name = "Order Type";

	//One attribute.
	this->dataGridView->RowCount = 1;

	String^ s = ("X" + attributeValue);
	this->dataGridView->Rows[0]->Cells["Attribute"]->Value = s;
	this->dataGridView->Rows[0]->Cells["Order Type"]->Value = "";
}

// ===generateValues===
// Desc: Generates values for Nominal and Ordinal.
System::Void  CppCLRWinformsProjekt::OrderScheme::generateValues(System::Object^ sender, System::EventArgs^ e)
{
	int numNominal = 1;
	std::vector<std::string> nominalBinaryVector;
	std::vector<std::string> nominalHigherVector;
	std::vector<std::string> ordinalVector;
	std::vector<std::string> IntervalVector;
	std::vector<std::string> RatioVector;
	std::vector<std::string> storage;
	std::string temp;

	//Put all values from passed data into the storage vector:
	for (int i = 0; i < numRows; i++)
	{
		DataGridViewRow^ passedDataRow = this->passedData->Rows[i];
		temp = msclr::interop::marshal_as<std::string>(passedDataRow->Cells["Key"]->Value->ToString());
		storage.push_back(temp);
	}

	//Get what type of order the user chose.
	DataGridViewRow^ curRow = this->dataGridView->Rows[0];
	String^ typeM = this->dataGridView->Rows[0]->Cells["Order Type"]->Value->ToString();
	std::string type = msclr::interop::marshal_as<std::string>(typeM);

	//Check what type of data it is:
	if (type == "N") handleNominalBinary(storage);
	else if (type == "NH") handleNominalHigher(storage);
	else if (type == "O") handleOrdinal(storage);
	else if (type == "I") handleInterval(storage);
	else if (type == "R") handleRatio(storage);
	else
	{
		//Add other types of order and error handle for user.
	}
	
	//Close the window:
	this->Close();
}

// ===allOrdinal===
// Desc: Assigns Ordianl values to all keys.
System::Void  CppCLRWinformsProjekt::OrderScheme::allOrdinalVals(System::Object^ sender, System::EventArgs^ e)
{
	std::vector<std::string> ordinalVector;

	for (int i = 0; i < numUnique; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];

		std::string temp = msclr::interop::marshal_as<std::string>(curRow->Cells["Key"]->Value->ToString());
		ordinalVector.push_back(msclr::interop::marshal_as<std::string>(curRow->Cells["Key"]->Value->ToString()));
	}

	handleOrdinal(ordinalVector);
}

// ===allNominal===
// Desc: Assigns nominal values to all keys.
System::Void  CppCLRWinformsProjekt::OrderScheme::allNominalVals(System::Object^ sender, System::EventArgs^ e) 
{

	std::vector<std::string> nominalVector;

	for (int i = 0; i < numUnique; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];
		String^ keyM = this->dataGridView->Rows[i]->Cells["Key"]->Value->ToString();
		std::string key = msclr::interop::marshal_as<std::string>(keyM);

		nominalVector.push_back(msclr::interop::marshal_as<std::string>(curRow->Cells["Key"]->Value->ToString()));
	}
	
	handleNominalBinary(nominalVector);
	this->Close();
}

// ===handleNominal===
// Desc: Assigns nominal values with binary attributes.
System::Void CppCLRWinformsProjekt::OrderScheme::handleNominalBinary(std::vector<std::string> nominalVector)
{
	//Create a new dialog window to load the scheme.
	this->Hide();
	NominalGrouping^ nominalGroupingWindow = gcnew NominalGrouping(this->dataGridView, &nominalVector);
	nominalGroupingWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	nominalGroupingWindow->ShowDialog();

	//Add results to scheme.
	std::vector<std::string>* v = nominalGroupingWindow->getNominalVector();

	//Assign values to scheme:
	for (int i = 0; i < v->size(); i = i + 2)
	{

		auto it = this->schemeMap->find(v->at(i));
		if (it != schemeMap->end())
		{
			it->second = v->at(i + 1);
		}
		else
		{
			schemeMap->insert({ v->at(i), v->at(i + 1) });
		}
	}

}

// ===handleNominalHigher===
// Desc: Assigns nominal values with higher order.
System::Void CppCLRWinformsProjekt::OrderScheme::handleNominalHigher(std::vector<std::string> nominalVector)
{

	//Increments by one for nominal data.

	std::string key;
	int numNominal = 1;

	//Assign values:
	for (int i = 0; i < nominalVector.size(); i++)
	{
		key = nominalVector[i];

		//Put in scheme list:
		auto it = this->schemeMap->find(key);
		if (it != schemeMap->end()) it->second = std::to_string(numNominal);
		else schemeMap->insert({ key, std::to_string(numNominal) });

		numNominal++;
	}
}

// ===handleOrdinal===
// Desc: Gets user input for groups and assigns values.
System::Void CppCLRWinformsProjekt::OrderScheme::handleOrdinal(std::vector<std::string> ordinalVector)
{
	//Create a new dialog window to load the scheme.
	this->Hide();
	OrdinalGrouping^ ordinalGoupingWindow = gcnew OrdinalGrouping(this->dataGridView, &ordinalVector);
	ordinalGoupingWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	ordinalGoupingWindow->ShowDialog();

	//Add results to scheme.
	std::vector<std::string>* v = ordinalGoupingWindow->getOrdinalVector();

	for (int i = 0; i < v->size(); i = i + 2)
	{

		auto it = this->schemeMap->find(v->at(i));
		if (it != schemeMap->end())
		{
			it->second = v->at(i + 1);
		}
		else
		{
			schemeMap->insert({ v->at(i), v->at(i + 1) });
		}
	}

}

// ===handleInterval===
// Desc: Gets user input for groups and assigns values.
System::Void CppCLRWinformsProjekt::OrderScheme::handleInterval(std::vector<std::string> intervalVector)
{
	//Create a new dialog window to load the scheme.
	this->Hide();
	IntervalGrouping^ intervalGoupingWindow = gcnew IntervalGrouping(this->dataGridView, &intervalVector);
	intervalGoupingWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	intervalGoupingWindow->ShowDialog();

	//Add results to scheme.
	std::vector<std::string>* v = intervalGoupingWindow->getIntervalVector();
	for (int i = 0; i < v->size(); i = i + 2)
	{

		auto it = this->schemeMap->find(v->at(i));
		if (it != schemeMap->end())
		{
			it->second = v->at(i + 1);
		}
		else
		{
			schemeMap->insert({ v->at(i), v->at(i + 1) });
		}
	}

}

// ===handleRatio===
// Desc: Gets user input for groups and assigns values.
System::Void CppCLRWinformsProjekt::OrderScheme::handleRatio(std::vector<std::string> ratioVector)
{
	//Create a new dialog window to load the scheme.
	this->Hide();
	RatioGrouping^ ratioGoupingWindow = gcnew RatioGrouping(this->dataGridView, &ratioVector);
	ratioGoupingWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	ratioGoupingWindow->ShowDialog();

	//Add results to scheme.
	std::vector<std::string>* v = ratioGoupingWindow->getRatioVector();
	for (int i = 0; i < v->size(); i = i + 2)
	{

		auto it = this->schemeMap->find(v->at(i));
		if (it != schemeMap->end())
		{
			it->second = v->at(i + 1);
		}
		else
		{
			schemeMap->insert({ v->at(i), v->at(i + 1) });
		}
	}

}

// ===getSchemeMap===
// Desc: Getter for scheme map.
std::unordered_map<std::string, std::string>* CppCLRWinformsProjekt::OrderScheme::getSchemeMap()
{
	return this->schemeMap;
}

// ===getLenNominal===
// Desc: Getter for max nominal length (Num attributes to add).
int CppCLRWinformsProjekt::OrderScheme::getLenNominal()
{
	return this->lenNominal;
}

//===toBinary===
// Desc: Converts to binary.
std::string CppCLRWinformsProjekt::OrderScheme::toBinary(int n)
{
	std::string r;
	while (n != 0) { r = (n % 2 == 0 ? "0" : "1") + r; n /= 2; }
	return r;
}