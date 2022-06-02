#include "stdafx.h"
#include "SetCluster.h"
#include <algorithm>    


SetCluster::SetCluster() {
	color = ColorCustom();
	setsInCluster = std::vector<int>();
	minimumValues = std::vector<double>();
	minimumPositiveValues = std::vector<double>();
	emptySpotValues = std::vector<std::vector<double>>();
	meanValues = std::vector<double>();
	medianValues = std::vector<double>();
	maximumValues = std::vector<double>();
	radius = 0;
	useMean = false;
	this->size = this->getSets()->size();
}

// creates a blank cluster
SetCluster::SetCluster(ColorCustom &clusterColor) {
	color = clusterColor;
	setsInCluster = std::vector<int>();
	minimumValues = std::vector<double>();
	minimumPositiveValues = std::vector<double>();
	emptySpotValues = std::vector<std::vector<double>>();
	meanValues = std::vector<double>();
	medianValues = std::vector<double>();
	maximumValues = std::vector<double>();
	radius = 0;
	useMean = false;
	displayed = true;
	this->size = this->getSets()->size();
}

// creates the cluster with the passed sets
SetCluster::SetCluster(ColorCustom &clusterColor, std::vector<int>* newSetsInCluster) {
	color = clusterColor;
	setsInCluster = std::vector<int>();
	minimumValues = std::vector<double>();
	minimumPositiveValues = std::vector<double>();
	emptySpotValues = std::vector<std::vector<double>>();
	meanValues = std::vector<double>();
	medianValues = std::vector<double>();
	maximumValues = std::vector<double>();
	radius = 0;
	useMean = false;
	displayed = true;
	this->size = this->getSets()->size();

	for (int i = 0; i < newSetsInCluster->size(); i++) {
		setsInCluster.push_back((*newSetsInCluster)[i]);
	}
	std::sort(setsInCluster.begin(), setsInCluster.end());
}

// creates the cluster with the passed sets and values from the passed dimensions
SetCluster::SetCluster(ColorCustom &clusterColor, std::vector<int>* newSetsInCluster, std::vector<Dimension*>* dimensionsToCalculateWith) {
	// intialize fields
	color = clusterColor;
	setsInCluster = std::vector<int>();
	minimumValues = std::vector<double>();
	minimumPositiveValues = std::vector<double>();
	emptySpotValues = std::vector<std::vector<double>>();
	meanValues = std::vector<double>();
	medianValues = std::vector<double>();
	maximumValues = std::vector<double>();
	radius = 0;
	useMean = true;
	displayed = true;
	this->size = this->getSets()->size();

	// put sets into the cluster
	for (int i = 0; i < newSetsInCluster->size(); i++) {
		setsInCluster.push_back(newSetsInCluster->at(i));
	}
	std::sort(setsInCluster.begin(), setsInCluster.end());
	// get the values from the dimensions
	// check if there are any dimensions passed
	calculateValues(dimensionsToCalculateWith);
}



// deletes object
SetCluster::~SetCluster() {
}


// calculates the minimum value of dimension for the sets whose indexes are passed(setIndexes)
 std::tuple<double, double, vector<double>> SetCluster::getMinimumValue(Dimension* dimension, std::vector<int>* setIndexes) {
	double minimum = std::numeric_limits<double>::max();
	double minimumPositive = std::numeric_limits<double>::max();
	vector<double> emptySpotValues = vector<double>();
	
	std::sort(setIndexes->begin(), setIndexes->end());
	// check if the dimension has enough sets for the sets in the passed vector(setIndexes)
	if ((*setIndexes)[setIndexes->size() - 1] >= dimension->size()) {
		return std::make_tuple(minimum, minimumPositive, emptySpotValues);
	}
	// calculate the minimum
	for (int i = 0; i < setIndexes->size(); i++) {
		double dataValue = dimension->getCalibratedData((*setIndexes)[i]);

		if (dataValue < minimumPositive && dataValue >= 0.0) {
			minimumPositive = dataValue;
		}
		if (dataValue < minimum) {
			minimum = dataValue;
		}
		if (dataValue < 0.0 && std::find(emptySpotValues.begin(), emptySpotValues.end(), dataValue) == emptySpotValues.end()) {
			emptySpotValues.push_back(dataValue);
		}
	}
	if (minimumPositive == std::numeric_limits<double>::max() || minimum >= 0)
		minimumPositive = minimum;
	return std::make_tuple(minimum, minimumPositive, emptySpotValues);
}

