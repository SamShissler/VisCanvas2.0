#include "stdafx.h"
#include "Dimension.h"
#include "DataNode.h"
#include <vector>
#include <string>

using namespace std;

// create a dimension of size 0 at index 0
Dimension::Dimension() {
	originalIndex = 0;
	name = std::to_string(originalIndex);
	data = vector<DataNode*>();
	shiftAmount = 0.0;
	inverted = false;
	isDrawn = true;

	useArtificialCalibration = false;
	artificialMaximum = 1.0;
	artificialMinimum = 0.0;
}

// copies pointer to a new pointer.
Dimension* Dimension::copyDimensionToPtr(Dimension& toCopy)
{
	Dimension* toReturn = new Dimension();

	toReturn->setOriginalIndex(toCopy.getOriginalIndex());
	toReturn->setName(toCopy.getName());

	//copy over data
	toReturn->setAllData(toCopy.getAllData());
	return toReturn;
}

// sets all of the data for all of the sets.
void Dimension::setAllData(vector<DataNode*> toSet)
{
	vector<DataNode*> finalData;

	for (int i = 0; i < toSet.size(); i++)
	{
		DataNode* datan = toSet.at(i)->copyOfDataNode();
		finalData.push_back(datan);
	}

	this->data = finalData;
}

// create a dimension for the pass index(index) and passed size(size)
Dimension::Dimension(int index, int size) {
	originalIndex = index;

	name = std::to_string(index);

	data = vector<DataNode*>();
	for (int i = 0; i < size; i++) {
		data.push_back(new DataNode(0.0));
	}
	shiftAmount = 0.0;
	inverted = false;
	isDrawn = true;

	useArtificialCalibration = false;
	artificialMaximum = 1.0;
	artificialMinimum = 0.0;
}

// delete the object
Dimension::~Dimension() {
	for (unsigned int i = 0; i < data.size(); i++) {
		delete data[i];
	}
	data.clear();
}

// get the index of the the dimension was created with
int Dimension::getOriginalIndex() const {
	return originalIndex;
}

// set the index of the the dimension, and returns the old index
int Dimension::setOriginalIndex(int newIndex) {
	int oldIndex = originalIndex;
	originalIndex = newIndex;
	return oldIndex;
}

vector<DataNode*> Dimension::getAllData() {
	return this->data;
}

bool Dimension::isVisible() {
	return this->isDrawn;
}

void Dimension::setVisibility(bool isDrawn) {
	this->isDrawn = isDrawn;
}

// calibrate the data to the [0,1] space
void Dimension::calibrateData() {
	// reset data
	for (int i = 0; i < this->size(); i++) {
		(data[i])->resetData();
	}

	double maximum = getMaximum();
	double minimum = getMinimum();
	if (useArtificialCalibration) {
		maximum = artificialMaximum;
		minimum = artificialMinimum;
	}
	double range = maximum - minimum;

	if (fabs(maximum - minimum) < 0.01)
	{
		for (int i = 0; i < data.size(); i++)
		{
			(*data[i]).setDataCurrent(0.5);
		}
	}
	else
	{
		for (unsigned int i = 0; i < data.size(); i++) { //Normalize data
			(*data[i]).setDataCurrent(((*data[i]).getData() - minimum) / range);
		}
	}
}

// gets the data for the set of the passed index(dataIndex)
double Dimension::getData(int dataIndex) const {
	if (dataIndex >= size() || dataIndex < 0) {
		return 0.0;
	}
	double dataReturn = (*data[dataIndex]).getData();
	if (inverted && dataReturn > 0) { // If value is an empty spot leave at bottom of graph
		dataReturn = 1 - dataReturn;
	}
	dataReturn += shiftAmount;
	return dataReturn;
}

// gets the data calibrated, but not inverted or shifted data for the set of the passed index(dataIndex)
double Dimension::getCalibratedData(int dataIndex) const {
	if (dataIndex >= size() || dataIndex < 0) {
		return 0.0;
	}
	double dataReturn = (*data[dataIndex]).getData();
	if (inverted && dataReturn > 0) { //If value is an empty spot leave at bottom of graph
			dataReturn = 1 - dataReturn;
	}
	return dataReturn;
}

