#ifndef DataInterface_H
#define DataInterface_H
#include "DataNode.h"
#include "Dimension.h"
#include "ColorCustom.h"
#include "DataClass.h"
#include "DataSet.h"
#include "GraphNote.h"
#include "SetCluster.h"

#include <vector>
#include <list>
#include <set>
#include <string> 
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
/*
Author: Daniel Ortyn
Last Update: 2018/22/01
Purpose: CS 481 Project
*/


// a class to hold several sets of data and manage interactions with the data
class DataInterface {
public:
	// create a blank class without any data
	DataInterface();
	// delete the object
	~DataInterface();

	// read data from the file at the passed path(filePath) and 
	bool readFile(std::string* filePath);
	// saves the data and all settings to the file at the passed location
	bool saveToFile(std::string * filePath);

	// sets whether set names will be read from basic files 
	bool setReadSetNames(bool newReadSetNames);
	// sets whether dimension names will be read from basic files 
	bool setReadDimensionNames(bool newReadDimensionNames);
	// sets whether class names will be read from basic files 
	bool setReadClassNames(bool newReadClassNames);

	// returns whether set names will be read from basic files 
	bool isReadSetNames();
	// returns whether dimension names will be read from basic files 
	bool isReadDimensionNames();
	// returns whether class names will be read from basic files 
	bool isReadClassNames();


	void confusion();


	// get the amounts of class for the data
	int getClassAmount() const;
	// gets the total amount of data sets
	int getSetAmount() const;
	// gets the number of dimensions in the data
	int getDimensionAmount() const;
	vector<Dimension*>* getDataDimensions();
	// inverts the dimension
	void invertDimension(int dimensionIndex);
	// checks whether the dimension is inverted
	bool isDimensionInverted(int dimensionIndex);
	// gets the data in the set of the passed index(setIndex), for the passed dimension(indexOfData)
	double getData(int setIndex, int indexOfData) const;
	// gets the original data in the set of the passed index(setIndex), for the passed dimension(indexOfData)
	double getOriginalData(int setIndex, int indexOfData) const;
	// sets the data in the set of the passed index(setIndex), for the passed dimension(indexOfData), to the passed value(newDataValue)
	double setData(int setIndex, int indexOfData, double newDataValue);
	// moves the dimension at the passed index(indexOfDimension) to the spot after the other index(indexBeforeInsertion)
	bool moveData(int indexOfDimension, int indexBeforeInsertion);



	// gets the name of the class at the passed index(classIndex)
	std::string* getDimensionName(int dimensionIndex);
	// sets the name of the class at the passed index(classIndex) to the passed string(newName)
	void setDimensionName(int dimensionIndex, std::string* newName);

	// gets whether the calibration for the dimension(dimensionIndex) is artificial or relative
	bool isArtificiallyCalibrated(int dimensionIndex);
	// sets the calibration to use the data's(not the artificial) maximum and minimum in dimension at the passed index(dimensionIndex)
	void clearArtificialCalibration(int dimensionIndex);
	// sets the bounds to be used for artificial calibration at the passed index(dimensionIndex)
	void setCalibrationBounds(int dimensionIndex, double newMaximum, double newMinimum);
	// gets the artificial maximum for the dimension at the passed index(dimensionIndex)
	double getArtificialMaximum(int dimensionIndex) const;
	// gets the artificial minimum for the dimension at the passed index(dimensionIndex)
	double getArtificialMinimum(int dimensionIndex) const;

	// add the passed double(amountToAdd) to all the data in the dimension at the index(dimensionIndex)
	void addToDimension(int dimensionIndex, double amountToAdd);




	// gets the name of the class at the passed index(classIndex)
	std::string* getClassName(int classIndex);
	// sets the name of the class at the passed index(classIndex) to the passed string(newName)
	void setClassName(int classIndex, std::string* newName);
	// adds a new class to the list of classes
	void addClass();
	// delete a class from the list of classes
	void deleteClass(int classIndex);
	/*
	Gets the number of sets in the class at the passed index(classIndex)
	*/
	int getSetAmount(int classIndex) const;
	// gets a list of sets in the class
	std::vector<int>* getSetsInClass(int classIndex);
	// gets the color for the class at the passed class index
	std::vector<double>* getClassColor(int classIndex);
	// sets the color for the class at the passed class index(classIndex) to the passed color(newColor)
	void setClassColor(int classIndex, std::vector<double>* newColor);




