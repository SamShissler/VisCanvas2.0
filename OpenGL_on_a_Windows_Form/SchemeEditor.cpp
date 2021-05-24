#include "stdafx.h"
#include "pch.h"
#include "SchemeEditor.h"
#include <iostream>
#include <fstream> 
#include <msclr\marshal_cppstd.h>
#include <vector>
#include "KVPair.h"
#include "Scheme.h"
#include "ClassScheme.h"
#include <cstdlib>

// ===openToolStripMenuItem_Click===:
// Desc: Handles opening and reading data.
System::Void CppCLRWinformsProjekt::SchemeEditor::openToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	//Open File:
	OpenFileDialog^ ofd = gcnew OpenFileDialog;
	ofd->InitialDirectory = ""; // NOTE "c:\\" for future reference.
	ofd->Filter = "All Files (*.*)|*.*";
	ofd->FilterIndex = 1;
	ofd->RestoreDirectory = true;

	String^ path;
	this->path = path;

	if (ofd->ShowDialog() == System::Windows::Forms::DialogResult::OK) path = ofd->FileName;

	//Read data:
	std::ifstream inFile;
	std::string inDir = msclr::interop::marshal_as<std::string>(path);
	std::string temp;
	inFile.open(inDir);
	numRows = 0;
	numDimensions = 0;
	int curColumn = 0;
	this->dataGridView->RowCount = 1;
	this->dataGridView->ColumnCount = 1;

	//Get num of columns:
	getline(inFile, temp);
	for (std::size_t i = 0; i < temp.length(); i++) if (temp[i] == ',') numDimensions++;

	//Add columns:
	this->dataGridView->ColumnCount = numDimensions + 2;
	
	//Name columns:
	this->dataGridView->Columns[0]->Name = "ID";
	for (int i = 1; i <= numDimensions; i++)
	{
		this->dataGridView->Columns[i]->Name = ("X" + i);
	}

	this->dataGridView->Columns[numDimensions + 1]->Name = ("class");

	//Generate rows:
	while (getline(inFile, temp))
	{
		this->dataGridView->Rows->Add();
		numRows++;
	}
	inFile.close();

	//Generate string 2-D array used for sorting:
	sortingTable = new std::string * [numRows];
	for (int i = 0; i <= numRows; i++)
	{
		std::string* curRow = new std::string[numDimensions + 2];
		curRow[0] = i + 1;
		sortingTable[i] = curRow;
	}

	//Fill rows:
	inFile.open(inDir);
	for(int i = 0; i <= numRows; i++)
	{
		//Get data from current line:
		getline(inFile, temp);
		
		//Identify row:
		DataGridViewRow^ r = this->dataGridView->Rows[i];

		//Set ID:
		r->Cells["ID"]->Value = i + 1;

		//Parse String and set values:
		int pos = 0;
		int curCell = 1;
		std::string token;
		while ((pos = temp.find(",")) != std::string::npos)
		{
			//Get cur token:
			token = temp.substr(0, pos);

			//Save to sorting table:
			sortingTable[i][curCell] = token;

			//Add to cell:
			r->Cells["X" + curCell]->Value = gcnew String(token.c_str());

			//Erase used data and move to next cell:
			temp.erase(0, pos + 1);
			curCell++;
		}

		//Account for the last cell.
		r->Cells["class"]->Value = gcnew String(temp.c_str());
		sortingTable[i][curCell] = temp;
	}
}

// ===loadSchemeToolStripMenuItem_Click===:
// Desc: Opens scheme editor window and applys scheme.
System::Void CppCLRWinformsProjekt::SchemeEditor::loadSchemeToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{

	this->schemeMap = new std::unordered_map<std::string, std::string>();
	//Do for each column.

	//Reset attribute offset
	attributeAddOffset = 0;

	//Go over each column.
	for (int i = 1; i <= numDimensions - attributeAddOffset; i++)
	{
		String^ toMarshal;
		int numUnique = 0;
		std::vector<std::string>* keys = new std::vector<std::string>();

		//Go over each row.
		for (int j = 0; j < numRows + 1; j++)
		{
			DataGridViewRow^ curRow = this->dataGridView->Rows[j];

			//Get data in current column: Data:
			toMarshal = curRow->Cells["X" + (i + attributeAddOffset)]->Value->ToString();
			std::string curVal = msclr::interop::marshal_as<std::string>(toMarshal);

			//If the key is not in our scheme map:
			if (schemeMap->find(curVal) == schemeMap->end())
			{
				schemeMap->insert({ curVal, "" });
				keys->push_back(curVal);
				numUnique++;
			}

		}

		//At this point we have the values for the column. We want to build a window, then apply the values.

		//Launch the Scheme window:
		Scheme^ loadSchemeWindow = gcnew Scheme(this->schemeMap, keys, numUnique, numRows, numDimensions, i + attributeAddOffset);
		loadSchemeWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
		loadSchemeWindow->ShowDialog();

		//Get the scheme map values from the scheme window.
		this->schemeMap = loadSchemeWindow->getSchemeMap();

		//Apply the scheme to the specific row.
		this->applyScheme(i + attributeAddOffset);

		//Clear the scheme map for next column.
		this->schemeMap->clear();
	}

	//Handle Nominals.

}