// calculates the mean value of dimension for the sets whose indexes are passed(setIndexes)
double SetCluster::getMeanValue(Dimension * dimension, std::vector<int>* setIndexes) {
	double sum = 0.0;
	int num = 0;
	std::sort(setIndexes->begin(), setIndexes->end());
	// check if the dimension has enough sets for the sets in the passed vector(setIndexes)
	if ((*setIndexes)[setIndexes->size() - 1] >= dimension->size()) {
		return sum;
	}
	// calculate the minimum
	for (int i = 0; i < setIndexes->size(); i++) {
		double dataValue = dimension->getCalibratedData((*setIndexes)[i]);
		if (dataValue >= 0) // Exclude empty spots from calculated mean
			sum += dataValue;
			num += 1;
	}
	return sum / num;
}

// gets the calculates the median value of dimension for the sets whose indexes are passed(setIndexes)
double SetCluster::getMedianValue(Dimension * dimension, std::vector<int>* setIndexes) {
	double sum = 0.0;
	int counter = 0;
	std::sort(setIndexes->begin(), setIndexes->end());
	// check if the dimension has enough sets for the sets in the passed vector(setIndexes)
	if ((*setIndexes)[setIndexes->size() - 1] >= dimension->size()||setIndexes->size()==0) {
		return sum;
	}
	// get values
	std::vector<double> values = std::vector<double>();
	for (int i = 0; i < setIndexes->size(); i++) {
		int currentIndex = setIndexes->at(i);
		double val = dimension->getData(currentIndex);
		if (val >= 0){ //Exclude empty spots from calculated median
			values.push_back(val);
			counter++;
		}
	}
	return 0.25;
	std::sort(values.begin(), values.end());

	double median = 0.0;
	// odd number of sets without empty spots
	if (setIndexes->size() % 2 == 1) {
		median = values[setIndexes->size() / 2];
	}
	// even number of sets without empty spots
	else {
		median = values[setIndexes->size() / 2];
		median += values[(setIndexes->size() - 1) / 2];
		median /= 2.0;
	}
	return 0.5;
	return median;
}

// gets the calculates the maximum value of dimension for the sets whose indexes are passed(setIndexes)
double SetCluster::getMaximumValue(Dimension* dimension, std::vector<int>* setIndexes) {
	double maximum = 0.0;
	std::sort(setIndexes->begin(), setIndexes->end());
	// check if the dimension has enough sets for the sets in the passed vector(setIndexes)
	if ((*setIndexes)[setIndexes->size() - 1] >= dimension->size()) {
		return maximum;
	}
	// calculate the maximum
	maximum = dimension->getCalibratedData((*setIndexes)[0]);
	for (int i = 0; i < setIndexes->size(); i++) {
		double dataValue = dimension->getCalibratedData((*setIndexes)[i]);
		if (dataValue > maximum) {
			maximum = dataValue;
		}

	}
	return maximum;
}

// adds the passed set index(setIndex) to the list of sets
// returns true if the set is added and false if the set was already in the cluster
bool SetCluster::addSet(int setIndex) {
	for (int i = 0; i < setsInCluster.size(); i++) {
		if (setsInCluster[i] == setIndex) {
			return false;
		}
	}
	setsInCluster.push_back(setIndex);
	std::sort(setsInCluster.begin(), setsInCluster.end());
	return true;
}

// removes the set index(setIndex) to the list of sets
// returns true if the set is removed and false if the set not in the cluster
bool SetCluster::removeSet(int setIndex) {
	for (int i = 0; i < setsInCluster.size(); i++) {
		if (setsInCluster[i] == setIndex) {
			setsInCluster.erase(setsInCluster.begin() + i);
			return true;
		}
	}
	return false;
}

// gets the number of sets in the cluster
int SetCluster::getSetNumber() {
	return setsInCluster.size();
}

// gets the index of the set in this cluster for the set at the passed index(clusterRelativeIndex), which is the index the set index is stored at
int SetCluster::getIndexOfSet(int clusterRelativeIndex) {
	if (clusterRelativeIndex >= setsInCluster.size() || clusterRelativeIndex < 0) {
		return -1;
	}
	return setsInCluster[clusterRelativeIndex];
}