	// gets the name of the set in the class at the passed index(classIndex) at the passed class relative index(setIndex)
	std::string* getSetName(int setIndex);
	// sets the name of the set at the passed index(setIndex)
	void setSetName(int setIndex, std::string &newName);
	// get the data class index for the set at the passed index
	int getClassOfSet(int setIndex) const;
	// get the data set name for the class at the passed index
	std::string* getSetOfClass(int classIndex, int setIndex);
	// sets the index data class of the set at the passed index(setIndex)
	int setSetClass(int setIndex, int newClassIndex);
	// gets the color the set should be painted
	std::vector<double>* getSetColor(int setIndex);
	// checks whether the set is visible or not
	bool isVisible(int setIndex);
	// sets the set visible or not visible
	bool setVisible(int setIndex, bool newVisible);



	// gets the amount the dimension is shifted by
	double getDimensionShift(int dimensionIndex);
	// changes the shift of the dimension at the passed int to the passsed double
	void setDimensionShift(int dimensionIndex, double shiftMod);

	// gets the name of the x-axis
	std::string* getXAxisName();
	// gets the name of the y-axis
	std::string* getYAxisName();
	// gets the name of maximum y value
	std::string* getYMaxName();
	// gets the name of minimum y value
	std::string* getYMinName();




	// sorts the data by the set at the passed index, in ascending order(left to right)
	void sortAscending(int setIndex);
	// sorts the data by the set at the passed index, in descending order(left to right)
	void sortDescending(int setIndex);
	// places the dimensions back in the original order
	void sortOriginal();



	/* adjusts the data of each dimension by shifting the data of each dimension by the
	amount necessary to put the dat of the set at the passed index(setIndex) to the passed value(levelValue)*/
	void level(int setIndex, double levelValue);
	// gets the mean data value of the passed set
	double getMean(int setIndex) const;
	// gets the median data value of the passed set
	double getMedian(int setIndex) const;
	// calibrates each dimension's data to the [0,1] space, dimensions calibrated individually
	void calibrateData();
	// zero shifts in the dimensions
	void zeroShifts();

	// gets the title of the note at the passed index(noteIndex)
	std::string* getNoteTitle(int noteIndex);
	// gets the message of the note at the passed index(noteIndex)
	std::string* getNoteMessage(int noteIndex);
	// gets the x coordinate of the note at the passed index(noteIndex)
	double getNoteX(int noteIndex);
	// gets the y coordinate of the note at the passed index(noteIndex)
	double getNoteY(int noteIndex);
	// gets the number of notes
	int getNoteAmount();
	// deletes of the note at the passed index(noteIndex)
	void deleteNote(int noteIndex);





	// compares the data of a each set to the set at the passed index and checks if the data is within the radius of the data of the passed set
	bool hypercube(int setIndex, double radius);
	// compares the data of a each set to the set at the passed index and checks if the data is within the radius of the data of the passed set
	bool subHypercube(int setIndex, int clusterIndex, double radius);
	// creates a hypercube containing all instances of a class
	void classHypercube(int classIndex);
	// returns whether the clusters will use mean or median
	bool isUseMeanForClusters();
	// sets whether the clusters will use mean or median
	bool getUseMeanForClusters();
	// sets whether the clusters will use mean or median
	void setUseMeanForClusters(bool newUseMean);
	double getRadius();
	void setRadius(double newRadius);



	// increases the selected clusters class index
	void incrementSelectedClusterClassIndex();
	// increases the selected set index
	void incrementSelectedSetIndex();
	// decreases the selected set index
	void decrementSelectedSetIndex();
	// gets the index of the selected set
	int getSelectedSetIndex();
	void setSelectedSetIndex(int index);
	// increases the index of the selected class to the passed index(newSelectedClassIndex)
	void incrementSelectedClassIndex();
	// decreases the index of the selected class to the passed index(newSelectedClassIndex)
	void decrementSelectedClassIndex();

