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

	//===Normal Dom-Nom-Set Version===
	vector<unordered_map<double, double>*>* getBlockHeights(vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass);
	vector<vector<unordered_map<double, double>*>*>* getValuePerClassFreq();
	vector<vector<unordered_map<double, double>*>*>* getClassPercPerBlock(vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass);
	vector<vector<pair<double, double>>> getSortByPurity(vector<unordered_map<double, double>*>* blockHeights, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock);
	vector<vector<pair<double, double>>> getSortByFreqency(vector<unordered_map<double, double>*>* blockHeights);
	vector<vector<pair<double, double>>> getSortByClass(vector<unordered_map<double, double>*>* blockHeights, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock);

	//===Perc Color Version===
	//Used to calculate class color sub block positions by major block for Percent Color visualization.
	void calculateClassColorPositionByBlock(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	
	//Drawing:
	GLvoid drawVisualization();
	GLvoid drawRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawGrayRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawLines(double worldWidth);

	GLvoid drawColorPercentVisualization();
	GLvoid drawColorPercentRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawColorPercentLines(double worldWidth);

	//Analysis:
	vector<string> determineRules();
	

private:

	//Data for drawing:
	double worldHeight;
	double worldWidth;
	DataInterface* file;
	vector<vector<pair<double, double>>> middleOther; 
	vector<vector<pair<double, double>>> domClass;
	vector<vector<pair<double, double>>> sortedVector;

	//Calculated data:
	vector<unordered_map<double, unordered_map<double, double>>> classColorPositionByBlock;
	vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass;
	vector<vector<unordered_map<double, double>*>*>* classPercPerBlock;
	vector<unordered_map<double, double>*>* blockHeights;
	vector<vector<pair<double, double>>> sortedByPurityVector;
	vector<vector<pair<double, double>>> sortedByFreqVector;
	vector<vector<pair<double, double>>> sortedByClassVector;
};

#endif // !