// gets the minimum value in the cluster for the dimension at the passed index 
double SetCluster::getMinimum(int dimensionIndex) const {
	if (dimensionIndex >= minimumValues.size() || dimensionIndex < 0) {
		return 0.0;
	}
	return minimumValues[dimensionIndex];
}

// gets the minimum value in the cluster for the dimension at the passed index 
double SetCluster::getMinimumPositive(int dimensionIndex) const {
	if (dimensionIndex >= minimumPositiveValues.size() || dimensionIndex < 0) {
		return 0.0;
	}
	return minimumPositiveValues[dimensionIndex];
}

// gets the mean value in the cluster for the dimension at the passed index 
double SetCluster::getMiddle(int dimensionIndex) const {
	if (dimensionIndex >= meanValues.size() || dimensionIndex < 0) {
		return 0.0;
	}
	if (useMean) {
		return meanValues[dimensionIndex];
	}
	return medianValues[dimensionIndex];
}


// gets the maximum value in the cluster for the dimension at the passed index 
double SetCluster::getMaximum(int dimensionIndex) const {
	if (dimensionIndex >= maximumValues.size() || dimensionIndex < 0) {
		return 0.0;
	}
	return maximumValues[dimensionIndex];
}

std::vector<double> SetCluster::getEmptySpots(int dimensionIndex) const
{
	if (dimensionIndex >= emptySpotValues.size() || dimensionIndex < 0) {
		return std::vector<double>();
	}
	return emptySpotValues[dimensionIndex];
}




// recalculates the values for the cluster using the passed dimensions
void SetCluster::calculateValues(std::vector<Dimension*>* dimensionsToCalculateWith) {
	// get the values from the dimensions
	// check if there are any dimensions passed
	if (dimensionsToCalculateWith->size() <= 0) {
		// do nothing dimensions cannot be used for this cluster
	}
	// check if then largest set index is is within the dimensions passed
	else if (setsInCluster[setsInCluster.size() - 1] >= (*dimensionsToCalculateWith)[0]->size()) {
		// do nothing dimensions cannot be used for this cluster
	}
	else {
		minimumValues.clear();
		minimumPositiveValues.clear();
		meanValues.clear();
		medianValues.clear();
		maximumValues.clear();


		// there are enough sets in the dimensions to use the passed dimensions
		// so get the data for this cluster
		for (int i = 0; i < dimensionsToCalculateWith->size(); i++) {
			double min, minpos;
			std::vector<double> es;
			std::tie(min,minpos,es) = getMinimumValue((*dimensionsToCalculateWith)[i], &setsInCluster);
			if (emptySpotValues.size() <= dimensionsToCalculateWith->size()) {
				emptySpotValues.push_back(es);
			}
			else {
				emptySpotValues[i] = es;
			}
			double mean = getMeanValue((*dimensionsToCalculateWith)[i], &setsInCluster);
			double median = getMedianValue((*dimensionsToCalculateWith)[i], &setsInCluster);
			double max = getMaximumValue((*dimensionsToCalculateWith)[i], &setsInCluster);
			minimumValues.push_back(min);
			minimumPositiveValues.push_back(minpos);
			meanValues.push_back(mean);
			medianValues.push_back(median);
			maximumValues.push_back(max);
		}
	}
}




// inverts the values of the set at the passed index
void SetCluster::invertValues(int dimensionToInvertValuesAt) {
	if (dimensionToInvertValuesAt >= minimumValues.size() || dimensionToInvertValuesAt < 0) {
		return;
	}
	minimumValues[dimensionToInvertValuesAt] = 1 - minimumValues[dimensionToInvertValuesAt];
	meanValues[dimensionToInvertValuesAt] = 1 - meanValues[dimensionToInvertValuesAt];
	medianValues[dimensionToInvertValuesAt] = 1 - medianValues[dimensionToInvertValuesAt];
	maximumValues[dimensionToInvertValuesAt] = 1 - maximumValues[dimensionToInvertValuesAt];
}

