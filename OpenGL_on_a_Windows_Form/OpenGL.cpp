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

	/*
	//Locat data:
	int targetClass = 1;
	vector<string> toReturn;
	double runningAverageOur = 0;
	double runningAverageComp = 0;

	for (int n = 0; n < 1; n++)
	{
		vector<vector<int>> partitions;
		vector<int> caseIDs;
		int partitionNumber = 10;

		//Fill partition vector:
		for (int i = 0; i < partitionNumber; i++)
		{
			vector<int> toAdd;
			partitions.push_back(toAdd);
		}

		//Fill the case IDs with the set IDs so it can be manipulated.
		for (int i = 0; i < file->getSetAmount(); i++)
		{
			caseIDs.push_back(i);
		}

		//While there are cases to choose from.
		while (caseIDs.size() != 0)
		{
			//Randomly select IDs for partitions.
			for (int i = 0; i < partitionNumber; i++) //Iterate over groups.
			{

				//Determine a random index in remaining cases.
				//std::srand(std::time(nullptr));
				int index = rand() % caseIDs.size();

				//Add the index to the current data
				partitions.at(i).push_back(index);

				//Remove the case ID at the index.
				int count = 0;
				for (auto it = caseIDs.begin(); it != caseIDs.end(); it++)
				{
					if (count == index)
					{
						caseIDs.erase(it);
						break;
					}
					count++;
				}

				//If there are no more cases, return.
				if (caseIDs.size() == 0) break;
			}
		}

		//Use the partitions seqentially as test data:
		double totalOurAccuracy = 0;
		double totalCompAccuracy = 0;
		for (int i = 0; i < partitions.size(); i++)
		{
			vector<int> testPartition = partitions.at(i);

			//Determine the number of target class cases in this partition so we can see how many are missed.
			int numTargetClassCases = 0;
			for (int caseIndex = 0; caseIndex < testPartition.size(); caseIndex++)
			{
				int curCase = testPartition.at(caseIndex);
				if (file->getClassOfSet(curCase) == targetClass)
				{
					numTargetClassCases++;
				}
			}

			//Iterate over test partition and perdict for competitiors rules.
			double correctlyPredictedComp = 0;
			int cor1 = 0;
			int cor2 = 0;
			int cor3 = 0;
			int incor1 = 0;
			int incor2 = 0;
			int incor3 = 0;


			for (int caseIndex = 0; caseIndex < testPartition.size(); caseIndex++)
			{
				int curCase = testPartition.at(caseIndex);
				int curCaseClass = file->getClassOfSet(curCase);
				double valAt5 = file->getOriginalData(curCase, 4);
				double valAt8 = file->getOriginalData(curCase, 7);
				double valAt12 = file->getOriginalData(curCase, 11);
				double valAt20 = file->getOriginalData(curCase, 19);
				double valAt21 = file->getOriginalData(curCase, 20);

				if (valAt5 != 1 && valAt5 != 2 && valAt5 != 7)
				{
					if (curCaseClass == targetClass)
					{
						correctlyPredictedComp++;
						cor1++;
					}
					else
					{
						incor1++;
					}
				}
				
				else if (valAt20 == 5)
				{
					if (curCaseClass == targetClass)
					{
						correctlyPredictedComp++;
						cor2++;
					}
					else
					{
						incor2++;
					}
				}


				else if (valAt8 == 2 && ((valAt12 == 3 || valAt12 == 2) || valAt21 == 2))
				{
					if (curCaseClass == targetClass)
					{
						correctlyPredictedComp++;
						cor3++;
					}
					else
					{
						incor3++;
					}
				}
			}

			//Accuracy calculation:
			double compAccuracy = (correctlyPredictedComp / numTargetClassCases) * 100.0;

			//Add to total to get average.
			totalCompAccuracy += compAccuracy;

			//Record test results.
			string toAdd = "Test " + to_string(i + 1) + ":\n";
			toAdd += "Accuracy for the competitors rules = " + to_string(compAccuracy) + "%" + " Predicted: " + to_string(correctlyPredictedComp) + " incor:"
				+ to_string(numTargetClassCases - correctlyPredictedComp) + " total: " + to_string(numTargetClassCases)+ "\n\n";
			toReturn.push_back(toAdd);
		}

		double finalAverageCompRules = totalCompAccuracy / partitionNumber;

		runningAverageComp += finalAverageCompRules;

		string averages = "% Comp avrg = " + to_string(finalAverageCompRules) + "%\n\n\n";
		toReturn.push_back(averages);
	}

	double overallAverComp = runningAverageComp / 10.0;

	string fin =  "Comp: " + to_string(overallAverComp) + "\n\n";
	toReturn.push_back(fin);

	for (int i = 0; i < toReturn.size(); i++)
	{
		rules.push_back(toReturn.at(i));
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