// gets the original data for the set of the passed index(dataIndex)
double Dimension::getOriginalData(int dataIndex) const {
	if (dataIndex >= size() || dataIndex < 0) {
		return 0.0;
	}
	return (*data[dataIndex]).getOriginalData();
}

// gets the name
string * Dimension::getName() {
	return &name;
}


// sets the name and returns the old name
void Dimension::setName(string * newName) {
	name = *newName;
}

// sets the data of the set at the passed index(dataIndex) to the passed value(newData), alters the original data
void Dimension::setData(int dataIndex, double newData) {
	if (dataIndex >= size() || dataIndex < 0) {
		return;
	}
	(*data[dataIndex]).setData(newData);
}

// deletes the data at the passed index.
void Dimension::deleteData(int dataIndex)
{
	//Go over data.
	int index = 0;
	for (auto it = data.begin(); it != data.end(); it++)
	{
		if (index == dataIndex)
		{
			data.erase(it);
			break;
		}

		index++;
	}
}


// multiplies all the data in the dimension by the passed double, does not alter original data
void Dimension::multiplyData(double multiplier) {
	for (unsigned int i = 0; i < data.size(); i++) {
		(*data[i]).divideData(multiplier);
	}
}

// divides all the data in the dimension by the passed double, does not alter original data
void Dimension::divideData(double divisor) {
	for (unsigned int i = 0; i < data.size(); i++) {
		(*data[i]).divideData(divisor);
	}
}

// adds the passed double to all the data in the dimension, does not alter original data
void Dimension::addToData(double addend) {
	for (unsigned int i = 0; i < data.size(); i++) {
		(*data[i]).addToData(addend);
	}
}

// adds the amount passed(shiftAmount) to the data shift amount
void Dimension::shiftDataBy(double modToShiftAmount) {
	shiftAmount += modToShiftAmount;
}

// gets the amount data shift amount
double Dimension::getShift() {
	return shiftAmount;
}

bool Dimension::isInverted() {
	return inverted;
}

// toggles whether the data is inverted
void Dimension::invert() {
	inverted = !inverted;
}


// gets the number of sets in the dimensions
int Dimension::size() const {
	return data.size();
}

// returns whether there is artificial calibration or not
bool Dimension::isArtificiallyCalibrated() {
	return useArtificialCalibration;
}

// sets the calibration to use the data's(not the artificial) maximum and minimum
void Dimension::clearArtificialCalibration() {
	useArtificialCalibration = false;
	calibrateData();
}

// sets the bounds to be used for artificial calibration
void Dimension::setCalibrationBounds(double newMaximum, double newMinimum) {
	useArtificialCalibration = true;
	artificialMaximum = newMaximum;
	artificialMinimum = newMinimum;
	calibrateData();
}

// gets the artficial calibration maximum for the dimension
double Dimension::getArtificialMaximum() const {
	return artificialMaximum;
}

// gets the artficial calibration maximum for the dimension
double Dimension::getArtificialMinimum() const {
	return artificialMinimum;
}

// gets the maximum data value in the dimension
double Dimension::getMaximum() const {
	if (size() == 0) {
		return 0.0;
	}
	double maximum = (*data[0]).getData();
	for (unsigned int i = 1; i < data.size(); i++) {
		if (maximum <(*data[i]).getData()) {
			maximum = (*data[i]).getData();
		}
	}
	return maximum;
}

// gets the minimum data value in the dimension
double Dimension::getMinimum() const {
	if (this->size() == 0) {
		return 0.0;
	}
	double minimum = (*data[0]).getData();
	for (unsigned int i = 1; i < data.size(); i++) {
		if (minimum > (*data[i]).getData() && (*data[i]).getData() >= 0.0) {//excludes empty spots
			minimum = (*data[i]).getData();
		}
	}
	return minimum;
}

double Dimension::spaceForBlocks(int numEmpty, int setIndex) const{
	double max = 0.95;
	double min = 0.05;
	return (max - ((max - min) / (numEmpty - 1)) * -10 * ((data[setIndex])->getData() + 0.2));
}