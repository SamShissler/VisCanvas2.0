/// 
/// Dominant Nominal Sets Visualization
/// Date: 8/7/21
/// Desc: Header file for Dominant Nominal Set Class. This class handles drawing
///  and caluclating data for dominant nominal sets.
/// 

#ifndef DOMNOMINALSET_H
#define DOMNOMINALSET_H

#include "DataInterface.h"
#include <unordered_map>
#include <vector>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <utility>

class DomNominalSet 
{

public:

	//Constructors:
	DomNominalSet();
	DomNominalSet(DataInterface* file, double worldHeight, double worldWidth);
	
	//Data Compiling:

	//===Dom-Nom-Set Version===
	void reCalculateData();
	vector<unordered_map<double, double>*>* getBlockHeights(vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass);
	vector<vector<unordered_map<double, double>*>*>* getValuePerClassFreq();
	vector<vector<unordered_map<double, double>*>*>* getClassPercPerBlock(vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass);
	vector<vector<pair<double, double>>> getSortByPurity(vector<unordered_map<double, double>*>* blockHeights, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock);
	vector<vector<pair<double, double>>> getSortByFreqency(vector<unordered_map<double, double>*>* blockHeights);
	vector<vector<pair<double, double>>> getSortByClass(vector<unordered_map<double, double>*>* blockHeights, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock);
	
	//Drawing Starting Visualizaation:
	GLvoid drawVisualization();
	GLvoid drawRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawLines(double worldWidth);
	GLvoid drawSelectorBoxes(double worldWidth);
	GLvoid drawHoverInfo(double worldWidth);

	//Color Precent Visualization!
	GLvoid drawColorPercentVisualization();
	GLvoid drawColorPercentRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawColorPercentLines(double worldWidth);

	//Gray Block Only Visualization!
	GLvoid drawGrayRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawGrayLines(double worldWidth);

	//Analysis:
	string linguisticDesc();
	vector<string> determineRules();
	vector<string> ruleGenerationSequential();
	vector<string> MTBRuleGeneration();
	GLvoid visualizeRules();

	//Getters and Setters:
	vector<pair<double, pair<double, double>>> getRuleData();

private:

	//Data for drawing:
	double worldHeight;
	double worldWidth;
	bool grayBlockVisActive = false;
	DataInterface* file;
	vector<vector<pair<double, double>>> middleOther; //Positions of the middle of the gray blocks (Other classes).
	vector<vector<pair<double, double>>> domClass; //Dominant Classes for each block.
	vector<vector<pair<double, double>>> sortedVector; //Vector sorted by one of the sorting alogirthms.

	//Calculated data:
	vector<unordered_map<double, unordered_map<double, double>>> classColorPositionByBlock; //Positions of each class color sub block in each coordinate.
	vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass; //Frequency of each value in each cooridnate.
	vector<vector<unordered_map<double, double>*>*>* classPercPerBlock; //Percent each class takes up for each block.
	vector<unordered_map<double, double>*>* blockHeights; //Block Heights for each coordinate.
	vector<vector<pair<double, double>>> sortedByPurityVector; //Blocks sorted by the dominant (purest) class).
	vector<vector<pair<double, double>>> sortedByFreqVector; //Blocks sorted by the highest frequency.
	vector<vector<pair<double, double>>> sortedByClassVector; //Blocks sorted by class.
	vector<pair<double, pair<double, double>>> ruleData; //Rule data pair <starting coordinate, pair(value1,value2)>
	vector<vector<int>> casesPerCoordinate; // Records the number of cases to be used when drawing the hover info.
};

#endif 