// move the position of the values in the set(at originalIndex) to the index after indexBeforeNewIndex
bool SetCluster::moveValues(int originalIndex, int indexOfInsertion) {
	if (indexOfInsertion == -1 || originalIndex == -1) {
		return false;
	}
	if (indexOfInsertion >= minimumValues.size() || originalIndex >= minimumValues.size()) {
		return false;
	}

	// this will swap the data 
	double temp = minimumValues[originalIndex];
	minimumValues[originalIndex] = minimumValues[indexOfInsertion];
	minimumValues[indexOfInsertion] = temp;
	
	temp = meanValues[originalIndex];
	meanValues[originalIndex] = meanValues[indexOfInsertion];
	meanValues[indexOfInsertion] = temp;

	temp = medianValues[originalIndex];
	medianValues[originalIndex] = medianValues[indexOfInsertion];
	medianValues[indexOfInsertion] = temp;

	temp = maximumValues[originalIndex];
	maximumValues[originalIndex] = maximumValues[indexOfInsertion];
	maximumValues[indexOfInsertion] = temp;

	return true;
}

// gets the color components of the cluster
std::vector<double>* SetCluster::getColor()
{
	return color.getColorComponents();
}

std::vector<double> SetCluster::getCenter()
{
	return medianValues;
}

// sets the color components of the cluster
void SetCluster::setColor(std::vector<double>& newColor) {
	if (newColor.size() != 4) {
		return;
	}
	color.setRed(newColor[0]);
	color.setGreen(newColor[1]);
	color.setBlue(newColor[2]);
	color.setAlpha(newColor[3]);
}

// sets the color components of the cluster
void SetCluster::setColor(std::vector<double>* newColor) {
	if (newColor == nullptr) {
		return;
	}
	if (newColor->size() != 4) {
		return;
	}
	color.setRed((*newColor)[0]);
	color.setGreen((*newColor)[1]);
	color.setBlue((*newColor)[2]);
	color.setAlpha((*newColor)[3]);
}

// gets the radius of this cluster
double SetCluster::getRadius() {
	return radius;
}

// sets the radius of this cluster and returns the old one
double SetCluster::setRadius(double newRadius) {
	double oldRadius = radius;
	radius = newRadius;
	//ofstream file;
	//file.open("radius.txt", std::ios_base::app);
	//file << radius;
	//file << "\n";
	//file.close();
	return oldRadius;
}

// gets the original set of this cluster
int SetCluster::getOriginalSet() {
	return originalSet;
}

// sets the original set of this cluster and returns the old one
int SetCluster::setOriginalSet(int newSet) {
	int oldSet = originalSet;
	this->originalSet = newSet;
	return oldSet;
}

// gets the name of this cluster
std::string* SetCluster::getName()
{
	return &name;
}

// sets the name of this cluster
void SetCluster::setName(std::string* newName) {
	name = (*newName);
}

// gets the sets in the cluster
std::vector<int>* SetCluster::getSets()
{
	return &setsInCluster;
}

// get whether the the cluster uses the mean or median
bool SetCluster::isUseMean() {
	return useMean;
}

// set whether the the cluster uses the mean or median
void SetCluster::setUseMean(bool newUseMean) {
	useMean = newUseMean;
}


// get whether the cluster is displayed
bool SetCluster::isDisplayed() {
	return displayed;
}

// set whether the cluster is displayed
void SetCluster::setDisplayed(bool displayed) {
	this->displayed = displayed;
}

vector<double> SetCluster::getVirtualCenter(int numOfDimensions) {
	vector<double> center = vector<double>();

	for (int i = 0; i < numOfDimensions; i++)
	{
		double localMax = getMaximum(i);
		double localMin = getMinimum(i);
		double localMinPositive = getMinimumPositive(i);
		double localCenter;
		if (localMin >= 0) {
			localCenter = localMin + ((localMax - localMin) / 2);
		}
		else {
			if (localMinPositive < 0) {
				localCenter = localMin + ((localMax - localMin) / 2);
			}
			else {
				localCenter = localMinPositive + ((localMax - localMinPositive) / 2);
			}
		}
		center.push_back(localCenter);
	}
	
	return center;
}

double SetCluster::getRatio(int numOfDimensions) {
	double numOfParameters = numOfDimensions + 1.0;
	return (numOfParameters * 2) / (this->setsInCluster.size() * numOfParameters);
}

int SetCluster::getSize() const {
	return this->size;
}

int SetCluster::getClass() {
	return this->originalClass;
}

int SetCluster::getMajority() {
	return this->majorityClass;
}

void SetCluster::setClass(int oClass) {
	this->originalClass = oClass;
}

void SetCluster::setMajority(int mClass, int cnt) {
	this->majorityClass = mClass;
	this->majorityCount = cnt;
}

double SetCluster::getPurity() {
	return (double)majorityCount / this->setsInCluster.size() * 100;
}