	void incrementSelectedSetIndexCluster();
	void decrementSelectedSetIndexCluster();
	void incrementSelectedClusterIndex();
	void decrementSelectedClusterIndex();

	int getTotalCubeCount();
	int getOverlappingCubeCount();

	// gets the index of the selected class
	int getSelectedClassIndex() const;
	int getSelectedClusterIndex() const;
	void setSelectedClusterIndex(int);
	int getSelectedClusterClassIndex() const;

	// sets the selected set's color to the colors in the passed double array(newSelectedSetColor)
	void setSelectedSetColor(std::vector<double> &newSelectedSetColor);
	// gets the color of the selected set
	std::vector<double>* getSelectedSetColor();


	void xorClusters(int firstCluster, int secondCluster);

	// whether to paint the clusters or not
	bool isPaintClusters() const;
	// whether to paint the clusters using class colors or hypercube colors
	bool isPaintClassColors() const;
	// toggles whether to paint the clusters or not
	bool togglePaintClusters();
	// toggles whether to paint with class colors or hypercube colors
	bool togglePaintClassColors();
	// gets amount of clusters
	int getClusterAmount();
	// get set clusters
	std::vector<SetCluster> getClusters();
	// the minimum value for the cluster data
	double getClusterMinimum(int clusterIndex, int dimensionIndex) const;
	// the mean value for the cluster data
	double getClusterMiddle(int clusterIndex, int dimensionIndex) const;
	// the maximum value for the cluster data
	double getClusterMaximum(int clusterIndex, int dimensionIndex) const;
	// gets the color of the cluster
	std::vector<double>* getClusterColor(int clusterIndex);
	// sets the color of the cluster at the passed index
	void setClusterColor(int clusterIndex, std::vector<double>* newColor);
	// deletes the cluster at the passed index
	void deleteCluster(int classIndex);
	// gets the name of the cluster
	std::string* getClusterName(int clusterIndex);
	// sets whether the named cluster is displayed
	void setDisplayed(string name, bool display);
	// gets whether a cluster is displayed
	bool getDisplayed(int index);
	// sets the name of the cluster
	void setClusterName(int clusterIndex, std::string* newName);
	// gets a list of the sets in the class
	std::vector<int>* getClusterSets(int clusterIndex);



	// sets the color for the background
	void setBackgroundColor(std::vector<double>* newColor);
	// gets the color for the background
	std::vector<double>* getBackgroundColor();

	// auto color classes
	void autoColor();

	// auto create hypercube clusters around classes
	void autoCluster();
	void highlightOverlap(double threshold);
	void deleteEqualClusters();
	void combineAdjacentCubes();

	void setHypercubeThreshold(double newThreshold);
	double getHypercubeThreshold();

	bool selectorLineIsHidden();
	void hideSelector(bool isHidden);

	bool drawBorders();
	void setDrawBorders(bool drawBorders);

	void identicalCubes(set<int> * classesInCube, vector<int> * selectedInstances);
	void createAutoCube(set<int> * classesInCube, vector<int> * selectedInstances, double threshold, int i);
	void simpleAdjacencyOriginal(set<int> * classesInCube, vector<int> * selectedInstances);
	void simpleAdjacency(set<int> * classesInCube, vector<int> * selectedInstances);
	void sharedPointAdjacency(set<int> * classesInCube, vector<int> * selectedInstances);
	void hyperBlockAdjacency(set<int> * classesInCube, vector<int> * selectedInstances);
	void emptySpaceClusters();
	void emptySpacePureCubes();

	void cycleCombinationMode();
	int getCombinationMode();
	bool isCombining();
	void setWillCombine(bool willCombine);
	bool isThresholdMode();
	void setThresholdMode(bool isThresholdMode);

	void toggleHistogramMode(bool isHistogramMode);
	bool getHistogramMode();
	void setFrequencyMode(bool isFrequencyMode);
	bool getFrequencyMode();
	int getVisibleDimensionCount();
	int getVisibleBlockCount();
	void setQuadMode(bool isQuadMode);
	bool getQuadMode();

	vector<SetCluster> getPureCubes();
	vector<DataClass> getClasses();

