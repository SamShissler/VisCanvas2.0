#ifndef DOMNOMINALSET_H
#define DOMNOMINALSET_H

#include "DataInterface.h"
#include <unordered_map>
#include <vector>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <utility>

class DomNominalSet {
public:

	//Constructors:
	DomNominalSet();
	DomNominalSet(DataInterface* file, int worldHeight);
	
	//Data Compiling:
	vector<unordered_map<double, double>*>* getBlockHeights(vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass);
	vector<vector<unordered_map<double, double>*>*>* getValuePerClassFreq();
	vector<vector<unordered_map<double, double>*>*>* getClassPercPerBlock(vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass);
	vector<vector<pair<double, double>>> getSortByPurity(vector<unordered_map<double, double>*>* blockHeights, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock);
	vector<vector<pair<double, double>>> getSortByFreqency(vector<unordered_map<double, double>*>* blockHeights);
	vector<vector<pair<double, double>>> getSortByClass(vector<unordered_map<double, double>*>* blockHeights, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock);
	
	//Drawing:
	GLvoid drawVisualization(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawGrayRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawLines(double worldWidth);

	GLvoid drawColorPercentVisualization(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);
	GLvoid drawColorPercentRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth);

	//Analysis:
	vector<string> determineRules();
	

private:
	int worldHeight;
	DataInterface* file;
	vector<vector<pair<double, double>>> middleOther; 
	vector<vector<pair<double, double>>> domClass;
	vector<vector<pair<double, double>>> sortedVector;
};

#endif // !