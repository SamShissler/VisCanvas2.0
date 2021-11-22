#include "stdafx.h"
#include "LoadingScreen.h"
#include "OpenGL.h"
#include "CoordinateGroupingMainWindow.h"
#include <time.h>

//Set up for code cleanup.

// visualizeDomNomVisualization:
// Desc: Tells the dominant nominal visualization to draw.
void OpenGLForm::COpenGL::visualizeDomNomVisualization()
{
	domNomVisualization->drawVisualization();
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

	//====================================================//
	rules.push_back("\n\n=============75%===============\n\n");
	//vector <string> Per75 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(75.0, groups, classToTest);
	//vector < string> Per75 = this->domNomVisualization->MTBRuleGenResults(75.0, groups, classToTest);
	pair<vector<string>, vector<DNSRule>> Per75 = this->domNomVisualization->MTBRGSequential(75.0, groups, classToTest);
	for (int i = 0; i < Per75.first.size(); i++)
	{
		rules.push_back(Per75.first.at(i));
	}
	
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

	//Determine all cases in CR1.

	/*
	vector<int> setsToDelete;
	for (int i = 0; i < file->getSetAmount(); i++)
	{
		double originalVal5 = file->getOriginalData(i, 4);

		if (originalVal5 == 1 || originalVal5 == 2 || originalVal5 == 7)
		{
			setsToDelete.push_back(i);
		}
	}
	
	

	//Remove all cases.
	for (int i = setsToDelete.size() - 1; i >= 0; i--)
	{
		file->deleteSet(setsToDelete.at(i));
	}
	*/

	/*
	//Run 10-fold cross validation.
	vector<string> t = this->domNomVisualization->tenFoldCrossValidation(classToTest, groups);

	//Record all generated data.
	for (int i = 0; i < t.size(); i++)
	{
		rules.push_back(t.at(i));
	}
	*/

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