// ===applyScheme===:
// Desc: Applys the scheme to the existing data.
System::Void CppCLRWinformsProjekt::SchemeEditor::applyScheme(int attributeVal)
{
	try
	{
		//Get the first scheme value of the attribute.
		string firstValue = schemeMap->at(msclr::interop::marshal_as<std::string>(
			this->dataGridView->Rows[0]->Cells["X" + (attributeVal)]->Value->ToString()
		));

		//Keep track of class vals.
		vector<string> classVals;

		//Check to see if values are in binarys so that we must add attributes.
		if (firstValue[firstValue.size() - 1] == 'b')
		{
			int numAttributesToAdd = firstValue.size() - 2; //Minus 2 for using the already existing column.

			//Add attributes:
			this->dataGridView->ColumnCount += numAttributesToAdd;

			//Record class values:
			for (int i = 0; i < this->dataGridView->RowCount; i++)
			{
				classVals.push_back(msclr::interop::marshal_as<std::string>(this->dataGridView->Rows[i]->Cells["class"]->Value->ToString()));
			}

			//Re-label the columns.
			for (int i = 1; i < this->dataGridView->ColumnCount - 1; i++)
			{
				this->dataGridView->Columns[i]->Name = ("X" + i);
			}

			this->dataGridView->Columns[this->dataGridView->ColumnCount - 1]->Name = ("class");

			//Shift data over:

			//Make sure there is attributes being added:
			if (firstValue.length() > 2)
			{
				//Go over all rows:
				for (int i = 0; i < this->dataGridView->RowCount; i++)
				{

					//Shift all values after current attribute:
					for (int j = this->dataGridView->ColumnCount - 2 - numAttributesToAdd; j > attributeVal; j--)
					{

						//Grab current val:
						String^ curVal = this->dataGridView->Rows[i]->Cells["X" + j]->Value->ToString();

						//Move it over:
						this->dataGridView->Rows[i]->Cells["X" + (j + numAttributesToAdd)]->Value = curVal;

						//Clear current spot:
						this->dataGridView->Rows[i]->Cells["X" + j]->Value = "";

					}
				}
			}

			//Replace class vals:
			for (int i = 0; i < this->dataGridView->RowCount; i++)
			{
				this->dataGridView->Rows[i]->Cells["class"]->Value = gcnew String(classVals.at(i).c_str());
			}

			//Update the new cells with scheme values:
			for (int i = 0; i < this->dataGridView->RowCount; i++)
			{
				//Get curent key in curent row.
				DataGridViewRow^ curRow = this->dataGridView->Rows[i];
				String^ toMarshal = curRow->Cells["X" + attributeVal]->Value->ToString();
				std::string curVal = msclr::interop::marshal_as<std::string>(toMarshal);

				//Get the scheme we decided to replace with.
				std::string valToReplace = schemeMap->at(curVal);

				//Remove last letter.
				valToReplace.pop_back();

				int valToReplaceStaticLength = valToReplace.length();
				//Apply the new scheme value.
				for (int j = 0; j < valToReplaceStaticLength; j++)
				{
					string s = valToReplace.substr(0, 1);
					this->dataGridView->Rows[i]->Cells["X" + (attributeVal + j)]->Value = gcnew String(s.c_str());
					valToReplace = valToReplace.substr(1, valToReplace.length() - 1);//Remove val used.
				}
			}

			//Update attribute offset:
			numDimensions += numAttributesToAdd;
			attributeAddOffset += numAttributesToAdd;

		}
		else
		{
			//Itterate over cells in the attribute:
			for (int i = 0; i < numRows + 1; i++)
			{
				//Get curent key in curent row.
				DataGridViewRow^ curRow = this->dataGridView->Rows[i];
				String^ toMarshal = curRow->Cells["X" + attributeVal]->Value->ToString();
				std::string curVal = msclr::interop::marshal_as<std::string>(toMarshal);

				//Get the scheme we decided to replace with.
				std::string valToReplace = schemeMap->at(curVal);

				//Apply the new scheme value.
				curRow->Cells["X" + attributeVal]->Value = gcnew String(valToReplace.c_str());
			}

		}

	}
	catch (...)
	{
		return; //Prevents empty error.
	}

}

