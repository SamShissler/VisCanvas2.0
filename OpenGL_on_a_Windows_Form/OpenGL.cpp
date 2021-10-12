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
	
	rules.push_back("\n\n=============75%===============\n\n");
	//vector <string> Per75 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(75.0, groups, classToTest);
	//vector < string> Per75 = this->domNomVisualization->MTBRuleGenResults(75.0, groups, classToTest);
	vector < string> Per75 = this->domNomVisualization->MTBRGSequential(75.0, groups, classToTest);
	for (int i = 0; i < Per75.size(); i++)
	{
		rules.push_back(Per75.at(i));
	}
	

	
	rules.push_back("\n\n=============85%===============\n\n");
	//vector < string> Per85 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(85.0, groups, classToTest);
	//vector < string> Per85 = this->domNomVisualization->MTBRuleGenResults(85.0, groups, classToTest);
	vector < string> Per85 = this->domNomVisualization->MTBRGSequential(85.0, groups, classToTest);
	for (int i = 0; i < Per85.size(); i++)
	{
		rules.push_back(Per85.at(i));
	}
	
	
	
	rules.push_back("\n\n=============95%===============\n\n");
	//vector < string> Per95 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(95.0, groups, classToTest);
	//vector < string> Per95 = this->domNomVisualization->MTBRuleGenResults(95.0, groups, classToTest);
	vector < string> Per95 = this->domNomVisualization->MTBRGSequential(95.0, groups, classToTest);
	for (int i = 0; i < Per95.size(); i++)
	{
		rules.push_back(Per95.at(i));
	}
	
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