	bool drawMinLine();
	bool drawCenterLine();
	bool drawMaxLine();
	void setDrawMin(bool drawMin);
	void setDrawCenter(bool drawCenter);
	void setDrawMax(bool drawMax);
	
	void findSubsetOfDimensions(bool isOverlapping);
	string getLinguisticDescription();
	int getOverlappingCount(int pointIndex);

	void createOverlaps();
	void setOverlapMode(bool);
	bool getOverlapMode();
	vector<SetCluster> * getOverlaps();
	int getImpurities(int index);
	map<string, double> getAboveOne();
	
private:
	
	map<string, double> aboveOne;
	
	vector<SetCluster> overlaps;

	bool overlapMode;
	bool histogramMode;
	bool frequencyMode;
	bool quadMode;

	int combinationMode;
	bool willCombine;
	bool thresholdMode;
	std::vector<SetCluster> pureCubes;

	bool readSetNamesBasic;
	bool readDimensionNamesBasic;
	bool readClassNamesBasic;


	// a vector to hold the dimensions containing the data for the sets
	std::vector<Dimension*> dataDimensions;

	// a vector to hold the data class
	std::vector<DataClass> dataClasses;
	// a vector to hold the data sets
	std::vector<DataSet> dataSets;
	// a field to hold the color of the background set
	ColorCustom backgroundColor;
	// a field to hold the color of the selected set
	ColorCustom selectedSetColor;
	// a field to hold the index of the selected set
	int selectedSetIndex;
	// a field to hold the index of the selected class
	int selectedClassIndex;
	// a field to hold the index of the selected hypercube
	int selectedClusterIndex;
	// a field to hold the index of the selected class within a hypercube
	int selectedClusterClassIndex;

	int totalCubeCount;
	int overlappingCubeCount;
	double hypercubeThreshold;

	bool hideSelectorLine;
	bool isDrawBorders;
	bool drawMin;
	bool drawCenter;
	bool drawMax;

	// holds the clusters of the data
	std::vector<SetCluster> clusters;
	// holds the boolean of whether to paint the cluster or not
	bool paintClusters;
	// holds the boolean of whether to use class colors or hypercube colors when painting clusters 
	bool paintClassColors;
	// holds the boolean of whether clusters use mean or median
	bool useMean;
	double radius;
	// holds the graph notes
	std::vector<GraphNote> notes;

	// a field to hold the name of the maximum y value
	std::string yMaxName;
	// a field to hold the name of the minimum y value
	std::string yMinName;
	// a field to hold the name of the x axis
	std::string xAxisName;
	// a field to hold the name of the y axis
	std::string yAxisName;

	// a method to hold the basic intialization of the object fields
	void init();
	// a method to hold the setup of fields to be performed after everything else
	void finalInit();

	// reads the contents of the file, at fileName, into a vector
	std::vector<std::vector<std::string>*>* readFileIntoVector(std::string* fileName);

	// takes the passed string and tokenizes it by commas
	std::vector<std::string>* tokenizeString(std::string* stringToTokenize);

	// removes duplicate strings from the passed vector
	void removeDuplicates(std::vector<std::string>* stringList);

	// reads the contents of the file, at fileName, into a vector
	bool readBasicFile(std::vector<std::vector<std::string>*>* fileContents);

	// separate a hypercube cluster into sub clusters
	void separateCluster(SetCluster cluster);

	// reads the contents of the file, at fileName, into a vector
	void readCustomFile(std::vector<std::vector<std::string>*>* fileContents);

	// parses a command line from a save file
	void parseLine(std::vector<std::string>* lineTokens);

	// counts the number of characters in the passed string(line) that are one of the characters in the passed vector(characters)
	int countCharacters(vector<char>* characters, string* line);



	// an ascending(left to right) merge sort of the passed dimension list by the set at the passed index
	std::list<Dimension*>* mergeSortAscending(std::list<Dimension*>* listToSort, int setIndex);

	// an descending(left to right) merge sort of the passed dimension list by the set at the passed index
	std::list<Dimension*>* mergeSortDescending(std::list<Dimension*>* listToSort, int setIndex);

	// sorts the dimensions to their original places in a merge sort
	std::list<Dimension*>* mergeSortOriginal(std::list<Dimension*>* listToSort);

	
};

#endif
