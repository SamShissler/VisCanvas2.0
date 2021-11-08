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

	//Testing:
	/*
	groups.clear();

	int testGroups[30][3] = { {9, 15, 19}, {19, 15, 5}, {21, 15, 20}, {19, 20, 5}, {22, 19, 9}, {19, 20, 15}, {20, 9, 21}, {5, 20, 22}, {21, 15, 9}, {15, 19, 22},
							  {9, 22, 21}, {20, 5, 22}, {15, 22, 20}, {21, 22, 5}, {9, 20, 21}, {22, 9, 19}, {19, 22, 15}, {5, 15, 20}, {15, 19, 22}, {21, 15, 20},
							  {5, 19, 9}, {5, 22, 19}, {9, 19, 21}, {21, 20, 19}, {5, 15, 9}, {19, 22, 9}, {22, 9, 21}, {22, 19, 15}, {15, 21, 5}, {21, 19, 20} };

	for (int i = 0; i < 30; i++)
	{
		vector<int> toAdd;
		for (int j = 0; j < 3; j++)
		{
			toAdd.push_back(testGroups[i][j]);
		}
		groups.push_back(toAdd);
	}
	*/
	//End Testing.


	//====================================================//
	/*
	rules.push_back("\n\n=============75%===============\n\n");
	//vector <string> Per75 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(75.0, groups, classToTest);
	//vector < string> Per75 = this->domNomVisualization->MTBRuleGenResults(75.0, groups, classToTest);
	vector < string> Per75 = this->domNomVisualization->MTBRGSequential(0.0, groups, classToTest);
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
	
	*/
	rules.push_back("\n\n=============95%===============\n\n");
	//vector < string> Per95 = this->domNomVisualization->ParetoFrontRuleGenWithOverlap(95.0, groups, classToTest);
	//vector < string> Per95 = this->domNomVisualization->MTBRuleGenResults(95.0, groups, classToTest);
	pair<vector<string>, vector<DNSRule>> Per95 = this->domNomVisualization->MTBRGSequential(95.0, groups, classToTest);
	for (int i = 0; i < Per95.first.size(); i++)
	{
		rules.push_back(Per95.first.at(i));
	}
	
	//vector<string> t = this->domNomVisualization->tenFoldCrossValidation(classToTest, groups);

	/*
	for (int i = 0; i < t.size(); i++)
	{
		rules.push_back(t.at(i));
	}
	*/

	/*
	double correctlyPredictedComp = 0;
	double corR1 = 0;
	double corR2 = 0;
	double corR3 = 0;
	vector<int> casesR1;
	vector<int> casesR2;
	vector<int> casesR3;
	double incorrectlyPredictedComp1 = 0;
	double incorrectlyPredictedComp2 = 0;
	double incorrectlyPredictedComp3 = 0;
	for (int caseIndex = 0; caseIndex < file->getSetAmount(); caseIndex++)
	{
		int curCase = caseIndex;
		int curCaseClass = file->getClassOfSet(curCase);
		double valAt5 = file->getOriginalData(curCase, 4);
		double valAt8 = file->getOriginalData(curCase, 7);
		double valAt9 = file->getOriginalData(curCase, 8);
		double valAt12 = file->getOriginalData(curCase, 11);
		double valAt20 = file->getOriginalData(curCase, 19);
		double valAt21 = file->getOriginalData(curCase, 20);

		if (valAt5 == 1 && valAt9 != 1)
		{
			casesR1.push_back(caseIndex);
			if (curCaseClass == 2)
			{
				correctlyPredictedComp++;
				corR1++;
			}
			else
			{
				incorrectlyPredictedComp1++;
			}
		}

		if (valAt5 == 2 && valAt9 != 1)
		{
			casesR2.push_back(caseIndex);
			if (curCaseClass == 2)
			{
				correctlyPredictedComp++;
				corR2++;
			}
			else
			{
				incorrectlyPredictedComp2++;
			}
		}
	}

	//Determine overlap.
	vector<int> overlapWithR1;

	int numCasesR1R2 = 0;
	int numCasesR1R3 = 0;
	int numCasesR2R3 = 0;

	for (int i = 0; i < casesR1.size(); i++)
	{
		bool contained = false;
		for (int j = 0; j < casesR2.size(); j++)
		{
			if (casesR1.at(i) == casesR2.at(j))
			{
				contained = true;
			}
		}

		if (contained)
		{
			numCasesR1R2++;
		}
	}

	for (int i = 0; i < casesR1.size(); i++)
	{
		bool contained = false;
		for (int j = 0; j < casesR3.size(); j++)
		{
			if (casesR1.at(i) == casesR3.at(j))
			{
				contained = true;
			}
		}

		if (contained)
		{
			numCasesR1R3++;
		}
	}

	for (int i = 0; i < casesR2.size(); i++)
	{
		bool contained = false;
		for (int j = 0; j < casesR3.size(); j++)
		{
			if (casesR2.at(i) == casesR3.at(j))
			{
				contained = true;
			}
		}

		if (contained)
		{
			numCasesR2R3++;
		}
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