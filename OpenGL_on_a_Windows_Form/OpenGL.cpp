#include "stdafx.h"
#include "LoadingScreen.h"
#include "OpenGL.h"
#include "CoordinateGroupingMainWindow.h"
#include <time.h>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include "loadingForm.h"
#include <chrono>
#include <thread>


using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
//Set up for code cleanup.

// visualizeDomNomVisualization:
// Desc: Tells the dominant nominal visualization to draw.
void OpenGLForm::COpenGL::visualizeDomNomVisualization()
{
	domNomVisualization->drawVisualization();
}

//used as a thread to start up loading form
void loadLoadingFormFunc(bool* loading)
{
	VisCanvas::loadingForm^ form = gcnew VisCanvas::loadingForm();
	form->Show();
	form->Update();
	int n = 0;
	while (*loading)
	{
		string formText;
		if (n == 0)
			form->updateLabel("loading");
		else if (n == 1)
			form->updateLabel("loading.");
		else if (n == 2)
			form->updateLabel("loading..");
		else if (n == 3)
		{
			form->updateLabel("loading...");
			n = -1;//this way n = 0 upon n++ below
		}
		n++;

		form->Update();
		sleep_until(system_clock::now() + milliseconds(500));//half a second between every update
	}
	form->Close();
}

// generateRulesDNS:
// Desc: Tells the Dominant nomianl visualization to generate rules.
void OpenGLForm::COpenGL::generateRulesDNS()
{
	vector<int>* avalibleDimensions = new vector<int>();

	for (int i = 0; i < file->getDimensionAmount(); i++)
	{
		avalibleDimensions->push_back(i + 1);
	}

	CppCLRWinformsProjekt::CoordinateGroupingMainWindow^ coordinateGrouping = gcnew CppCLRWinformsProjekt::CoordinateGroupingMainWindow(avalibleDimensions);
	coordinateGrouping->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
	coordinateGrouping->ShowDialog();

	vector<string> rules;
	vector<vector<int>> groups = *coordinateGrouping->getCoordinateGroups();
	const int classToTest = 1;
	
	
	//vector<string> g = this->domNomVisualization->geneticAttributeGroupGeneration();
	//file->setDNSRulesGenerated(g);

	//return;

	
	groups.clear();
	int toAddGroups[2][3] = { {1, 2, 3}, {4, 5, 6} };

	for (int i = 0; i < 2; i++)
	{
		vector<int>toAdd;
		for (int j = 0; j < 3; j++)
		{
			toAdd.push_back(toAddGroups[i][j]);
		}

		groups.push_back(toAdd);
	}

	//opens the config file
	string fileName = "config.txt";
	ifstream precThreshInp;
	precThreshInp.open(fileName);
	vector<float> precThreshs;

	//checks to see if config file was properly opened / found
	std::size_t found;
	if (!precThreshInp.is_open())
	{
		exit(EXIT_FAILURE);
	}

	//finds the line in the config file which has "precisionThresholds" and fills precThreshs with all values
	for(string strPrec; getline(precThreshInp, strPrec);)
	{
		found = strPrec.find("PrecisionThresholds");
		if (found != string::npos)
		{
			istringstream iss(strPrec);
			string val;
			iss >> val;
			while(iss)
			{
				iss >> val;
				if (val.find(";") != string::npos)
				{
					break;
				}
				precThreshs.push_back(stod(val));
				//precThreshInp >> strPrec;
			} 
			break;
		}
	}
	//loading variable used to terminate loadaing thread
	bool loading = true;
	std::thread thread_obj(loadLoadingFormFunc, &loading);

	//calls MTBRG for every defined precision threshold from precThreshs
	for (int i = 0; i < precThreshs.size(); i++)
	{
		string beg = "\n\n=============";
		string mid = to_string(precThreshs[i]);
		string end = "===============\n\n";
		rules.push_back(beg + mid + end);		
		pair<vector<string>, vector<DNSRule>> Per = this->domNomVisualization->MTBRGSequential(precThreshs[i], groups, classToTest);
		for (int i = 0; i < Per.first.size(); i++)
		{
			rules.push_back(Per.first.at(i));
		}
	}

	//the loading is done, now join threads
	loading = false;
	//join thread after termination; loading form closed
	thread_obj.join();
	//reset loading for the next rule generation
	loading = true;
	/*//====================================================//
	rules.push_back("\n\n=============25%===============\n\n");
	//vector <string> Per75 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(75.0, groups, classToTest);
	//vector < string> Per75 = this->domNomVisualization->MTBRuleGenResults(75.0, groups, classToTest);
	pair<vector<string>, vector<DNSRule>> Per25 = this->domNomVisualization->MTBRGSequential(25.0, groups, classToTest);
	for (int i = 0; i < Per25.first.size(); i++)
	{
		rules.push_back(Per25.first.at(i));
	}

	rules.push_back("\n\n=============50%===============\n\n");
	//vector <string> Per75 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(75.0, groups, classToTest);
	//vector < string> Per75 = this->domNomVisualization->MTBRuleGenResults(75.0, groups, classToTest);
	pair<vector<string>, vector<DNSRule>> Per50 = this->domNomVisualization->MTBRGSequential(50.0, groups, classToTest);
	for (int i = 0; i < Per50.first.size(); i++)
	{
		rules.push_back(Per50.first.at(i));
	}
	/*
	rules.push_back("\n\n=============75%===============\n\n");
	//vector <string> Per75 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(75.0, groups, classToTest);
	//vector < string> Per75 = this->domNomVisualization->MTBRuleGenResults(75.0, groups, classToTest);
	pair<vector<string>, vector<DNSRule>> Per75 = this->domNomVisualization->MTBRGSequential(75.0, groups, classToTest);
	for (int i = 0; i < Per75.first.size(); i++)
	{
		rules.push_back(Per75.first.at(i));
	}
	*/
	/*
	rules.push_back("\n\n=============85%===============\n\n");
	//vector < string> Per85 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(85.0, groups, classToTest);
	//vector < string> Per85 = this->domNomVisualization->MTBRuleGenResults(85.0, groups, classToTest);
	pair<vector<string>, vector<DNSRule>> Per85 = this->domNomVisualization->MTBRGSequential(85.0, groups, classToTest);
	for (int i = 0; i < Per85.first.size(); i++)
	{
		rules.push_back(Per85.first.at(i));
	}

	rules.push_back("\n\n=============95%===============\n\n");
	//vector < string> Per95 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(95.0, groups, classToTest);
	//vector < string> Per95 = this->domNomVisualization->MTBRuleGenResults(95.0, groups, classToTest);
	pair<vector<string>, vector<DNSRule>> Per95 = this->domNomVisualization->MTBRGSequential(95.0, groups, classToTest);
	for (int i = 0; i < Per95.first.size(); i++)
	{
		rules.push_back(Per95.first.at(i));
	}
	*/
	rules.push_back("\n\n\n\n");
	
	//vector<string> tenFold = this->domNomVisualization->tenFoldCrossValidation(classToTest, groups);

	
	file->setDNSRulesGenerated(rules);
	
}

// setDomNomSetVisualizatonL
// Desc: Creates a new dominant nominal visualization.
void OpenGLForm::COpenGL::setDomNomSetVisualization(DataInterface* file, double worldHeight, double worldWidth)
{
	this->domNomVisualization = new DomNominalSet(file, worldHeight, worldWidth);
}

// DNScalculateLinePosition
// Desc: Allows viscanvas to tell visualziation to recalate line positions. Used after shifts.
void OpenGLForm::COpenGL::DNScalculateLinePosition()
{
	this->domNomVisualization->calculateLinePositions(worldWidth);
}