// ===saveToolStripMenuItem_Click===:
// Desc: Handles saving existing data to a file.
System::Void CppCLRWinformsProjekt::SchemeEditor::saveToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{
	//Open File:
	SaveFileDialog^ sfd = gcnew SaveFileDialog;
	sfd->InitialDirectory = ""; // NOTE "c:\\" for future reference.
	sfd->Filter = "Text Files (*.csv, *.txt, *text)|*.csv, *txt, *text|All Files (*.*)|*.*";
	sfd->FilterIndex = 1;
	sfd->RestoreDirectory = true;

	String^ path;

	if (sfd->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		path = sfd->FileName;
	}

	//Write Data:
	std::ofstream outFile;
	std::string outDir = msclr::interop::marshal_as<std::string>(path);
	std::string temp = "";

	outFile.open(outDir);

	//Add top lables:
	//outFile << msclr::interop::marshal_as<std::string>("id");
	outFile << "id,";
	for (int i = 1; i < dataGridView->ColumnCount - 1; i++)
	{
		outFile << msclr::interop::marshal_as<std::string>("X" + i + ",");
	}
	outFile << "class\n";

	//Itterate over rows:
	for (int i = 0; i <= numRows; i++)
	{
		DataGridViewRow^ curRow = this->dataGridView->Rows[i];

		//Itterate over cells in rows:
		String^ toMarshal = curRow->Cells["ID"]->Value->ToString();
		std::string curVal = msclr::interop::marshal_as<std::string>(toMarshal);
		temp += curVal + ",";

		int j = 1;
		for (; j < dataGridView->ColumnCount - 1; j++)
		{
			String^ toMarshal = curRow->Cells["X" + j]->Value->ToString();
			std::string curVal = msclr::interop::marshal_as<std::string>(toMarshal);
			temp += curVal + ",";
		}

		//Account for the class cell:
		toMarshal = curRow->Cells["class"]->Value->ToString();
		curVal = msclr::interop::marshal_as<std::string>(toMarshal);
		temp += curVal + "\n";

	}

	//Write to the file:
	outFile << temp;
	outFile.close();
}

// ===saveToolStripMenuItem_Click===:
// Desc: Handles saving existing data to a file.
System::Void CppCLRWinformsProjekt::SchemeEditor::editClassValuesToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e)
{

	//Launch the Scheme window:
	ClassScheme^ loadClassSchemeWindow = gcnew ClassScheme(this->path, this->dataGridView, numRows, numDimensions);
	loadClassSchemeWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	loadClassSchemeWindow->ShowDialog();

	this->classMap = loadClassSchemeWindow->getSchemeMap();

	//Replace class values:
	for (int i = 0; i <= numRows; i++)
	{
		String^ toMarshal = this->dataGridView->Rows[i]->Cells["class"]->Value->ToString();
		string curClassVal = msclr::interop::marshal_as<std::string>(toMarshal);
		string toReplace = classMap->at(curClassVal);
		
		this->dataGridView->Rows[i]->Cells["class"]->Value = gcnew String(toReplace.c_str());
	}

}

// ===ascendingOrderButton_Click===:
// Desc: Handles sorting in ascending order.
System::Void CppCLRWinformsProjekt::SchemeEditor::ascendingOrderButton_Click(System::Object^ sender, System::EventArgs^ e)
{
	//Get input from user:
	String^ inputM = this->columnInputFeild->Text;

	//Sort:
	sortAscending(inputM);
}

// ===descendingOrderButton_Click===:
// Desc: Handles sorting in descending order.
System::Void CppCLRWinformsProjekt::SchemeEditor::descendingOrderButton_Click(System::Object^ sender, System::EventArgs^ e)
{
	//Get input from user:
	String^ inputM = this->columnInputFeild->Text;

	//Sort:
	sortDesending(inputM);
}

// ===sortAscending===:
// Desc: sort.
System::Void CppCLRWinformsProjekt::SchemeEditor::sortAscending(System::String^ column)
{
	KVPair **arrKV = new KVPair*[numRows + 1];
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
System::Void CppCLRWinformsProjekt::SchemeEditor::sortDesending(System::String^ column)
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