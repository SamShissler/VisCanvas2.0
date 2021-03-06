#pragma once
#include "DataSet.h"
#include "Dimension.h"
#include "ColorCustom.h"

#include <vector>
#include <string> 

class SetCluster {
public:

	SetCluster();
	// creates a blank cluster
	SetCluster(ColorCustom &clusterColor);
	// creates the cluster with the passed sets
	SetCluster(ColorCustom &clusterColor, std::vector<int>* newSetsInCluster);
	// creates the cluster with the passed sets and values from the passed dimensions
	SetCluster(ColorCustom &clusterColor, std::vector<int>* newSetsInCluster, std::vector<Dimension*>* dimensionToCalculateWith);
	// deletes object
	~SetCluster();

	// gets the calculates the minimum value of dimension for the sets whose indexes are passed(setIndexes)
	static double getMinimumValue(Dimension* dimension, std::vector<int>* setIndexes);
	// gets the calculates the mean value of dimension for the sets whose indexes are passed(setIndexes)
	static double getMeanValue(Dimension* dimension, std::vector<int>* setIndexes);
	// gets the calculates the median value of dimension for the sets whose indexes are passed(setIndexes)
	static double getMedianValue(Dimension* dimension, std::vector<int>* setIndexes);
	// gets the calculates the maximum value of dimension for the sets whose indexes are passed(setIndexes)
	static double getMaximumValue(Dimension* dimension, std::vector<int>* setIndexes);
	// gets the center point of the cube
	vector<double> getCenter();

	// adds the passed set index(setIndex) to the list of sets
	// returns true if the set is added and false if the set was already in the cluster
	bool addSet(int setIndex);
	// removes the set index(setIndex) to the list of sets
	// returns true if the set is removed and false if the set not in the cluster
	bool removeSet(int setIndex);
	// gets the number of sets in the cluster
	int getSetNumber();
	// gets the index of the set in this cluster for the set at the passed index(clusterRelativeIndex), which is the index the set index is stored at
	int getIndexOfSet(int clusterRelativeIndex);

	// gets the minimum value in the cluster for the dimension at the passed index 
	double getMinimum(int dimensionIndex) const;
	// gets the mean value in the cluster for the dimension at the passed index 
	double getMiddle(int dimensionIndex) const;
	// gets the maximum value in the cluster for the dimension at the passed index 
	double getMaximum(int dimensionIndex) const;

	// recalculates the values for the cluster using the passed dimensions
	void calculateValues(std::vector<Dimension*>* dimensionToCalculateWith);
	// inverts the values of the set at the passed index
	void invertValues(int dimensionToInvertValuesAt);
	// move the position of the values in the set(at originalIndex) to the index after indexBeforeNewIndex
	bool moveValues(int originalIndex, int indexOfInsertion);

	int getSize() const;

	// gets the color components of the cluster
	std::vector<double>* getColor();
	// sets the color components of the cluster
	void setColor(std::vector<double>& newColor);
	// sets the color components of the cluster
	void setColor(std::vector<double>* newColor);

	// gets the radius of this cluster
	double getRadius();
	// sets the radius of this cluster and returns the old one
	double setRadius(double newRadius);

	// gets the original set of this cluster
	int getOriginalSet();
	// sets the original set of this cluster and returns the old one
	int setOriginalSet(int newSet);

	// gets the name of this cluster
	std::string* getName();
	// sets the name of this cluster
	void setName(std::string* newName);
	// gets the sets in the cluster
	std::vector<int>* getSets();

	// get whether the the cluster uses the mean or median
	bool isUseMean();
	// set whether the the cluster uses the mean or median
	void setUseMean(bool newUseMean);

	// get whether the cluster is displayed
	bool isDisplayed();
	// set whether the cluster is displayed
	void setDisplayed(bool displayed);

	vector<double> getVirtualCenter(int numOfDimensions);
	double getRatio(int numOfDimensions);

private:
	ColorCustom color;
	std::vector<int> setsInCluster;
	std::vector<double> minimumValues;
	std::vector<double> meanValues;
	std::vector<double> medianValues;
	std::vector<double> maximumValues;

	int size;
	double radius;
	int originalSet;
	bool useMean;
	bool displayed;
	std::string name;


};
