#include "stdafx.h"
#include "DataNode.h"
#include "Dimension.h"
#include "ColorCustom.h"
#include "DataClass.h"
#include "DataSet.h"
#include "SetCluster.h"
#include "VisCanvas.h"

#include <vector>
#include <set>
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <time.h>

using namespace std;

// create a blank class without any data
DataInterface::DataInterface() {
	readSetNamesBasic = true;
	readDimensionNamesBasic = true;
	readClassNamesBasic = true;

	backgroundColor = ColorCustom();
	backgroundColor.setRed(0.5);
	backgroundColor.setGreen(0.5);
	backgroundColor.setBlue(0.75);
	backgroundColor.setAlpha(1.0);
	radius = 0.2;
	init();
	finalInit();
}


// delete the object
DataInterface::~DataInterface() {
	/*
	for (unsigned int i = 0; i < dataDimensions.size(); i++) {
	delete dataDimensions[i];
	}
	*/
	dataDimensions.clear();
}


// parse the data in the file at the passed path into the object
bool DataInterface::readFile(string * filePath) {
	std::vector<std::vector<std::string>*>* fileLines = readFileIntoVector(filePath);
	if (fileLines == nullptr) {
		return false;
	}
	if (fileLines->size() < 2) {
		return false;
	}
	if ((*fileLines)[0]->size() <= 0) {
		return false;
	}
	// find if a custom file or a basic file
	// find first none blank line
	bool isCustomFileFormat = false;
	if (isCustomFileFormat = ((*(*fileLines)[0])[0] == "VisCanvas Save File Format")) {
		isCustomFileFormat = true;
	}

	for (unsigned int i = 0; i < dataDimensions.size(); i++) {
		delete dataDimensions[i];
	}
	dataDimensions.clear();
	dataClasses.clear();
	dataSets.clear();
	clusters.clear();
	notes.clear();
	init();

	try {
		if (isCustomFileFormat) {
			readCustomFile(fileLines);
		}
		else {
			readBasicFile(fileLines);
		}
	}
	catch (...) {
		dataDimensions.clear();
		dataClasses.clear();
		dataSets.clear();
		clusters.clear();
		notes.clear();
		init();
		finalInit();
		return false;
	}
	// perform final setup operations
	finalInit();
	return true;
}


// saves the data and all settings to the file at the passed location
bool DataInterface::saveToFile(std::string * filePath) {
	ofstream saveFile;
	saveFile.open(*filePath);
	// check if file is open
	if (saveFile.is_open() == false) {
		return false;
	}
	// write file type identifier
	saveFile << "VisCanvas Save File Format,";
	saveFile << ",";
	// write dimension names
	for (int i = 0; i < getDimensionAmount(); i++) {
		saveFile << *(dataDimensions[i]->getName());
		if (i < getDimensionAmount() - 1) {
			saveFile << ",";
		}
	}
	saveFile << "\n";
	// write data sets, their class and data
	for (int j = 0; j < getSetAmount(); j++) {
		// write set class and name
		std::string className = *(getClassName(getClassOfSet(j)));
		std::string setName = *(getSetName(j));
		saveFile << className << "," << setName << ",";
		for (int i = 0; i < getDimensionAmount(); i++) {
			double currentData = dataDimensions[i]->getOriginalData(j);
			saveFile << currentData;
			if (i < getDimensionAmount() - 1) {
				saveFile << ",";
			}
		}
		saveFile << "\n";
	}
	// put space between data and commands
	saveFile << "\n\n";

	// write set classes
	saveFile << "classes\n";
	for (int i = 1; i < getClassAmount(); i++) {
		std::vector<double>* color = getClassColor(i);
		saveFile << (*getClassName(i)) << ",";
		saveFile << (*color)[0] << "," << (*color)[1] << "," << (*color)[2] << "," << (*color)[3];
		saveFile << "\n";
	}
	saveFile << "\n\n";



	saveFile << "Commands\n";


	// print dimension shifts
	for (int i = 0; i < getDimensionAmount(); i++) {
		double shift = dataDimensions[i]->getShift();
		// make sure shift is large enough to bother with
		if (abs(shift) > 0.001) {
			saveFile << "shift," << i << "," << shift << "\n";
		}
	}

	// print dimension bounds
	for (int i = 0; i < getDimensionAmount(); i++) {
		double min = dataDimensions[i]->getArtificialMinimum();
		double max = dataDimensions[i]->getArtificialMaximum();
		// make the artificial calibration exists
		if (dataDimensions[i]->isArtificiallyCalibrated()) {
			saveFile << "bounds," << i << "," << max << "," << min << "\n";
		}
	}

	// print dimension order
	saveFile << "original indexes,";
	for (int i = 0; i < getDimensionAmount(); i++) {
		int originalIndex = this->dataDimensions[i]->getOriginalIndex();
		saveFile << originalIndex;
		if (i < getDimensionAmount() - 1) {
			saveFile << ",";
		}
	}
	saveFile << "\n";

	for (int i = 0; i < getClusterAmount(); i++) {
		saveFile << "hypercube,";
		saveFile << clusters[i].getOriginalSet() << ",";
		saveFile << clusters[i].getRadius() << "\n";
	}

	// print whether to use mean or median for clusters
	saveFile << "useMean,";
	saveFile << useMean << "\n";

	// put artificial calibration in the list of 'commands'
	for (int i = 0; i < getDimensionAmount(); i++) {
		if(isArtificiallyCalibrated(i)){
			saveFile << "artificial calibration,";
			saveFile << i << ",";
			saveFile << dataDimensions[i]->isArtificiallyCalibrated() << ",";
			saveFile << dataDimensions[i]->getArtificialMaximum() << ",";
			saveFile << dataDimensions[i]->getArtificialMinimum() << "\n";
		}
	}

	// put default class color in
	std::vector<double>* defaultClassColor = getClassColor(0);
	saveFile << "Default Class Color,";
	saveFile << (*defaultClassColor)[0] << ",";
	saveFile << (*defaultClassColor)[1] << ",";
	saveFile << (*defaultClassColor)[2] << ",";
	saveFile << (*defaultClassColor)[3] << "\n";

	// put background color in
	std::vector<double>* componentsBackgroundColor = backgroundColor.getColorComponents();
	saveFile << "Background Color,";
	saveFile << (*componentsBackgroundColor)[0] << ",";
	saveFile << (*componentsBackgroundColor)[1] << ",";
	saveFile << (*componentsBackgroundColor)[2] << ",";
	saveFile << (*componentsBackgroundColor)[3] << "\n";

	// selected set color in
	std::vector<double>* componentsSelectedSetColor = selectedSetColor.getColorComponents();
	saveFile << "Selected Set Color,";
	saveFile << (*componentsSelectedSetColor)[0] << ",";
	saveFile << (*componentsSelectedSetColor)[1] << ",";
	saveFile << (*componentsSelectedSetColor)[2] << ",";
	saveFile << (*componentsSelectedSetColor)[3] << "\n";

	// put inverted dimensions in
	for (int i = 0; i < getDimensionAmount(); i++) {
		if (isDimensionInverted(i)) {
			saveFile << "Invert Dimension," << i << "\n";
		}
	}


	// end the file
	saveFile << "\n";
	saveFile.close();
}

// sets whether set names will be read from basic files
bool DataInterface::setReadSetNames(bool newReadSetNames) {
	bool previousVal = readSetNamesBasic;
	readSetNamesBasic = newReadSetNames;
	return previousVal;
}

// sets whether dimension names will be read from basic files
bool DataInterface::setReadDimensionNames(bool newReadDimensionNames) {
	bool previousVal = readDimensionNamesBasic;
	readDimensionNamesBasic = newReadDimensionNames;
	return previousVal;
}

// sets whether class names will be read from basic files
bool DataInterface::setReadClassNames(bool newReadClassNames) {
	bool previousVal = readDimensionNamesBasic;
	readClassNamesBasic = newReadClassNames;
	return previousVal;
}

// returns whether set names will be read from basic files
bool DataInterface::isReadSetNames() {
	return readSetNamesBasic;
}

// returns whether dimension names will be read from basic files
bool DataInterface::isReadDimensionNames() {
	return readDimensionNamesBasic;
}

// returns whether class names will be read from basic files
bool DataInterface::isReadClassNames() {
	return readClassNamesBasic;
}







// gets the number of classes in the data
int DataInterface::getClassAmount() const {
	return dataClasses.size();
}

/*
Gets the total number of sets in the data
full implementation after file parsing
*/
int DataInterface::getSetAmount() const {
	if (dataDimensions.size() == 0) {
		return 0;
	}
	return (*dataDimensions[0]).size();
}

// gets the number of dimensions in the data
int  DataInterface::getDimensionAmount() const {
	return dataDimensions.size();
}


// inverts the dimension
void DataInterface::invertDimension(int dimensionIndex) {
	if (dimensionIndex >= this->getDimensionAmount() || dimensionIndex < 0) {
		return;
	}

	(dataDimensions[dimensionIndex])->invert();
}

// checks whether the dimension is inverted
bool DataInterface::isDimensionInverted(int dimensionIndex) {
	if (dimensionIndex >= this->getDimensionAmount() || dimensionIndex < 0) {
		return false;
	}

	return (dataDimensions[dimensionIndex])->isInverted();
}





// gets the data in the passed set at the passed index
double DataInterface::getData(int setIndex, int indexOfData) const {
	// check if indexes are in bounds
	if (setIndex >= getSetAmount() || setIndex < 0) {
		return 0.0;
	}
	if (indexOfData >= getDimensionAmount() || indexOfData < 0) {
		return 0.0;
	}

	// get data
	return (*dataDimensions[indexOfData]).getData(setIndex);
}

// gets the original data in the set of the passed index(setIndex), for the passed dimension(indexOfData)
double DataInterface::getOriginalData(int setIndex, int dimensionIndex) const {
	// check if indexes are in bounds
	if (setIndex >= getSetAmount() || setIndex < 0) {
		return 0.0;
	}
	if (dimensionIndex >= getDimensionAmount() || dimensionIndex < 0) {
		return 0.0;
	}

	// get data
	return (*dataDimensions[dimensionIndex]).getOriginalData(setIndex);
}


// sets the data in the set of the passed index(setIndex), for the passed dimension(indexOfData), to the passed value(newDataValue)
double DataInterface::setData(int setIndex, int indexOfData, double newDataValue) {
	// check if indexes are in bounds
	if (setIndex >= getSetAmount() || setIndex < 0) {
		return 0.0;
	}
	if (indexOfData >= getDimensionAmount() || indexOfData < 0) {
		return 0.0;
	}

	// get data
	double oldData = (*dataDimensions[indexOfData]).getOriginalData(setIndex);
	(*dataDimensions[indexOfData]).setData(setIndex, newDataValue);
	(*dataDimensions[indexOfData]).calibrateData();
	return oldData;
}

/*
moves the position of a dimensions from the passed index(indexOfDimension) to at the other passed index(indexOfInsertion)
Note: the new position is calculated before the dimension being moved is removed from its place so if the data had dimensions 0,1,2,3
and the call moveData(1, 3) was made the dimensions would become 0,2,1,3 not 0,2,3,1
*/
bool DataInterface::moveData(int indexOfDimension, int indexOfInsertion) {
	if (indexOfInsertion == -1 || indexOfDimension == -1) {
		return false;
	}
	if (indexOfInsertion >= getDimensionAmount() || indexOfDimension >= getDimensionAmount()) {
		return false;
	}

	// this will swap the data
	Dimension *temp = dataDimensions[indexOfDimension]; // temp = a
	dataDimensions[indexOfDimension] = dataDimensions[indexOfInsertion]; // a = b
	dataDimensions[indexOfInsertion] = temp; // b = temp

	return true;

	// std::swap(dataDimensions[indexOfDimension], dataDimensions[indexOfInsertion]);
	// return true;
}




// gets the name of the class at the passed index(classIndex) to the passed string(newName)
std::string * DataInterface::getDimensionName(int dimensionIndex) {
	if (dimensionIndex >= dataDimensions.size() || dimensionIndex < 0) {
		return nullptr;
	}
	return dataDimensions[dimensionIndex]->getName();
}

// sets the name of the class at the passed index(classIndex)
void DataInterface::setDimensionName(int dimensionIndex, string * newName) {
	if (dimensionIndex >= dataDimensions.size() || dimensionIndex < 0) {
		return;
	}
	dataDimensions[dimensionIndex]->setName(newName);
}

// gets whether the calibration for the dimension(dimensionIndex) is artificial or relative
bool DataInterface::isArtificiallyCalibrated(int dimensionIndex) {
	if (dimensionIndex >= dataDimensions.size() || dimensionIndex < 0) {
		return false;
	}
	return dataDimensions[dimensionIndex]->isArtificiallyCalibrated();
}

// sets the calibration to use the data's(not the artificial) maximum and minimum in dimension at the passed index(dimensionIndex)
void DataInterface::clearArtificialCalibration(int dimensionIndex) {
	if (dimensionIndex >= dataDimensions.size() || dimensionIndex < 0) {
		return;
	}
	dataDimensions[dimensionIndex]->clearArtificialCalibration();
}

// sets the bounds to be used for artificial calibration at the passed index(dimensionIndex)
void DataInterface::setCalibrationBounds(int dimensionIndex, double newMaximum, double newMinimum) {
	if (dimensionIndex >= dataDimensions.size() || dimensionIndex < 0) {
		return;
	}

	// take care of this if the user puts the wrong input for min-max
	if (newMinimum > newMaximum) {
		// swap them
		double temp = newMinimum;
		newMinimum = newMaximum;
		newMaximum = temp;
	}

	dataDimensions[dimensionIndex]->setCalibrationBounds(newMaximum, newMinimum);
}
// gets the artificial maximum for the dimension at the passed index(dimensionIndex)
double DataInterface::getArtificialMaximum(int dimensionIndex) const {
	if (dimensionIndex >= dataDimensions.size() || dimensionIndex < 0) {
		return 0.0;
	}
	return dataDimensions[dimensionIndex]->getArtificialMaximum();
}
// gets the artificial minimum for the dimension at the passed index(dimensionIndex)
double DataInterface::getArtificialMinimum(int dimensionIndex) const {
	if (dimensionIndex >= dataDimensions.size() || dimensionIndex < 0) {
		return 0.0;
	}
	return dataDimensions[dimensionIndex]->getArtificialMinimum();
}


// add the passed double(amountToAdd) to all the data in the dimension at the index(dimensionIndex)
void DataInterface::addToDimension(int dimensionIndex, double amountToAdd) {
	// don't accept out of bounds data set indexes
	if (dimensionIndex >= getDimensionAmount() || dimensionIndex < 0) {
		return;
	}
	dataDimensions[dimensionIndex]->addToData(amountToAdd);
}

vector<Dimension*> * DataInterface::getDataDimensions() {
	return &dataDimensions;
}






// gets the name of the class at the passed index
// full implementation after file parsing
string* DataInterface::getClassName(int classIndex) {
	if (classIndex >= dataClasses.size() || classIndex < 0) {
		return nullptr;
	}
	return dataClasses[classIndex].getName();
}

// sets the name of the class at the passed index to the passed string and returns the old name
// full implementation after file parsing
void DataInterface::setClassName(int classIndex, string* newName) {
	if (dataClasses.size() <= classIndex || classIndex < 0) {
		return;
	}
	dataClasses[classIndex].setName(newName);
}

// adds a new class to the list of classes
void DataInterface::addClass() {
	dataClasses.push_back(DataClass(dataClasses.size(), "Class " + std::to_string(dataClasses.size())));
}

// delete a class from the list of classes
void DataInterface::deleteClass(int classIndex) {
	// do not allow deletion of default class
	if (classIndex <= 0 || classIndex >= getClassAmount()) {
		return;
	}

	// put the sets in the class to be deleted in the default class
	for (int i = 0; i < this->getSetAmount(); i++) {
		if (dataSets[i].getClass() == classIndex) {
			dataSets[i].setDataClass(0);
		}
	}

	// delete the class
	dataClasses.erase(dataClasses.begin() + classIndex);
}

/*
Gets the number of sets in the class at the passed index(classIndex)
*/
int DataInterface::getSetAmount(int classIndex) const {
	if (classIndex < 0 || classIndex >= getClassAmount()) {
		return -1;
	}
	return dataClasses[classIndex].getSetNumber();
}

std::vector<int>* DataInterface::getSetsInClass(int classIndex) {
	// do not allow deletion of default class
	if (classIndex < 0 || classIndex >= getClassAmount()) {
		return nullptr;
	}
	return dataClasses[classIndex].getSetsInClass();
}

// sets the name of the class at the passed index(classIndex) to the passed string(newName)
std::vector<double>* DataInterface::getClassColor(int classIndex) {
	if (classIndex < 0 || classIndex >= dataClasses.size()) {
		return dataClasses[0].getColor();
	}
	return dataClasses[classIndex].getColor();
}

/*
Gets the number of sets in the class at the passed index(classIndex)
*/
void DataInterface::setClassColor(int classIndex, std::vector<double>* newColor) {
	if (classIndex < 0 || classIndex >= dataClasses.size()) {
		return;
	}
	if (newColor->size() < 4) {
		return;
	}

	dataClasses[classIndex].setColor(newColor);
}





// gets the name of the set at the pased index
// full implementation after file parsing
string* DataInterface::getSetName(int setIndex) {
	// check for invalid indexes
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return nullptr;
	}
	// get the set name
	return dataSets[setIndex].getName();
}

// sets the name of the set at the passed index to the passed string and returns the old name
// full implementation after file parsing
void DataInterface::setSetName(int setIndex, string  &newName) {
	// check for invalid indexes
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return;
	}
	// get the set name
	dataSets[setIndex].setName(newName);
}
/*
returns the index of the class containing the set at the passed index
returns -1 if the passed set index is out of bounds
*/
int DataInterface::getClassOfSet(int setIndex) const {
	// check if indexes are in bounds
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return -1;
	}
	return this->dataSets[setIndex].getClass();
}

// get the data set name for the class at the passed index
std::string *DataInterface::getSetOfClass(int classIndex, int setIndex) {
	if (setIndex >= this->getSetAmount() || setIndex < 0)
	{
		return nullptr;
	}

	if (classIndex >= this->getClassAmount() || classIndex < 0)
	{
		return nullptr;
	}
	std::string str = std::string("");
	if (this->dataSets[setIndex].getClass() == classIndex)
	{
		return this->dataSets[setIndex].getName();
	}
	else {
		return &str;
	}
}

// sets the index data class of the set at the passed index(setIndex)
int DataInterface::setSetClass(int setIndex, int newClassIndex) {
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return -1;
	}
	if (newClassIndex >= this->getClassAmount() || newClassIndex < 0) {
		return -1;
	}
	dataClasses[dataSets[setIndex].getClass()].removeSet(setIndex);
	dataClasses[newClassIndex].addSet(setIndex);
	return dataSets[setIndex].setDataClass(newClassIndex);
}

// gets the color the set should be painted
std::vector<double>* DataInterface::getSetColor(int setIndex) {
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return dataClasses[0].getColor();
	}
	if (setIndex == getSelectedSetIndex() && !hideSelectorLine) {
		return getSelectedSetColor();
	}
	std::vector<double>* color = dataClasses[dataSets[setIndex].getClass()].getColor();
	if (isVisible(setIndex) == false) {
		(*color)[3] = 0.0;
	}
	return color;
}


// checks whether the set is visible or not
bool DataInterface::isVisible(int setIndex) {
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return false;
	}
	if (setIndex == getSelectedSetIndex()) {
		return true;
	}
	return dataSets[setIndex].isVisible();
}

// sets the set visible or not visible and returns the old visibility
bool DataInterface::setVisible(int setIndex, bool newVisible) {
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return false;
	}
	return dataSets[setIndex].setVisible(newVisible);
}





// gets the amount the dimension is shifted by
double DataInterface::getDimensionShift(int dimensionIndex) {
	if (dimensionIndex >= this->getDimensionAmount() || dimensionIndex < 0) {
		return 0.0;
	}
	return (*dataDimensions[dimensionIndex]).getShift();


}

// changes the shift of the dimension at the passed int to the passsed double
void DataInterface::setDimensionShift(int dimensionIndex, double shiftMod) {
	if (dimensionIndex >= this->getDimensionAmount() || dimensionIndex < 0) {
		return;
	}
	double currentShift = (*dataDimensions[dimensionIndex]).getShift();
	return (*dataDimensions[dimensionIndex]).shiftDataBy(shiftMod - currentShift);
}



// gets the name of the x-axis
// full implementation after file parsing
std::string * DataInterface::getXAxisName() {
	return nullptr;
}

// gets the name of the y-axis
// full implementation after file parsing
string* DataInterface::getYAxisName() {
	return nullptr;
}

// gets the name of the maximum of the y axis
// full implementation after file parsing
string* DataInterface::getYMaxName() {
	return nullptr;
}

// gets the name of the minimum of the y axis
// full implementation after file parsing
string* DataInterface::getYMinName() {
	return nullptr;
}





// sorts the dimensions in ascending order by the data corresponding to the passed set index
void DataInterface::sortAscending(int setIndex) {
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return;
	}
	// construct list to sort
	std::list<Dimension*> totalList = std::list<Dimension*>();
	std::list<Dimension*>* ptrTotalList = &totalList;
	for (int i = 0; i < dataDimensions.size(); i++) {
		totalList.push_back(dataDimensions[i]);
	}
	ptrTotalList = mergeSortAscending(ptrTotalList, setIndex);

	// read sorted dimensions back into vector
	for (int i = 0; i < dataDimensions.size(); i++) {
		dataDimensions[i] = ptrTotalList->front();
		ptrTotalList->pop_front();
	}
	for (int i = 0; i < getClusterAmount(); i++) {
		clusters[i].calculateValues(&dataDimensions);
	}
}

// sorts the dimensions in descending order by the data corresponding to the passed set index(setIndex)
void DataInterface::sortDescending(int setIndex) {
	if (setIndex >= this->getSetAmount() || setIndex < 0) {
		return;
	}
	// construct list to sort
	std::list<Dimension*> totalList = std::list<Dimension*>();
	std::list<Dimension*>* ptrTotalList = &totalList;
	for (int i = 0; i < dataDimensions.size(); i++) {
		totalList.push_back(dataDimensions[i]);
	}
	ptrTotalList = mergeSortDescending(ptrTotalList, setIndex);

	// read sorted dimensions back into vector
	for (int i = 0; i < dataDimensions.size(); i++) {
		dataDimensions[i] = ptrTotalList->front();
		ptrTotalList->pop_front();
	}
	for (int i = 0; i < getClusterAmount(); i++) {
		clusters[i].calculateValues(&dataDimensions);
	}
}

// places the dimensions back in the original order
void DataInterface::sortOriginal() {
	// construct list to sort
	std::list<Dimension*> totalList = std::list<Dimension*>();
	std::list<Dimension*>* ptrTotalList = &totalList;
	for (int i = 0; i < dataDimensions.size(); i++) {
		totalList.push_back(dataDimensions[i]);
	}
	ptrTotalList = mergeSortOriginal(ptrTotalList);

	// read sorted dimensions back into vector
	for (int i = 0; i < dataDimensions.size(); i++) {
		dataDimensions[i] = ptrTotalList->front();
		ptrTotalList->pop_front();
	}
	for (int i = 0; i < getClusterAmount(); i++) {
		clusters[i].calculateValues(&dataDimensions);
	}
}





// shifts all the data of the passed set to the passed value
void DataInterface::level(int setIndex, double levelValue) {
	// don't accept out of bounds data set indexes
	if (setIndex >= getSetAmount() || setIndex < 0) {
		return;
	}
	zeroShifts();
	// adjust each data point to the new 0 amount for its dimension
	for (unsigned int i = 0; i < getDimensionAmount(); i++) {
		double currentData = (*dataDimensions[i]).getCalibratedData(setIndex);
		double change = levelValue - currentData;
		// change data
		(*dataDimensions[i]).shiftDataBy(change);
	}
}

// gets the mean data value of the set at the passed index
double DataInterface::getMean(int setIndex) const {
	// don't accept out of bounds data set indexes
	if (setIndex >= getSetAmount() || setIndex < 0) {
		return 0.0;
	}
	double sum = 0.0;
	// adjust each data point to the new 0 amount for its dimension
	for (unsigned int i = 0; i < getDimensionAmount(); i++) {
		double currentData = (*dataDimensions[i]).getCalibratedData(setIndex);
		sum += currentData;
	}
	return sum /= ((double)getDimensionAmount());
}

// gets the median data value of the set at the passed index
double DataInterface::getMedian(int setIndex) const {
	// don't accept out of bounds data set indexes
	if (setIndex >= getSetAmount() || setIndex < 0) {
		return 0.0;
	}
	if (getDimensionAmount() == 0) {
		return 0.0;
	}
	// make a vector to hold the set data
	vector<double> setData = vector<double>();
	// adjust each data point to the new 0 amount for its dimension
	for (unsigned int i = 0; i < getDimensionAmount(); i++) {
		double currentData = (*dataDimensions[i]).getCalibratedData(setIndex);
		setData.push_back(currentData);
	}
	stable_sort(setData.begin(), setData.end());
	// acount for an even number of dimensions
	if (setData.size() % 2 == 0) {
		double first = setData[setData.size() / 2];
		double second = setData[setData.size() / 2 - 1];
		return (first + second) / 2.0;
	}
	return setData[setData.size() / 2];

}

// calibrate each dimension to the [0,1] space
void DataInterface::calibrateData() {
	for (unsigned int i = 0; i < getDimensionAmount(); i++) {
		(*dataDimensions[i]).calibrateData();
	}
}

// zero shifts in the dimensions
void DataInterface::zeroShifts() {
	for (unsigned int i = 0; i < getDimensionAmount(); i++) {
		double shift = (*dataDimensions[i]).getShift();
		(*dataDimensions[i]).shiftDataBy(-shift);
	}
}




// gets the title of the note at the passed index(noteIndex)
std::string * DataInterface::getNoteTitle(int noteIndex) {
	if (noteIndex >= getNoteAmount() || noteIndex < 0) {
		return nullptr;
	}
	return notes[noteIndex].getTitle();
}

// gets the message of the note at the passed index(noteIndex)
std::string * DataInterface::getNoteMessage(int noteIndex) {
	if (noteIndex >= getNoteAmount() || noteIndex < 0) {
		return nullptr;
	}
	return notes[noteIndex].getContent();
}

// gets the x coordinate of the note at the passed index(noteIndex)
double DataInterface::getNoteX(int noteIndex) {
	if (noteIndex >= getNoteAmount() || noteIndex < 0) {
		return 0.0;
	}
	return notes[noteIndex].getXCord();
}

// gets the y coordinate of the note at the passed index(noteIndex)
double DataInterface::getNoteY(int noteIndex) {
	if (noteIndex >= getNoteAmount() || noteIndex < 0) {
		return 0.0;
	}
	return notes[noteIndex].getYCord();
}

// gets the number of notes
int DataInterface::getNoteAmount()
{
	return notes.size();
}

// deletes of the note at the passed index(noteIndex)
void DataInterface::deleteNote(int noteIndex)
{
	if (noteIndex >= getNoteAmount() || noteIndex < 0) {
		return;
	}
	notes.erase(notes.begin() + noteIndex);
}




// compares the data of a each set to the set at the passed index and checks if the data is within the radius of the data of the passed set
// if the passed bool(meanMedian) is true the mean will be used and otherwise the median will be
bool DataInterface::hypercube(int setIndex, double radius) {


	// don't accept out of bounds data set indexes
	if (setIndex >= getSetAmount() || setIndex < 0) {
		return false;
	}
	if (radius < 0) {
		return false;
	}
	for (int i = 0; i < dataDimensions[0]->size(); i++) {
		int clusterSets = 0;
	}

	std::string clusterName = *(dataSets[setIndex].getName()) + " " + std::to_string(radius);
	paintClusters = true;

	for (int i = 0; i < getClusterAmount(); i++)
	{
		if (getClusters().at(i).getName()->compare(clusterName) == 0)
		{
			return false;
		}
	}

	std::vector<int> selectedSets = std::vector<int>();

	for (int i = 0; i < getSetAmount(); i++)
	{
		bool withinCube = true;
		for (int j = 0; j < getDimensionAmount(); j++)
		{
			if (!(getData(i, j) >= getData(setIndex, j) - radius && getData(i, j) <= getData(setIndex, j) + radius))
			{
				withinCube = false;
			}
		}
		if (withinCube)
		{
			selectedSets.push_back(i);
		}
	}

	ColorCustom clusterColor = ColorCustom();
	int originalSetClass = getClassOfSet(setIndex);
	std::vector<double>* colorConponents = getClassColor(originalSetClass);
	clusterColor.setRed((*colorConponents)[0]);
	clusterColor.setGreen((*colorConponents)[1]);
	clusterColor.setBlue((*colorConponents)[2]);
	clusterColor.setAlpha((*colorConponents)[3]);
	clusters.push_back(SetCluster(clusterColor, &selectedSets/*, &dataDimensions*/));
	clusters[clusters.size() - 1].setRadius(radius);
	clusters[clusters.size() - 1].setOriginalSet(setIndex);
	clusters[clusters.size() - 1].setName(&clusterName);
	if (useMean) {
		clusters[clusters.size() - 1].setUseMean(useMean);
		clusters[clusters.size() - 1].calculateValues(&dataDimensions);
	}

	this->selectedSetIndex = 0;
	this->selectedClusterIndex = clusters.size() - 1;

	return true;
}

// compares the data of a each set to the set at the passed index and checks if the data is within the radius of the data of the passed set
// if the passed bool(meanMedian) is true the mean will be used and otherwise the median will be
bool DataInterface::subHypercube(int setIndex, int clusterIndex, double radius) {


	// don't accept out of bounds data set indexes
	if (setIndex >= getSetAmount() || setIndex < 0) {
		return false;
	}
	if (radius < 0) {
		return false;
	}

	std::string clusterName = "- " + *(dataSets[setIndex].getName()) + " " + std::to_string(radius);
	paintClusters = true;

	for (int i = 0; i < getClusterAmount(); i++)
	{
		if (getClusters().at(i).getName()->compare(clusterName) == 0)
		{
			return false;
		}
	}

	std::vector<int> selectedSets = std::vector<int>();

	for (int i = 0; i < clusters[clusterIndex].getSets()->size(); i++)
	{
		bool withinCube = true;
		for (int j = 0; j < getDimensionAmount(); j++)
		{
			if (!(getData(clusters[clusterIndex].getSets()->at(i), j) >= getData(setIndex, j) - radius && getData(clusters[clusterIndex].getSets()->at(i), j) <= getData(setIndex, j) + radius))
			{
				withinCube = false;
			}
		}
		if (withinCube)
		{
			selectedSets.push_back(clusters[clusterIndex].getSets()->at(i));
		}
	}

	ColorCustom clusterColor = ColorCustom();
	int originalSetClass = getClassOfSet(setIndex);
	std::vector<double>* colorConponents = getClassColor(originalSetClass);
	clusterColor.setRed((*colorConponents)[0]);
	clusterColor.setGreen((*colorConponents)[1]);
	clusterColor.setBlue((*colorConponents)[2]);
	clusterColor.setAlpha((*colorConponents)[3]);
	clusters.push_back(SetCluster(clusterColor, &selectedSets/*, &dataDimensions*/));
	clusters[clusters.size() - 1].setRadius(radius);
	clusters[clusters.size() - 1].setOriginalSet(setIndex);
	clusters[clusters.size() - 1].setName(&clusterName);
	if (useMean) {
		clusters[clusters.size() - 1].setUseMean(useMean);
		clusters[clusters.size() - 1].calculateValues(&dataDimensions);
	}


	return true;
}

// returns whether the clusters will use mean or median
bool DataInterface::isUseMeanForClusters() {
	return useMean;
}

// returns whether the clusters will use mean or median
bool DataInterface::getUseMeanForClusters() {
	return useMean;
}

// returns whether the clusters will use mean or median
void DataInterface::setUseMeanForClusters(bool newUseMean) {
	useMean = newUseMean;
	for (int i = 0; i < getClusterAmount(); i++) {
		clusters[i].setUseMean(newUseMean);
	}

}

double DataInterface::getRadius() {
	return radius;
}

void DataInterface::setRadius(double newRadius) {
	radius = newRadius;
}


void DataInterface::incrementSelectedClusterClassIndex() {
	int newSelectedIndex = selectedClusterClassIndex + 1;

	for (int i = 0; i < clusters[selectedClusterIndex].getSets()->size(); i++)
	{
		if (getClassOfSet(clusters[selectedClusterIndex].getSets()->at(i)) != selectedClusterClassIndex)
		{
			newSelectedIndex = getClassOfSet(clusters[selectedClusterIndex].getSets()->at(i));
			break;
		}
	}

	if (newSelectedIndex >= getClassAmount()) {
		newSelectedIndex = 0;
	}

	selectedClusterClassIndex = newSelectedIndex;
}

// sets the index of the selected set to the passed index(newSelectedSetIndex)
void DataInterface::incrementSelectedSetIndex() {
	int newSelectedSetIndex = selectedSetIndex;
	int numberOfSetsChecked = 0;
	// ensure class has sets
	newSelectedSetIndex++;
	// loop
	if (newSelectedSetIndex >= dataClasses[selectedClassIndex].getSetNumber()) {
		newSelectedSetIndex = 0;
	}
	if(dataClasses[selectedClassIndex].getSetNumber()==0) {
		newSelectedSetIndex=0;
	}
	selectedSetIndex = newSelectedSetIndex;
}

void DataInterface::incrementSelectedSetIndexCluster() {
	int newSelectedSetIndex = selectedSetIndex + 1;

	if (newSelectedSetIndex >= clusters[selectedClusterIndex].getSetNumber()) {
		newSelectedSetIndex = 0;
	}

	selectedSetIndex = newSelectedSetIndex;
}

// sets the index of the selected set to the passed index(newSelectedSetIndex)
void DataInterface::decrementSelectedSetIndex() {
	int newSelectedSetIndex = selectedSetIndex;
	int numberOfSetsChecked = 0;
	// ensure class has sets
	newSelectedSetIndex--;
	// loop
	if (newSelectedSetIndex < 0) {
		newSelectedSetIndex = dataClasses[selectedClassIndex].getSetNumber() - 1;
	}
	if(dataClasses[selectedClassIndex].getSetNumber()==0) {
		newSelectedSetIndex=0;
	}
	selectedSetIndex = newSelectedSetIndex;
}

void DataInterface::decrementSelectedSetIndexCluster() {
	int newSelectedSetIndex = selectedSetIndex - 1;

	if (newSelectedSetIndex < 0) {
		newSelectedSetIndex = clusters[selectedClusterIndex].getSetNumber() - 1;
	}

	selectedSetIndex = newSelectedSetIndex;
}

// gets the index of the selected set
int DataInterface::getSelectedSetIndex() {
	if (this->paintClusters)
	{
		if (clusters.size() <= 0 || clusters[selectedClusterIndex].getSets() <= 0) {
			return 0;
		}

		return (clusters[selectedClusterIndex].getSets())->at(selectedSetIndex);
	}

	if (dataClasses[selectedClassIndex].getSetNumber() <= 0) {
		return 0;
	}
	return (dataClasses[selectedClassIndex].getSetsInClass())->at(selectedSetIndex);
}

void DataInterface::setSelectedSetIndex(int index) {
	this->selectedSetIndex = index;
}

// increases the index of the selected class to the passed index(newSelectedClassIndex)
void DataInterface::incrementSelectedClassIndex() {
	int newSelectedClassIndex = selectedClassIndex+1;
	if (newSelectedClassIndex >= getClassAmount()) {
		newSelectedClassIndex = 0;
	}
	int numberOfClassesChecked = 0;
	// ensure class has sets
	while (dataClasses[newSelectedClassIndex].getSetNumber() == 0 && numberOfClassesChecked <= getClassAmount()) {
		newSelectedClassIndex++;
		// loop
		if (newSelectedClassIndex >= getClassAmount()) {
			newSelectedClassIndex = 0;
		}
		numberOfClassesChecked++;
	}
	selectedClassIndex = newSelectedClassIndex;
	selectedSetIndex = 0;
}

void DataInterface::incrementSelectedClusterIndex() {
	int newSelectedClusterIndex = selectedClusterIndex + 1;

	if (newSelectedClusterIndex >= clusters.size()) {
		newSelectedClusterIndex = 0;
	}

	selectedClusterIndex = newSelectedClusterIndex;
	selectedSetIndex = 0;
}

// decreases the index of the selected class to the passed index(newSelectedClassIndex)
void DataInterface::decrementSelectedClassIndex() {
	int newSelectedClassIndex = selectedClassIndex-1;
	if (newSelectedClassIndex < 0) {
		newSelectedClassIndex = getClassAmount() - 1;
	}
	int numberOfClassesChecked = 0;
	// ensure class has sets
	while (dataClasses[newSelectedClassIndex].getSetNumber() == 0 && numberOfClassesChecked <= getClassAmount()) {
		newSelectedClassIndex--;
		// loop
		if (newSelectedClassIndex < 0) {
			newSelectedClassIndex = getClassAmount() - 1;
		}
		numberOfClassesChecked++;
	}
	selectedClassIndex = newSelectedClassIndex;
	selectedSetIndex = 0;
}

void DataInterface::decrementSelectedClusterIndex() {
	int newSelectedClusterIndex = selectedClusterIndex - 1;

	if (newSelectedClusterIndex < 0) {
		newSelectedClusterIndex = clusters.size() - 1;
	}
	selectedClusterIndex = newSelectedClusterIndex;
	selectedSetIndex = 0;
}

// gets the index of the selected class
int DataInterface::getSelectedClassIndex() const {
	return selectedClassIndex;
}


int DataInterface::getSelectedClusterIndex() const {
	return selectedClusterIndex;
}

void DataInterface::setSelectedClusterIndex(int newIndex) {
	this->selectedClusterIndex = newIndex;
}

int DataInterface::getSelectedClusterClassIndex() const {
	return selectedClusterClassIndex;
}

// sets the selected set's color to the colors in the passed double array(newSelectedSetColor)
void DataInterface::setSelectedSetColor(std::vector<double> &newSelectedSetColor) {
	if (newSelectedSetColor.size() < 44) {
		return;
	}
	selectedSetColor.setRed(newSelectedSetColor[0]);
	selectedSetColor.setGreen(newSelectedSetColor[1]);
	selectedSetColor.setBlue(newSelectedSetColor[2]);
	selectedSetColor.setAlpha(newSelectedSetColor[3]);
}

// gets the color of the selected set
std::vector<double>* DataInterface::getSelectedSetColor() {
	return selectedSetColor.getColorComponents();
}




// whether to paint the clusters or not
bool DataInterface::isPaintClusters() const {
	return paintClusters;
}

// whether to paint the clusters using class colors or hypercube colors
bool DataInterface::isPaintClassColors() const {
	return paintClassColors;
}

// toggles whether to paint the clusters or not
bool DataInterface::togglePaintClusters() {
	paintClusters = !paintClusters;
	return paintClusters;
}

// toggles whether to paint the clusters using class colors or hypercube colors
bool DataInterface::togglePaintClassColors() {
	paintClassColors = !paintClassColors;
	return paintClassColors;
}

// gets amount of clusters
int DataInterface::getClusterAmount() {
	return clusters.size();
}

// get set clusters
std::vector<SetCluster> DataInterface::getClusters() {
	return clusters;
}

// get displayed of cluster at index
bool DataInterface::getDisplayed(int index) {
	return clusters[index].isDisplayed();
}

// the minimum value for the cluster data
double DataInterface::getClusterMinimum(int clusterIndex, int dimensionIndex) const {
	if (dimensionIndex >= getDimensionAmount() || dimensionIndex < 0) {
		return 0.0;
	}
	return clusters[clusterIndex].getMinimum(dimensionIndex) + dataDimensions[dimensionIndex]->getShift();
}

// the mean value for the cluster data
double DataInterface::getClusterMiddle(int clusterIndex, int dimensionIndex) const {
	if (dimensionIndex >= getDimensionAmount() || dimensionIndex < 0) {
		return 0.0;
	}
	return clusters[clusterIndex].getMiddle(dimensionIndex) + dataDimensions[dimensionIndex]->getShift();
}

// the maximum value for the cluster data
double DataInterface::getClusterMaximum(int clusterIndex, int dimensionIndex) const {
	if (dimensionIndex >= getDimensionAmount() || dimensionIndex < 0) {
		return 0.0;
	}
	return clusters[clusterIndex].getMaximum(dimensionIndex) + dataDimensions[dimensionIndex]->getShift();
}

// gets the color of the cluster
std::vector<double>* DataInterface::getClusterColor(int clusterIndex) {
	if (clusterIndex <0 || clusterIndex>getClusterAmount()) {
		return nullptr;
	}
	return clusters[clusterIndex].getColor();
}

// sets the color of the cluster at the passed index
void DataInterface::setClusterColor(int clusterIndex, std::vector<double>* newColor) {
	if (clusterIndex < 0 || clusterIndex >= getClusterAmount()) {
		return;
	}
	if (newColor->size() < 4) {
		return;
	}
	clusters[clusterIndex].setColor(newColor);
}

// delete a class from the list of classes
void DataInterface::deleteCluster(int clusterIndex) {

	selectedClusterIndex = 0;
	selectedSetIndex = 0;

	// do not allow deletion of default class
	if (clusterIndex < 0 || clusterIndex >= getClusterAmount()) {
		return;
	}
	// delete the class
	clusters.erase(clusters.begin() + clusterIndex);
}

// gets the name of the cluster
std::string * DataInterface::getClusterName(int clusterIndex) {
	if (clusterIndex < 0 || clusterIndex >= getClusterAmount()) {
		return nullptr;
	}
	return clusters[clusterIndex].getName();
}

// sets the name of the cluster
void DataInterface::setClusterName(int clusterIndex, std::string* newName) {
	if (clusterIndex < 0 || clusterIndex >= getClusterAmount()) {
		return;
	}
	return clusters[clusterIndex].setName(newName);
}

// sets the color of nominal sets
void DataInterface::setNominalColorChoice(int i)
{
	nominalColorChoice = i;
}

// gets nominal color
int DataInterface::getNominalColor()
{
	return nominalColorChoice;
}

// gets a list of the sets in the class
std::vector<int>* DataInterface::getClusterSets(int clusterIndex) {
	if (clusterIndex < 0 || clusterIndex >= getClusterAmount()) {
		return nullptr;
	}
	return clusters[clusterIndex].getSets();
}





// sets the color for the background
void DataInterface::setBackgroundColor(std::vector<double>* newColor) {
	if (newColor->size() < 4) {
		return;
	}
	backgroundColor.setRed(newColor->at(0));
	backgroundColor.setGreen(newColor->at(1));
	backgroundColor.setBlue(newColor->at(2));
	backgroundColor.setAlpha(newColor->at(3));
}

// gets the color for the background
std::vector<double>* DataInterface::getBackgroundColor() {
	return backgroundColor.getColorComponents();
}






// private:
// a method to hold the basic intialization of the object fields
void DataInterface::init() {

	dataDimensions = std::vector<Dimension*>();
	// create and set the fields
	dataClasses = std::vector<DataClass>();
	dataClasses.push_back(DataClass(0, "Default"));
	std::vector<double> newColor = std::vector<double>();
	newColor.push_back(0.0);
	newColor.push_back(0.0);
	newColor.push_back(1.0);
	newColor.push_back(1.0);
	dataClasses[0].setColor(newColor);
	dataSets = std::vector<DataSet>();
	clusters = std::vector<SetCluster>();
	pureCubes = std::vector<SetCluster>();

	selectedSetColor = ColorCustom();
	selectedSetColor.setRed(0.0);
	selectedSetColor.setGreen(0.0);
	selectedSetColor.setBlue(0.0);
	selectedSetColor.setAlpha(0.85);


	selectedSetIndex = 0;
	selectedClassIndex = 0;
	selectedClusterIndex = 0;
	selectedClusterClassIndex = 0;

	totalCubeCount = 0;
	overlappingCubeCount = 0;
	combinationMode = 0;
	thresholdMode = false;
	willCombine = false;
	hypercubeThreshold = 100;

	histogramMode = false;
	frequencyMode = false;
	quadMode = false;
	overlapMode = false;
	nominalSetsMode = false;

	hideSelectorLine = false;
	isDrawBorders = true;
	drawMin = true;
	drawCenter = true;
	drawMax = true;

	yMaxName = "1.0";
	yMinName = "0.0";
	xAxisName = "X-Axis";
	yAxisName = "Y-Axis";
	paintClusters = false;
	paintClassColors = true;
	useMean = false;
}

// a method to hold the setup of fields to be performed after everything else
void DataInterface::finalInit() {

}

std::vector<std::vector<std::string>*>* DataInterface::readFileIntoVector(std::string* fileName) {
	std::ifstream file(*fileName);
	std::string str;
	std::stringstream ss;
	std::vector<std::vector<std::string>*>* fileLines = new std::vector<std::vector<std::string>*>();
	// read data
	while (std::getline(file, str))
	{
		int tokenStart = 0;
		int tokenEnd = 0;
		// make new data set
		std::vector<std::string>* newTokens = tokenizeString(&str);
		fileLines->push_back(newTokens);
	}
	ss.clear();
	file.close();
	// validate file
	// check that file had at least one none empty line
	if (fileLines->size() <= 0) {
		return nullptr;
	}

	return fileLines;
}

// takes the passed string and tokenizes it by commas
std::vector<std::string>* DataInterface::tokenizeString(std::string * stringToTokenize) {
	std::vector<std::string>* stringTokens = new std::vector<std::string>();
	if (stringToTokenize->size() <= 0) {
		return stringTokens;
	}
	int startToken = 0;
	int endToken = stringToTokenize->find_first_of(',', startToken);
	int lengthToken = endToken - startToken;
	if (endToken == std::string::npos) {
		endToken = stringToTokenize->size();
	}
	// parse string
	while (startToken < stringToTokenize->size()) {
		// parse token
		std::string currentToken = (stringToTokenize->substr(startToken, lengthToken));
		// add the new token to the next
		stringTokens->push_back(currentToken);
		// find next token
		startToken = endToken + 1;
		endToken = stringToTokenize->find_first_of(',', startToken);
		if (endToken == std::string::npos) {
			endToken = stringToTokenize->size();
		}
		lengthToken = endToken - startToken;
	}
	return stringTokens;
}

// removes duplicate strings from the passed vector
void DataInterface::removeDuplicates(std::vector<std::string>* stringList) {
	std::vector<std::string> uniqueList = std::vector<std::string>();
	for (int i = 0; i < stringList->size(); i++) {
		std::string newClassName = (*stringList)[i];
		// check if the class already exists
		bool isNewClass = true;
		for (int j = 0; j < uniqueList.size(); j++) {
			if (uniqueList[j] == newClassName) {
				isNewClass = false;
				j = uniqueList.size();
			}
		}
		if (isNewClass) {
			uniqueList.push_back(newClassName);
		}
	}
	stringList->clear();

	for (int i = 0; i < uniqueList.size(); i++) {
		stringList->push_back(uniqueList[i]);
	}
}

// reads the contents of the file, at fileName, into a vector
bool DataInterface::readBasicFile(std::vector<std::vector<std::string>*>* fileContents) {

	try {
		// get number of sets
		int setNumber = fileContents->size();
		int startRow = 0;
		int startColumn = 0;
		int endColumn = (*fileContents)[0]->size();
		int off = 0;
		int dimOff;

		// create data classes
		// create default class
		dataClasses.clear();
		dataClasses.push_back(DataClass(0, "Default"));
		std::vector<double> newColor = std::vector<double>();
		newColor.push_back(0.0);
		newColor.push_back(0.0);
		newColor.push_back(1.0);
		newColor.push_back(1.0);
		dataClasses[0].setColor(newColor);

		if (readSetNamesBasic)
		{
			off = 1;
			startColumn = 1; // actual data starts here
		}

		std::map<std::string, int> classIdx;
		if (readClassNamesBasic)
		{
			endColumn--; // actual data ends here

			std::set<std::string> classes;
			for (int i = startRow; i < fileContents->size(); i++)
			{
				classes.insert((*fileContents)[i]->back());
			}

			for (auto x : classes)
			{
				addClass();
				dataClasses[getClassAmount() - 1].setName(&x);
				classIdx[x] = getClassAmount() - 1;
				cout << x << endl;
			}
		}

		if (readDimensionNamesBasic) 
		{
			dimOff = 1;
			setNumber--; // first row is not data
			startRow = 1; // data starts here
		}

		
		// create dimensions
		vector<map<int, std::string>> dimensionsToClean;
		aboveOne.clear();
		for (int i = startColumn; i < endColumn; i++)
		{

			dataDimensions.push_back(new Dimension(i - off, setNumber));
			dataDimensions[i - off]->setName(&(*(*fileContents)[0])[i]);
			dimensionsToClean.push_back(map<int, std::string>());

			// populate dimensions with data
			for (int j = startRow; j < fileContents->size(); j++)
			{
				double newData;
				std::string content = (*(*fileContents)[j])[i];
				if (content.empty() || content[0] < 48 || content[0] > 57) // doesn't start with a number
				{
					if (content.empty())
					{
						content = "Empty";
					}
					
					dimensionsToClean[dimensionsToClean.size() - 1][j - off] = content;
					
					if (aboveOne.find(content) == aboveOne.end())
					{
						aboveOne[content] = 0.0 - ((aboveOne.size() + 1.0) * 0.1);
					}
					newData = 0;
				}
				else
				{
					newData = std::stod(content);
				}

				dataDimensions[i - off]->setData(j - off, newData);
			}

			dataDimensions[i - off]->calibrateData();

			for (int j = 0; j < dimensionsToClean.size(); j++)
			{
				for (auto entry : dimensionsToClean[j])
				{
					dataDimensions[j]->setData(entry.first, aboveOne[entry.second]);
				}
			}
		}

		// Create classes 
		for (int i = startRow; i < fileContents->size(); i++)
		{
			std::string idx = std::string((*fileContents)[i]->back());
			std::string newSetName;
			int val = i - dimOff;
			if (readSetNamesBasic)
			{
				newSetName = (*(*fileContents)[i])[0];
			}
			else
			{
				newSetName = std::to_string(val);
			}

			dataSets.push_back(DataSet(val, classIdx[idx]));
			dataSets[val].setName(newSetName);
			dataClasses[classIdx[idx]].addSet(val);
		}

		autoColor();
		createOverlaps();
	}
	catch (...) {
		return false;
	}

	return true;
}

vector<DataClass> DataInterface::getClasses() {
	return this->dataClasses;
}

void DataInterface::autoColor() {
	double red = 1;
	double green = 1;
	double blue = 0;
	double step = 1.0 / ((dataClasses.size() - 1) / 3);
	std::vector<vector<double>>startingColors = std::vector<vector<double>>();
	for (int i = 0; i < dataClasses.size() - 1; i++)
	{
		startingColors.push_back(std::vector<double>());
		startingColors[i].push_back(red);
		startingColors[i].push_back(green);
		startingColors[i].push_back(blue);
		startingColors[i].push_back(1.0);

		if ((dataClasses.size() - 1) < 3)
		{
			red = 1.0;
			green = 0.0;
			blue = 1.0;
		}
		else if (i < ((dataClasses.size() - 1) / 3)) // reds
		{
			if (green >= 0.4)
			{
				green -= step;
				if (green <= 0.4)
				{
					blue = 0.6;
				}
			}
			else
			{
				blue += step;
			}

			if (i == ((dataClasses.size() - 1) / 3) - 1)
			{
				red = 1.0;
				green = 0.0;
				blue = 1.0;
			}
		}
		else if (i < (((dataClasses.size() - 1) / 3) * 2)) // blues
		{
			if (red >= 0.4)
			{
				red -= step;
				if (red <= 0.4)
				{
					green = 0.6;
				}
			}
			else
			{
				green += step;
			}

			if (i == (((dataClasses.size() - 1) / 3) * 2) - 1)
			{
				red = 0.0;
				green = 1.0;
				blue = 1.0;
			}
		}
		else if (i < (dataClasses.size() - 1)) // greens
		{
			if (blue >= 0.4)
			{
				blue -= step;
				if (blue <= 0.4)
				{
					red = 0.6;
				}
			}
			else
			{
				red += step;
			}
		}
	}

	for (int i = 1; i < dataClasses.size(); i++)
	{
		dataClasses[i].setColor(startingColors[i - 1]);
	}
}

void DataInterface::autoCluster() {
	std::vector<double> classMax = std::vector<double>();
	std::vector<double> classMin = std::vector<double>();
	std::vector<double> classMedian = std::vector<double>();
	double overallMax;
	double overallMin;
	std::vector<int> * instancesInClass;
	std::vector<int> selectedInstances = std::vector<int>();
	paintClusters = true;

	for (int i = 1; i < dataClasses.size(); i++)
	{
		// reset data for each class
		classMax.clear();
		classMin.clear();
		for (int j = 0; j < getDimensionAmount(); j++)
		{
			classMax.push_back(DBL_MIN);
			classMin.push_back(DBL_MAX);
		}
		overallMax = DBL_MIN;
		overallMin = DBL_MAX;

		instancesInClass = dataClasses[i].getSetsInClass();

		// search class for max and min
		for (int j = 0; j < instancesInClass->size(); j++)
		{
			for (int k = 0; k < getDimensionAmount(); k++)
			{

				double currentData = getData(instancesInClass->at(j), k);
				overallMax = max(overallMax, currentData);
				overallMin = min(overallMin, currentData);
				classMax.at(k) = max(classMax.at(k), currentData);
				classMin.at(k) = min(classMin.at(k), currentData);
			}
		}

		double cubeThreshold = (overallMax - overallMin) / 2;

		classMedian.clear();

		// find virtualized median line
		for (int j = 0; j < getDimensionAmount(); j++)
		{
			classMedian.push_back( classMin.at(j) + ((classMax.at(j) - classMin.at(j)) / 2) );
		}

		selectedInstances.clear();

		// define hypercube
		for (int j = 0; j < getSetAmount(); j++)
		{
			bool withinCube = true;
			for (int k = 0; k < getDimensionAmount(); k++)
			{
				if (!(getData(j, k) >= classMin.at(k)/*(classMedian.at(k) - cubeThreshold)*/ && getData(j, k) <= classMax.at(k)/*(classMedian.at(k) + cubeThreshold)*/))
				{
					withinCube = false;
				}
			}
			if (withinCube)
			{
				selectedInstances.push_back(j);
			}
		}

		ColorCustom clusterColor = ColorCustom();
		std::vector<double>* colorConponents = dataClasses[i].getColor();
		clusterColor.setRed((*colorConponents)[0]);
		clusterColor.setGreen((*colorConponents)[1]);
		clusterColor.setBlue((*colorConponents)[2]);
		clusterColor.setAlpha((*colorConponents)[3]);
		clusters.push_back(SetCluster(clusterColor, &selectedInstances/*, &dataDimensions*/));
		clusters[clusters.size() - 1].setRadius(cubeThreshold);
		clusters[clusters.size() - 1].setName(dataClasses[i].getName());
	}
}

void DataInterface::highlightOverlap(double threshold)
{
	clusters.clear();
	pureCubes.clear();
	selectedSetIndex = 0;
	totalCubeCount = 0;
	overlappingCubeCount = 0;
	double accThreshold = 0.90;
	selectedSetIndex = 0;
	vector<int> selectedInstances = vector<int>();
	vector<SetCluster> blocks = vector<SetCluster>();

	for (int i = 0; i < getSetAmount(); i++) // selected instances
	{
		selectedInstances.clear();
		selectedInstances.push_back(i);

		ColorCustom clusterColor = ColorCustom();
		std::vector<double>* colorConponents = dataClasses[getClassOfSet(i)].getColor();
		clusterColor.setRed((*colorConponents)[0]);
		clusterColor.setGreen((*colorConponents)[1]);
		clusterColor.setBlue((*colorConponents)[2]);
		clusterColor.setAlpha((*colorConponents)[3]);
		blocks.push_back(SetCluster(clusterColor, &selectedInstances, &dataDimensions));
		blocks[blocks.size() - 1].setRadius(0.0);
		string name = to_string(i);
		blocks[blocks.size() - 1].setName(&name);
		blocks[blocks.size() - 1].setOriginalSet(i);

	}

	bool actionTaken = false;
	set<int> toBeDeleted = set<int>();
	int count = blocks.size();
	do
	{
		if (count <= 0)
		{
			count = blocks.size();
		}

		toBeDeleted.clear();
		actionTaken = false;

		if (blocks.size() <= 0)
			break;

		SetCluster temp = blocks.front();

		blocks.erase(blocks.begin());

		int tempClass = getClassOfSet(temp.getSets()->at(0));

		for (int i = 0; i < blocks.size(); i++)
		{
			int currCubeClass = getClassOfSet(blocks[i].getSets()->at(0));
			if (tempClass != currCubeClass)
			{
				continue;
			}

			vector<double> maxPoint = vector<double>();
			vector<double> minPoint = vector<double>();

			// define combined space
			for (int j = 0; j < getDimensionAmount(); j++)
			{
				double newLocalMax = max(temp.getMaximum(j), blocks[i].getMaximum(j));
				double newLocalMin = min(temp.getMinimum(j), blocks[i].getMinimum(j));

				maxPoint.push_back(newLocalMax);
				minPoint.push_back(newLocalMin);
			}

			vector<int> pointsInSpace = vector<int>();

			// define all points in combined space
			for (int j = 0; j < getSetAmount(); j++)
			{
				bool withinSpace = true;
				for (int k = 0; k < getDimensionAmount(); k++)
				{
					double currData = getData(j, k);
					if (!(currData <= maxPoint.at(k) && currData >= minPoint.at(k)))
					{
						withinSpace = false;
						break;
					}
				}
				if (withinSpace)
				{
					pointsInSpace.push_back(j);
				}
			}

			set<int> classCount = set<int>();

			// check if new space is pure
			for (int j = 0; j < pointsInSpace.size(); j++)
			{
				int currClass = getClassOfSet(pointsInSpace.at(j));

				if (classCount.find(currClass) != classCount.end())
				{
					continue;
				}

				classCount.insert(currClass);
			}

			// if pure then combine the two cubes
			if (classCount.size() <= 1)
			{
				actionTaken = true;

				temp.getSets()->clear();
				for (int j = 0; j < pointsInSpace.size(); j++)
				{
					temp.addSet(pointsInSpace.at(j));
				}
				temp.calculateValues(&dataDimensions);

				// store this index, to delete the cube that was combined
				toBeDeleted.insert(i);
			}
		}

		int offset = 0;
		for (auto x : toBeDeleted)
		{
			blocks.erase(blocks.begin() + (x - offset));
			offset++;
		}

		blocks.push_back(temp);

		count--;

	} while (actionTaken || count > 0);

	// impure

	actionTaken = false;
	toBeDeleted = set<int>();
	count = blocks.size();
	do
	{
		if (count <= 0)
		{
			count = blocks.size();
		}

		toBeDeleted.clear();
		actionTaken = false;

		if (blocks.size() <= 0)
			break;

		SetCluster temp = blocks.front();

		blocks.erase(blocks.begin());

		vector<double> acc = vector<double>();
		for (int i = 0; i < blocks.size(); i++)
		{
			// get majority class
			int majorityClass = 0;
			map<int, int> classCount = map<int, int>();
			for (int j = 0; j < blocks[i].getSetNumber(); j++)
			{
				int currClass = getClassOfSet(blocks[i].getSets()->at(j));
				if (classCount.find(currClass) != classCount.end())
				{
					classCount[currClass] += 1;
				}
				else
				{
					classCount[currClass] = 1;
				}
			}

			int majorityCount = INT_MIN;
			for (auto entry : classCount)
			{
				if (entry.second > majorityCount)
				{
					majorityCount = entry.second;
					majorityClass = entry.first;
				}
			}

			int currCubeClass = getClassOfSet(blocks[i].getSets()->at(0));

			vector<double> maxPoint = vector<double>();
			vector<double> minPoint = vector<double>();

			// define combined space
			for (int j = 0; j < getDimensionAmount(); j++)
			{
				double newLocalMax = max(temp.getMaximum(j), blocks[i].getMaximum(j));
				double newLocalMin = min(temp.getMinimum(j), blocks[i].getMinimum(j));

				maxPoint.push_back(newLocalMax);
				minPoint.push_back(newLocalMin);
			}

			vector<int> pointsInSpace = vector<int>();

			// define all points in combined space
			for (int j = 0; j < getSetAmount(); j++)
			{
				bool withinSpace = true;
				for (int k = 0; k < getDimensionAmount(); k++)
				{
					double currData = getData(j, k);
					if (!(currData <= maxPoint.at(k) && currData >= minPoint.at(k)))
					{
						withinSpace = false;
						break;
					}
				}
				if (withinSpace)
				{
					pointsInSpace.push_back(j);
				}
			}

			classCount.clear();

			// check if new space is pure enough
			for (int j = 0; j < pointsInSpace.size(); j++)
			{
				int currClass = getClassOfSet(pointsInSpace.at(j));

				if (classCount.find(currClass) != classCount.end())
				{
					classCount[currClass] += 1;
				}
				else
				{
					classCount[currClass] = 1;
				}
			}

			double currClassTotal = 0;
			double classTotal = 0;
			for (auto entry : classCount)
			{
				if (entry.first == majorityClass)
				{
					currClassTotal = entry.second;
				}

				classTotal += entry.second;
			}

			acc.push_back(currClassTotal / classTotal);
		}

		int highestAccIdx = 0;
		for (int j = 0; j < acc.size(); j++)
		{
			if (acc[j] > acc[highestAccIdx])
			{
				highestAccIdx = j;
			}
		}

		// if acc meets threshold
		if (acc[highestAccIdx] >= accThreshold)
		{
			actionTaken = true;

			vector<double> maxPoint = vector<double>();
			vector<double> minPoint = vector<double>();

			// define combined space
			for (int j = 0; j < getDimensionAmount(); j++)
			{
				double newLocalMax = max(temp.getMaximum(j), blocks[highestAccIdx].getMaximum(j));
				double newLocalMin = min(temp.getMinimum(j), blocks[highestAccIdx].getMinimum(j));

				maxPoint.push_back(newLocalMax);
				minPoint.push_back(newLocalMin);
			}

			vector<int> pointsInSpace = vector<int>();

			// define all points in combined space
			for (int j = 0; j < getSetAmount(); j++)
			{
				bool withinSpace = true;
				for (int k = 0; k < getDimensionAmount(); k++)
				{
					double currData = getData(j, k);
					if (!(currData <= maxPoint.at(k) && currData >= minPoint.at(k)))
					{
						withinSpace = false;
						break;
					}
				}
				if (withinSpace)
				{
					pointsInSpace.push_back(j);
				}
			}

			temp.getSets()->clear();
			for (int j = 0; j < pointsInSpace.size(); j++)
			{
				temp.addSet(pointsInSpace.at(j));
			}
			temp.calculateValues(&dataDimensions);

			// store this index, to delete the cube that was combined
			toBeDeleted.insert(highestAccIdx);
		}

		int offset = 0;
		for (auto x : toBeDeleted)
		{
			blocks.erase(blocks.begin() + (x - offset));
			offset++;
		}

		blocks.push_back(temp);

		count--;

	} while (actionTaken || count > 0);


	for (int i = 0; i < blocks.size(); i++)
	{
		clusters.push_back(blocks.at(i));
	}
	
	// count cubes that share instance of data
	overlappingCubeCount = 0;
	for (int i = 0; i < getSetAmount(); i++)
	{
		int presentIn = 0;

		for (int j = 0; j < pureCubes.size(); j++)
		{
			for (int k = 0; k < pureCubes[j].getSets()->size(); k++)
			{
				if (pureCubes[j].getSets()->at(k) == i)
				{
					presentIn++;
					break;
				}
			}
		}

		if (presentIn > 1)
		{
			overlappingCubeCount++;
		}
	}

	totalCubeCount = clusters.size();

	for (int i = 0; i < clusters.size(); i++)
	{
		string name = to_string(i);
		clusters[i].setName(&name);
	}

	paintClusters = true;
}

void DataInterface::identicalCubes(set<int> * classesInCube, vector<int> * selectedInstances) {
	// Avoid creating identical cubes
	for (int j = 0; j < clusters.size(); j++) // mixed cubes
	{
		sort(selectedInstances->begin(), selectedInstances->end());
		if (*selectedInstances == *clusters[j].getSets())
		{
			classesInCube->clear();
			break;
		}
	}

	for (int j = 0; j < pureCubes.size(); j++) // pure cubes
	{
		sort(selectedInstances->begin(), selectedInstances->end());
		if (*selectedInstances == *pureCubes[j].getSets())
		{
			classesInCube->clear();
			break;
		}
	}
}

void DataInterface::createAutoCube(set<int> * classesInCube, vector<int> * selectedInstances, double threshold, int i) {
	// Create cubes that contain more than one class
	if (classesInCube->size() > 1) // mixed cubes
	{
		overlappingCubeCount++;
		ColorCustom clusterColor = ColorCustom();
		std::vector<double>* colorConponents = dataClasses[getClassOfSet(i)].getColor();
		clusterColor.setRed((*colorConponents)[0]);
		clusterColor.setGreen((*colorConponents)[1]);
		clusterColor.setBlue((*colorConponents)[2]);
		clusterColor.setAlpha((*colorConponents)[3]);
		clusters.push_back(SetCluster(clusterColor, selectedInstances, &dataDimensions));
		clusters[clusters.size() - 1].setRadius(threshold);
		string name = to_string(i) + " - Mixed";
		clusters[clusters.size() - 1].setName(&name);
		clusters[clusters.size() - 1].setOriginalSet(i);
	}
	else if (classesInCube->size() == 1)// create pure cube
	{
		totalCubeCount++;
		ColorCustom clusterColor = ColorCustom();
		std::vector<double>* colorConponents = dataClasses[getClassOfSet(i)].getColor();
		clusterColor.setRed((*colorConponents)[0]);
		clusterColor.setGreen((*colorConponents)[1]);
		clusterColor.setBlue((*colorConponents)[2]);
		clusterColor.setAlpha((*colorConponents)[3]);
		pureCubes.push_back(SetCluster(clusterColor, selectedInstances, &dataDimensions));
		pureCubes[pureCubes.size() - 1].setRadius(threshold);
		string name = to_string(i) + " - Pure";
		pureCubes[pureCubes.size() - 1].setName(&name);
		pureCubes[pureCubes.size() - 1].setOriginalSet(i);
	}
}

void DataInterface::simpleAdjacencyOriginal(set<int> * classesInCube, vector<int> * selectedInstances) {
		// Combine adjacent cubes
		SetCluster temp = SetCluster(ColorCustom(), selectedInstances, &dataDimensions);
		if (classesInCube->size() > 1) // mixed cubes
		{
			for (int j = 0; j < clusters.size(); j++)
			{
				bool isAdjacent = true;
				clusters[j].calculateValues(&dataDimensions);
				double newRadius = 0.0;
				for (int k = 0; k < getDimensionAmount(); k++)
				{
					newRadius = max(newRadius, (temp.getCenter()[k] - clusters[j].getCenter()[k]));
					if (!(abs(temp.getCenter()[k] - clusters[j].getCenter()[k]) / 2 <= getRadius()))
					{
						isAdjacent = false;
					}
				}
				if (isAdjacent)
				{
					for (int k = 0; k < temp.getSets()->size(); k++)
					{
						clusters[j].addSet(temp.getSets()->at(k));
					}
					classesInCube->clear();
					clusters[j].setRadius(newRadius);
					break;
				}
			}
		}
		else if (classesInCube->size() == 1)// combine pure cubes
		{
			for (int j = 0; j < pureCubes.size(); j++)
			{
				bool isAdjacent = true;
				pureCubes[j].calculateValues(&dataDimensions);
				double newRadius = 0.0;
				for (int k = 0; k < getDimensionAmount(); k++)
				{
					newRadius = max(newRadius, (temp.getCenter()[k] - pureCubes[j].getCenter()[k]));
					if (!(abs(temp.getCenter()[k] - pureCubes[j].getCenter()[k]) / 2 <= getRadius()))
					{
						isAdjacent = false;
					}
				}
				if (isAdjacent)
				{
					for (int k = 0; k < temp.getSets()->size(); k++)
					{
						pureCubes[j].addSet(temp.getSets()->at(k));
					}
					pureCubes[j].setRadius(newRadius);
					classesInCube->clear();
					break;
				}
			}
		}
}

void DataInterface::simpleAdjacency(set<int> * classesInCube, vector<int> * selectedInstances) {
	// Combine adjacent cubes
	SetCluster temp = SetCluster(ColorCustom(), selectedInstances, &dataDimensions);
	if (classesInCube->size() > 1) // mixed cubes
	{
		for (int j = 0; j < clusters.size(); j++)
		{
			bool isAdjacent = true;
			clusters[j].calculateValues(&dataDimensions);
			double newRadius = 0.0;
			for (int k = 0; k < getDimensionAmount(); k++)
			{
				newRadius = max(newRadius, (temp.getCenter()[k] - clusters[j].getCenter()[k]));
				if (!(abs(temp.getCenter()[k] - clusters[j].getCenter()[k]) / 2 <= getRadius()))
				{
					isAdjacent = false;
				}
			}
			if (isAdjacent)
			{
				//overlappingCubeCount++; // If desired to see count before combinations
				for (int k = 0; k < temp.getSets()->size(); k++)
				{
					clusters[j].addSet(temp.getSets()->at(k));
				}
				classesInCube->clear();
				clusters[j].setRadius(newRadius);
				break;
			}
		}
	}
	else if (classesInCube->size() == 1)// combine pure cubes
	{
		for (int j = 0; j < pureCubes.size(); j++)
		{
			bool isAdjacent = true;
			pureCubes[j].calculateValues(&dataDimensions);
			double newRadius = 0.0;
			for (int k = 0; k < getDimensionAmount(); k++)
			{
				newRadius = max(newRadius, (temp.getCenter()[k] - pureCubes[j].getCenter()[k]));
				if (!(abs(temp.getCenter()[k] - pureCubes[j].getCenter()[k]) / 2 <= getRadius()))
				{
					isAdjacent = false;
				}
			}
			if (isAdjacent)
			{
				for (int k = 0; k < temp.getSets()->size(); k++)
				{
					pureCubes[j].addSet(temp.getSets()->at(k));
				}
				pureCubes[j].setRadius(newRadius);
				classesInCube->clear();
				break;
			}
		}
	}
}

void DataInterface::sharedPointAdjacency(set<int> * classesInCube, vector<int> * selectedInstances) {
	// Combine adjacent cubes
	SetCluster temp = SetCluster(ColorCustom(), selectedInstances, &dataDimensions);
	if (classesInCube->size() > 1) // mixed cubes
	{
		for (int j = 0; j < clusters.size(); j++)
		{
			bool isAdjacent = false;
			clusters[j].calculateValues(&dataDimensions);
			double newRadius = 0.0;

			for (int a = 0; a < temp.getSets()->size(); a++)
			{
				for (int b = 0; b < clusters[j].getSets()->size(); b++)
				{
					bool sharedPoint = true;
					for (int c = 0; c < getDimensionAmount(); c++)
					{
						newRadius = max(newRadius, (temp.getCenter()[c] - clusters[j].getCenter()[c]));

						if (getData(temp.getSets()->at(a), c) !=
							getData(clusters[j].getSets()->at(b), c))
						{
							sharedPoint = false;
						}
					}
					if (sharedPoint)
					{
						isAdjacent = true;
						break;
					}
				}
				if (isAdjacent)
				{
					break;
				}
			}

			if (isAdjacent)
			{
				for (int k = 0; k < temp.getSets()->size(); k++)
				{
					clusters[j].addSet(temp.getSets()->at(k));
				}
				classesInCube->clear();
				clusters[j].setRadius(newRadius);
				break;
			}
		}
	}
	else if (classesInCube->size() == 1)// combine pure cubes
	{
		for (int j = 0; j < pureCubes.size(); j++)
		{
			bool isAdjacent = true;
			pureCubes[j].calculateValues(&dataDimensions);
			double newRadius = 0.0;

			for (int a = 0; a < temp.getSets()->size(); a++)
			{
				for (int b = 0; b < pureCubes[j].getSets()->size(); b++)
				{
					bool sharedPoint = true;
					for (int c = 0; c < getDimensionAmount(); c++)
					{
						newRadius = max(newRadius, (temp.getCenter()[c] - pureCubes[j].getCenter()[c]));

						if (getData(temp.getSets()->at(a), c) !=
							getData(pureCubes[j].getSets()->at(b), c))
						{
							sharedPoint = false;
						}
					}
					if (sharedPoint)
					{
						isAdjacent = true;
						break;
					}
				}
				if (isAdjacent)
				{
					break;
				}
			}

			if (isAdjacent)
			{
				//overlappingCubeCount++; // If desired to see count before combinations
				for (int k = 0; k < temp.getSets()->size(); k++)
				{
					pureCubes[j].addSet(temp.getSets()->at(k));
				}
				pureCubes[j].setRadius(newRadius);
				classesInCube->clear();
				break;
			}
		}
	}
}

void DataInterface::hyperBlockAdjacency(set<int> * classesInCube, vector<int> * selectedInstances) {
	// Combine adjacent cubes
	SetCluster temp = SetCluster(ColorCustom(), selectedInstances, &dataDimensions);
	if (classesInCube->size() > 1) // mixed cubes
	{
		for (int j = 0; j < clusters.size(); j++)
		{
			bool isAdjacent = true;
			clusters[j].calculateValues(&dataDimensions);
			double newRadius = 0.0;

			int sameCount = 0;
			int withinRangeCount = 0;

			for (int k = 0; k < getDimensionAmount(); k++)
			{
				newRadius = max(newRadius, (temp.getCenter()[k] - clusters[j].getCenter()[k]));

				if (abs(temp.getCenter()[k] - clusters[j].getCenter()[k]) / 2 <= getRadius())
				{
					withinRangeCount++;
				}

				if (temp.getCenter()[k] == clusters[j].getCenter()[k])
				{
					sameCount++;
				}
			}

			if (getDimensionAmount() - sameCount == 1 &&
				withinRangeCount == 1) // if is adjacent
			{
				for (int k = 0; k < temp.getSets()->size(); k++)
				{
					clusters[j].addSet(temp.getSets()->at(k));
				}
				classesInCube->clear();
				clusters[j].setRadius(newRadius);
				break;
			}
		}
	}
	else if (classesInCube->size() == 1)// combine pure cubes
	{
		for (int j = 0; j < pureCubes.size(); j++)
		{
			bool isAdjacent = true;
			pureCubes[j].calculateValues(&dataDimensions);
			double newRadius = 0.0;
			for (int k = 0; k < getDimensionAmount(); k++)
			{
				newRadius = max(newRadius, (temp.getCenter()[k] - pureCubes[j].getCenter()[k]));
				if (!(abs(temp.getCenter()[k] - pureCubes[j].getCenter()[k]) / 2 <= getRadius()))
				{
					isAdjacent = false;
				}
			}
			if (isAdjacent)
			{
				for (int k = 0; k < temp.getSets()->size(); k++)
				{
					pureCubes[j].addSet(temp.getSets()->at(k));
				}
				pureCubes[j].setRadius(newRadius);
				classesInCube->clear();
				break;
			}
		}
	}
}

void DataInterface::emptySpaceClusters() {

	vector<double> newCenter = vector<double>();
	double newRadius = 0.0;
	set<int> toBeDeleted = set<int>();

	// adjust for non-adjacent but seperated by empty space
	for (int i = 0; i < clusters.size(); i++) // compare each cube
	{
		if (toBeDeleted.find(i) != toBeDeleted.end())
		{
			continue;
		}

		for (int j = 0; j < clusters.size(); j++) // with every other cube
		{
			if (toBeDeleted.find(j) != toBeDeleted.end() || i == j)
			{
				continue;
			}

			newCenter.clear();
			newRadius = 0.0;
			double maxDifference = 0.0;

			// define new center and max difference of dimensions
			for (int k = 0; k < getDimensionAmount(); k++)
			{
				// each dimension of new center
				newCenter.push_back((abs( clusters[j].getCenter()[k] - clusters[i].getCenter()[k]) / 2)
									+ min(clusters[j].getCenter()[k], clusters[i].getCenter()[k]));

				// find maxDifference
				maxDifference = max(maxDifference, abs(clusters[j].getCenter()[k] - clusters[i].getCenter()[k]));
			}

			// define new radius
			newRadius = maxDifference + clusters[i].getRadius() + clusters[j].getRadius();

			bool blocked = false;
			for (int k = 0; k < pureCubes.size(); k++) // check if pure cube existing in space
			{
				for (int l = 0; l < getDimensionAmount(); l++)
				{
					if ( abs(pureCubes[k].getCenter()[l] - newCenter[l]) <
						(pureCubes[k].getRadius() + newRadius) )
					{
						blocked = true;
					}
				}
			}
			if (!blocked)
			{
				for (int l = 0; l < clusters[j].getSets()->size(); l++)
				{
					clusters[i].addSet(clusters[j].getSets()->at(l));
				}
				clusters[i].setRadius(newRadius);
				clusters[i].calculateValues(&dataDimensions);
				toBeDeleted.insert(j);
			}
		}
	}

	overlappingCubeCount -= toBeDeleted.size();
	totalCubeCount += overlappingCubeCount;

	// Clean up cubes
	int offset = 0;
	for (auto x : toBeDeleted)
	{
		clusters.erase(clusters.begin() + (x - offset));
		offset++;
	}
}

void DataInterface::emptySpacePureCubes() {

	vector<double> newCenter = vector<double>();
	double newRadius = 0.0;
	set<int> toBeDeleted = set<int>();

	// adjust for non-adjacent but seperated by empty space
	for (int i = 0; i < pureCubes.size(); i++) // compare each cube
	{
		if (toBeDeleted.find(i) != toBeDeleted.end())
		{
			continue;
		}

		for (int j = 0; j < pureCubes.size(); j++) // with every other cube
		{
			if (toBeDeleted.find(j) != toBeDeleted.end() || i == j)
			{
				continue;
			}

			newCenter.clear();
			newRadius = 0.0;
			double maxDifference = 0.0;

			// define new center and max difference of dimensions
			for (int k = 0; k < getDimensionAmount(); k++)
			{
				// each dimension of new center
				newCenter.push_back((abs(pureCubes[j].getCenter()[k] - pureCubes[i].getCenter()[k]) / 2)
					+ min(pureCubes[j].getCenter()[k], pureCubes[i].getCenter()[k]));

				// find maxDifference
				maxDifference = max(maxDifference, abs(pureCubes[j].getCenter()[k] - pureCubes[i].getCenter()[k]));
			}

			// define new radius
			newRadius = maxDifference + pureCubes[i].getRadius() + pureCubes[j].getRadius();

			bool blocked = false;
			for (int k = 0; k < clusters.size(); k++) // check if pure cube existing in space
			{
				for (int l = 0; l < getDimensionAmount(); l++)
				{
					if (abs(clusters[k].getCenter()[l] - newCenter[l]) <
						(clusters[k].getRadius() + newRadius))
					{
						blocked = true;
					}
				}
			}
			if (!blocked)
			{
				for (int l = 0; l < pureCubes[j].getSets()->size(); l++)
				{
					pureCubes[i].addSet(pureCubes[j].getSets()->at(l));
				}
				pureCubes[i].setRadius(newRadius);
				pureCubes[i].calculateValues(&dataDimensions);
				toBeDeleted.insert(j);
			}
		}
	}

	totalCubeCount -= toBeDeleted.size();

	// Clean up cubes
	int offset = 0;
	for (auto x : toBeDeleted)
	{
		pureCubes.erase(pureCubes.begin() + (x - offset));
		offset++;
	}
}

int DataInterface::getTotalCubeCount() {
	return totalCubeCount;
}

int DataInterface::getOverlappingCubeCount() {
	return overlappingCubeCount;
}

void DataInterface::cycleCombinationMode() {
	combinationMode++;
	if (combinationMode > 2)
	{
		combinationMode = 0;
	}
}

int DataInterface::getCombinationMode() {
	return combinationMode;
}

bool DataInterface::isCombining() {
	return willCombine;
}

void DataInterface::setWillCombine(bool willCombine) {
	this->willCombine = willCombine;
}

bool DataInterface::isThresholdMode() {
	return thresholdMode;
}

void DataInterface::setThresholdMode(bool isThresholdMode) {
	this->thresholdMode = isThresholdMode;
}

vector<SetCluster> DataInterface::getPureCubes() {
	return pureCubes;
}

bool DataInterface::selectorLineIsHidden() {
	return hideSelectorLine;
}

void DataInterface::hideSelector(bool isHidden) {
	this->hideSelectorLine = isHidden;
}

bool DataInterface::drawBorders() {
	return isDrawBorders;
}

void DataInterface::setDrawBorders(bool drawBorders) {
	this->isDrawBorders = drawBorders;
}

void DataInterface::combineAdjacentCubes() {
	int size = clusters.size();

	vector<SetCluster> temp = vector<SetCluster>();
	SetCluster newCluster;

	for (int i = 0; i < size; i++)
	{
		newCluster = clusters[0];
		clusters.erase(clusters.begin());

		for (int j = 0; j < clusters.size(); j++)
		{
			bool isAdjacent = true;

			for (int k = 0; k < getDimensionAmount(); k++)
			{
				if (!( abs(clusters[j].getCenter()[k] - newCluster.getCenter()[k]) / 2 <= getRadius() ))
				{
					isAdjacent = false;
				}
			}

			if (isAdjacent)
			{
				for (int k = 0; k < clusters[j].getSets()->size(); k++)
				{
					newCluster.addSet(clusters[j].getSets()->at(k));
				}

				temp.push_back(newCluster);
			}
		}
	}

	clusters.clear();
	for (int i = 0; i < temp.size(); i++)
	{
		clusters.push_back(temp.at(i));
	}
}

void DataInterface::deleteEqualClusters()
{
	set<int> toDelete = set<int>();

	for (int i = 0; i < clusters.size(); i++)
	{
		for (int j = 0; j < clusters.size(); j++)
		{
			if (clusters[i].getSets() == clusters[j].getSets() &&
				toDelete.find(i) == toDelete.end())
			{
				toDelete.insert(j);
			}
		}
	}

	int offset = 0;
	for (auto i : toDelete)
	{
		clusters.erase(clusters.begin() + (i - offset));
		offset++;
	}

}

void DataInterface::separateCluster(SetCluster cluster)
{
	std::vector<int> overlappedInstances = std::vector<int>();
	std::vector<int> nonoverlappedInstances = std::vector<int>();

	for (int i = 0; i < cluster.getSetNumber(); i++)
	{
		bool overlapped = false;
		int currentClass = getClassOfSet(cluster.getIndexOfSet(i));
		for (int j = 0; j < cluster.getSetNumber(); j++)
		{
			double currentPrev = getData(cluster.getIndexOfSet(i), 0);
			double comparedPrev = getData(cluster.getIndexOfSet(j), 0);
			for (int k = 1; k < getDimensionAmount(); k++)
			{
				double currentData = getData(cluster.getIndexOfSet(i), k);
				double comparedData = getData(cluster.getIndexOfSet(j), k);
				if (currentClass != getClassOfSet(cluster.getIndexOfSet(j)))
				{
					if (currentPrev == comparedPrev || comparedData == currentData)
					{
						overlapped = true;
						break;
					}

				}
				currentPrev = currentData;
				comparedPrev = comparedData;
			}
			if (overlapped) break;
		}
		if (overlapped)
		{
			overlappedInstances.push_back(cluster.getIndexOfSet(i));
		}
		else
		{
			nonoverlappedInstances.push_back(cluster.getIndexOfSet(i));
		}
	}

	ColorCustom clusterColor = ColorCustom();
	std::vector<double>* colorConponents = cluster.getColor();
	clusterColor.setRed((*colorConponents)[0]);
	clusterColor.setGreen((*colorConponents)[1]);
	clusterColor.setBlue((*colorConponents)[2]);
	clusterColor.setAlpha((*colorConponents)[3]);

	if (overlappedInstances.size() > 0 && nonoverlappedInstances.size() > 0)
	{
		clusters.push_back(SetCluster(clusterColor, &overlappedInstances/*, &dataDimensions*/));
		string name = *cluster.getName();
		clusters[clusters.size() - 1].setName(&name.append(" - overlap"));

		clusters.push_back(SetCluster(clusterColor, &nonoverlappedInstances/*, &dataDimensions*/));
		name = *cluster.getName();
		clusters[clusters.size() - 1].setName(&name.append(" - non-overlap"));
	}
	else if (overlappedInstances.size() > 0 )
	{
		clusters[clusters.size() - 1].getName()->append("- overlap");
	}
	else
	{
		clusters[clusters.size() - 1].getName()->append("- non-overlap");
	}
}

// reads the contents of the file, at fileName, into a vector
void DataInterface::readCustomFile(std::vector<std::vector<std::string>*>* fileContents) {
	// ensure file has data
	if (fileContents->size() < 2) {
		return;
	}
	int lastDataLine = 0;
	// find the last line of data
	int tokenNumber = (*fileContents)[0]->size();
	for (int i = 1; i < fileContents->size(); i++) {
		if (tokenNumber != (*fileContents)[i]->size()) {
			i = fileContents->size();
		}
		else {
			lastDataLine = i;
		}
	}
	if (lastDataLine < 1) {
		return;
	}
	// create the data classes
	// find the start of the class section
	int classSectionFirstLine = lastDataLine + 1;
	for (int i = classSectionFirstLine; i < fileContents->size(); i++) {
		if ((*(*fileContents)[i]).size() > 0) {
			if ((*(*fileContents)[i])[0].compare("classes") == 0) {
				classSectionFirstLine = i + 1;
				i = fileContents->size();
			}
		}
	}
	// find the line after the end of the class section
	int classSectionAfterLastLine = classSectionFirstLine;
	for (int i = classSectionFirstLine; i < fileContents->size(); i++) {
		if ((*(*fileContents)[i]).size() <= 0) {
			classSectionAfterLastLine = i;
			i = fileContents->size();
		}
		else if ((*(*fileContents)[i])[0].compare("") == 0) {
			break;
		}
	}
	dataClasses.clear();
	dataClasses.push_back(DataClass(0, "Default"));
	std::vector<double> newColor = std::vector<double>();
	newColor.push_back(0.0);
	newColor.push_back(0.0);
	newColor.push_back(1.0);
	newColor.push_back(1.0);
	dataClasses[0].setColor(newColor);
	// check if there are classes to create
	if (classSectionFirstLine < classSectionAfterLastLine) {
		// create the classes
		for (int i = classSectionFirstLine; i < classSectionAfterLastLine; i++) {
			std::string className = (*(*fileContents)[i])[0];
			std::vector<double> colorComponents = std::vector<double>();
			colorComponents.push_back(stod((*(*fileContents)[i])[1]));
			colorComponents.push_back(stod((*(*fileContents)[i])[2]));
			colorComponents.push_back(stod((*(*fileContents)[i])[3]));
			colorComponents.push_back(stod((*(*fileContents)[i])[4]));
			this->addClass();
			dataClasses[getClassAmount() - 1].setName(&className);
			dataClasses[getClassAmount() - 1].setColor(colorComponents);
		}

	}

	// get number of sets
	int setNumber = lastDataLine;
	// get dimension names
	std::vector<std::string> headers = std::vector<std::string>((*fileContents)[0]->size() - 2);
	for (int i = 2; i < (*fileContents)[0]->size(); i++) {
		std::string newDimensionName = (*(*fileContents)[0])[i];
		dataDimensions.push_back(new Dimension(i - 2, setNumber));
		dataDimensions[i - 2]->setName(&newDimensionName);
	}

	// create data sets
	for (int i = 1; i <= lastDataLine; i++) {
		std::string newSetName = (*(*fileContents)[i])[1];
		std::string setClassName = (*(*fileContents)[i])[0];
		int classIndex = 0;
		for (int j = 0; j < dataClasses.size(); j++) {
			if (dataClasses[j].getName()->compare(setClassName) == 0) {
				classIndex = j;
				j = dataClasses.size();
			}
		}
		dataSets.push_back(DataSet(i - 1, classIndex));
		dataSets[dataSets.size()-1].setName(newSetName);
		dataClasses[classIndex].addSet(i - 1);
	}

	// add data structure
	for (int i = 2; i < (*fileContents)[0]->size(); i++) {
		Dimension* currentDimension = dataDimensions[i - 2];
		for (int j = 1; j <= lastDataLine; j++) {
			double newData = std::stod((*(*fileContents)[j])[i]);
			currentDimension->setData(j - 1, newData);
		}
		currentDimension->calibrateData();
	}

	for (int i = classSectionAfterLastLine + 1; i < fileContents->size(); i++) {
		parseLine((*fileContents)[i]);
	}
}
// parses a command line from a save file
void DataInterface::parseLine(std::vector<std::string>* lineTokens) {
	if (lineTokens == nullptr) {
		// do nothing
	}
	else if (lineTokens->size() == 0) {
		// do nothing
	}
	// read data
	else if ((*lineTokens)[0].compare("shift") == 0) {
		if (lineTokens->size() >= 3) {
			int index = stoi((*lineTokens)[1]);
			double shift = stod((*lineTokens)[2]);
			if (index < 0 || index >= this->getSetAmount()) {
				return;
			}
			else {
				this->dataDimensions[index]->shiftDataBy(shift);
			}
		}
	}
	else if ((*lineTokens)[0].compare("bounds") == 0) {
		if (lineTokens->size() >= 4) {
			int dimIndex = stoi((*lineTokens)[1]);
			double max = stod((*lineTokens)[2]);
			double min = stod((*lineTokens)[3]);
			if (dimIndex < 0 || dimIndex >= getDimensionAmount()) {
				return;
			}
			dataDimensions[dimIndex]->setCalibrationBounds(max, min);
		}
	}
	else if ((*lineTokens)[0].compare("original indexes") == 0) {
		if (lineTokens->size() >= this->getDimensionAmount() + 1) {
			for (int i = 0; i < getDimensionAmount(); i++) {
				int originalIndex = stoi((*lineTokens)[i + 1]);
				this->dataDimensions[i]->setOriginalIndex(originalIndex);
			}
		}
	}
	else if ((*lineTokens)[0].compare("hypercube") == 0) {
		if (lineTokens->size() >= 3) {
			int index = stoi((*lineTokens)[1]);
			double radius = stod((*lineTokens)[2]);
			if (index < 0 || index >= this->getSetAmount()) {
				return;
			}
			else {
				if (radius < 0) {
					radius *= -1;
				}
				this->hypercube(index, radius);
			}
		}
	}
	else if ((*lineTokens)[0].compare("useMean") == 0) {
		if (lineTokens->size() >= 2) {
			bool newUseMean = stoi((*lineTokens)[1]);
			this->setUseMeanForClusters(newUseMean);
		}
	}
	else if ((*lineTokens)[0].compare("artificial calibration") == 0) {
		if (lineTokens->size() >= 5) {
			int index = stoi((*lineTokens)[1]);
			bool isCalibrated = stoi((*lineTokens)[2]);
			double maximum = stod((*lineTokens)[3]);
			double minimum = stod((*lineTokens)[4]);
			if (index < 0 || index >= this->getDimensionAmount()) {
				return;
			}
			this->setCalibrationBounds(index, maximum, minimum);
			if (isCalibrated == false) {
				this->dataDimensions[index]->clearArtificialCalibration();
			}
		}
	}
	else if ((*lineTokens)[0].compare("Default Class Color") == 0) {
		if (lineTokens->size() >= 5) {
			double red = stod((*lineTokens)[1]);
			double green = stod((*lineTokens)[2]);
			double blue = stod((*lineTokens)[3]);
			double alpha = stod((*lineTokens)[4]);
			std::vector<double> color = std::vector<double>();
			color.push_back(red);
			color.push_back(green);
			color.push_back(blue);
			color.push_back(alpha);
			dataClasses[0].setColor(&color);
		}
	}
	else if ((*lineTokens)[0].compare("Background Color") == 0) {
		if (lineTokens->size() >= 5) {
			double red = stod((*lineTokens)[1]);
			double green = stod((*lineTokens)[2]);
			double blue = stod((*lineTokens)[3]);
			double alpha = stod((*lineTokens)[4]);
			backgroundColor.setRed(red);
			backgroundColor.setGreen(green);
			backgroundColor.setBlue(blue);
			backgroundColor.setAlpha(alpha);
		}
	}

	else if ((*lineTokens)[0].compare("Selected Set Color") == 0) {
		if (lineTokens->size() >= 5) {
			double red = stod((*lineTokens)[1]);
			double green = stod((*lineTokens)[2]);
			double blue = stod((*lineTokens)[3]);
			double alpha = stod((*lineTokens)[4]);
			selectedSetColor.setRed(red);
			selectedSetColor.setGreen(green);
			selectedSetColor.setBlue(blue);
			selectedSetColor.setAlpha(alpha);
		}
	}
	else if ((*lineTokens)[0].compare("Invert Dimension") == 0) {
		if (lineTokens->size() >= 2) {
			int dimensionIndex = stoi((*lineTokens)[1]);
			invertDimension(dimensionIndex);
		}
	}

}


// count number of characters
int DataInterface::countCharacters(vector<char>* characters, string* line)
{
	int count = 0;
	// iterate through all characters in passed linep
	for (int i = 0; i < line->size(); i++) {
		// check the character against each character in the passed vector
		for (int j = 0; j < characters->size(); j++) {
			if (line->at(i) == (*characters)[j]) {
				count++;
				j = (*characters).size();
			}
		}
	}
	return count;
}





// an ascending(left to right) merge sort of the passed dimension list by the set at the passed index
std::list<Dimension*>* DataInterface::mergeSortAscending(std::list<Dimension*>* listToSort, int setIndex) {
	if (listToSort->size() <= 1) {
		return listToSort;
	}
	std::list<Dimension*> leftList = std::list<Dimension*>();
	std::list<Dimension*>* ptrLeftList = &leftList;
	std::list<Dimension*> rightList = std::list<Dimension*>();
	std::list<Dimension*>* ptrRightList = &rightList;
	// split list
	while (listToSort->size() > 0) {
		// pop element for left list
		Dimension* currentDimension = listToSort->front();
		listToSort->pop_front();
		leftList.push_back(currentDimension);
		// pop element for right list
		if (listToSort->size() > 0) {
			currentDimension = listToSort->back();
			listToSort->pop_back();
			ptrRightList->push_front(currentDimension);
		}
	}
	// sort the split list
	ptrLeftList = mergeSortAscending(ptrLeftList, setIndex);
	ptrRightList = mergeSortAscending(ptrRightList, setIndex);


	// merge lists back together in sorted order
	while (ptrLeftList->size() > 0 && ptrRightList->size() > 0) {
		double dataLeft = ptrLeftList->front()->getData(setIndex);
		double dataRight = ptrRightList->front()->getData(setIndex);
		// find which to take
		if (dataLeft <= dataRight) {
			listToSort->push_back(ptrLeftList->front());
			ptrLeftList->pop_front();
		}
		else {
			listToSort->push_back(ptrRightList->front());
			ptrRightList->pop_front();
		}
	}
	// get the remaining dimensions
	while (ptrLeftList->size() > 0) {
		listToSort->push_back(ptrLeftList->front());
		ptrLeftList->pop_front();
	}
	while (ptrRightList->size() > 0) {
		listToSort->push_back(ptrRightList->front());
		ptrRightList->pop_front();
	}
	return listToSort;
}

// an descending(left to right) merge sort of the passed dimension list by the set at the passed index
std::list<Dimension*>* DataInterface::mergeSortDescending(std::list<Dimension*>* listToSort, int setIndex) {
	if (listToSort->size() <= 1) {
		return listToSort;
	}

	std::list<Dimension*> leftList = std::list<Dimension*>();
	std::list<Dimension*>* ptrLeftList = &leftList;
	std::list<Dimension*> rightList = std::list<Dimension*>();
	std::list<Dimension*>* ptrRightList = &rightList;
	// split list
	while (listToSort->size() > 0) {
		// pop element for left list
		Dimension* currentDimension = listToSort->front();
		listToSort->pop_front();
		leftList.push_back(currentDimension);
		// pop element for right list
		if (listToSort->size() > 0) {
			currentDimension = listToSort->back();
			listToSort->pop_back();
			rightList.push_front(currentDimension);
		}
	}
	// sort the split list
	ptrLeftList = mergeSortDescending(ptrLeftList, setIndex);
	ptrRightList = mergeSortDescending(ptrRightList, setIndex);


	// merge lists back together in sorted order
	while (ptrLeftList->size() > 0 && ptrRightList->size() > 0) {
		double dataLeft = ptrLeftList->front()->getData(setIndex);
		double dataRight = ptrRightList->front()->getData(setIndex);
		// find which to take
		if (dataLeft >= dataRight) {
			listToSort->push_back(ptrLeftList->front());
			ptrLeftList->pop_front();
		}
		else {
			listToSort->push_back(ptrRightList->front());
			ptrRightList->pop_front();
		}
	}
	// get the remaining dimensions
	while (ptrLeftList->size() > 0) {
		listToSort->push_back(ptrLeftList->front());
		ptrLeftList->pop_front();
	}
	while (ptrRightList->size() > 0) {
		listToSort->push_back(ptrRightList->front());
		ptrRightList->pop_front();
	}
	return listToSort;
}

// sorts the dimensions to their original places in a merge sort
std::list<Dimension*>* DataInterface::mergeSortOriginal(std::list<Dimension*>* listToSort) {
	if (listToSort->size() <= 1) {
		return listToSort;
	}

	std::list<Dimension*> leftList = std::list<Dimension*>();
	std::list<Dimension*>* ptrLeftList = &leftList;
	std::list<Dimension*> rightList = std::list<Dimension*>();
	std::list<Dimension*>* ptrRightList = &rightList;
	// split list
	while (listToSort->size() > 0) {
		// pop element for left list
		Dimension* currentDimension = listToSort->front();
		listToSort->pop_front();
		leftList.push_back(currentDimension);
		// pop element for right list
		if (listToSort->size() > 0) {
			currentDimension = listToSort->back();
			listToSort->pop_back();
			rightList.push_front(currentDimension);
		}
	}
	// sort the split list
	ptrLeftList = mergeSortOriginal(ptrLeftList);
	ptrRightList = mergeSortOriginal(ptrRightList);


	// merge lists back together in sorted order
	while (ptrLeftList->size() > 0 && ptrRightList->size() > 0) {
		int indexLeft = ptrLeftList->front()->getOriginalIndex();
		int indexRight = ptrRightList->front()->getOriginalIndex();
		// find which to take
		if (indexLeft <= indexRight) {
			listToSort->push_back(ptrLeftList->front());
			ptrLeftList->pop_front();
		}
		else {
			listToSort->push_back(ptrRightList->front());
			ptrRightList->pop_front();
		}
	}
	// get the remaining dimensions
	while (ptrLeftList->size() > 0) {
		listToSort->push_back(ptrLeftList->front());
		ptrLeftList->pop_front();
	}
	while (ptrRightList->size() > 0) {
		listToSort->push_back(ptrRightList->front());
		ptrRightList->pop_front();
	}
	return listToSort;
}

void DataInterface::setDisplayed(string name, bool displayed)
{
	for (int i = 0; i < clusters.size(); i++)
	{
		if (clusters[i].getName()->compare(name) == 0)
		{
			clusters[i].setDisplayed(displayed);
		}
	}
}

void DataInterface::xorClusters(int mainCluster, int subCluster)
{

	for (int i = 0; i < clusters[subCluster].getSets()->size(); i++)
	{
		for (int j = 0, size = clusters[mainCluster].getSets()->size(); j < size; j++)
		{
			if (clusters[mainCluster].getSets()->at(j) == clusters[subCluster].getSets()->at(i))
			{
				clusters[mainCluster].getSets()->erase(clusters[mainCluster].getSets()->begin() + j);
				size--;
				j--;
				break;
			}

		}
	}
}

void DataInterface::setHypercubeThreshold(double newThreshold) {
	this->hypercubeThreshold = newThreshold;
}

double DataInterface::getHypercubeThreshold() {
	return hypercubeThreshold;
}

void DataInterface::toggleHistogramMode(bool isHistogramMode) {
	this->histogramMode = isHistogramMode;
}

bool DataInterface::getHistogramMode() {
	return this->histogramMode;
}

int DataInterface::getVisibleDimensionCount() {
	int count = 0;

	for (int i = 0; i < dataDimensions.size(); i++)
	{
		if (dataDimensions.at(i)->isVisible())
		{
			count++;
		}
	}

	return count;
}

int DataInterface::getVisibleBlockCount() {
	int count = 0;

	for (int i = 0; i < clusters.size(); i++)
	{
		if (clusters.at(i).isDisplayed())
		{
			count++;
		}
	}

	return count;
}

bool DataInterface::drawMinLine() {
	return this->drawMin;
}

bool DataInterface::drawCenterLine() {
	return this->drawCenter;
}

bool DataInterface::drawMaxLine() {
	return this->drawMax;
}

void DataInterface::setDrawMin(bool drawMin) {
	this->drawMin = drawMin;
}

void DataInterface::setDrawCenter(bool drawCenter) {
	this->drawCenter = drawCenter;
}

void DataInterface::setDrawMax(bool drawMax) {
	this->drawMax = drawMax;
}



void DataInterface::findSubsetOfDimensions(bool isOverlapping) {

	for (int i = 0; i < dataDimensions.size(); i++)
	{
		dataDimensions[i]->setVisibility(true);
	}

	for (int i = 0; i < getDimensionAmount(); i++) // for every dimension
	{

		for (int j = 0; j < clusters.size(); j++) // look at every selected block
		{

			if (!clusters[j].isDisplayed())
			{
				continue;
			}

			bool isOverlapping = false;

			for (int k = 0; k < clusters.size(); k++) // and check if blocks space overlaps
			{
				if (!clusters[k].isDisplayed() || j == k)
				{
					continue;
				}

				if (clusters[j].getMaximum(i) > clusters[k].getMinimum(i)  &&
					clusters[j].getMinimum(i) < clusters[k].getMaximum(i))
				{
					dataDimensions[i]->setVisibility(false);
					isOverlapping = true;
					break; // breaks out of k loop
				}

			} // end k loop

			if (isOverlapping) break; // breaks out of j loop

		} // end j loop

	}// end i loop

}

bool DataInterface::getFrequencyMode() {
	return this->frequencyMode;
}

void DataInterface::setFrequencyMode(bool isFrequencyMode) {
	this->frequencyMode = isFrequencyMode;
}

void DataInterface::setQuadMode(bool isQuadMode) {
	this->quadMode = isQuadMode;
}

bool DataInterface::getQuadMode() {
	return this->quadMode;
}

string DataInterface::getLinguisticDescription() {
	string description = "";
	vector<vector<string>> dims = vector<vector<string>>();
	for (int i = 0; i < 3; i++) dims.push_back(vector<string>());

	if (isPaintClusters()) // selected hyper block
	{
		SetCluster currCluster = clusters[selectedClusterIndex];
		int totalSets = currCluster.getSetNumber();
		vector<int>* currSets = currCluster.getSets();

		for (int i = 0; i < getDimensionAmount(); i++)
		{
			if (getDataDimensions()->at(i)->isVisible())
			{
				// get average for this dimensions
				double avgData = 0.0;
				double total = 0.0;

				for (int j = 0; j < totalSets; j++)
				{
					total += getData(currSets->at(j), i);
				}

				avgData = total / totalSets;

				double span = 1.0;
				double med = 0.5;
				double threshold = (span / 3.0) / 2.0;

				if (avgData > med + threshold) // upper
				{
					dims[2].push_back(*(dataDimensions[i]->getName()));
				}
				else if (avgData <= med + threshold &&
					avgData >= med - threshold) // middle
				{
					dims[1].push_back(*(dataDimensions[i]->getName()));
				}
				else // lower
				{
					dims[0].push_back(*(dataDimensions[i]->getName()));
				}
			}
		}

		map<int, string> names = { {0, "lower"}, { 1,"middle" }, { 2,"upper" } };

		for (int i = 0; i < 3; i++)
		{
			if (dims[i].size() > 1)
			{
				description += "The data in dimensions ";

				for (int j = 0; j < dims[i].size() - 1; j++)
				{
					description += dims[i][j] + ", ";
				}

				description += "and " + dims[i][dims[i].size() - 1] + " are concentrated in the " + names[i] + " third.\n\n";
			}
			else if (dims[i].size() == 1)
			{
				description += "The data in dimension " + dims[i][0] + " is concentrated in the " + names[i] + " third.\n\n";

			}
			else
			{
				description += "No dimensions have their data concentrated in the " + names[i] + " third.\n\n";
			}
		}
	}
	else // all data
	{
		for (int i = 1; i < getClassAmount(); i++)
		{
			dims.clear();
			for (int j = 0; j < 3; j++) dims.push_back(vector<string>());

			DataClass currClass = getClasses()[i];
			int totalSets = currClass.getSetNumber();
			vector<int>* currSets = currClass.getSetsInClass();

			for (int j = 0; j < getDimensionAmount(); j++)
			{
				if (getDataDimensions()->at(j)->isVisible())
				{
					// get average for this dimensions
					double avgData = 0.0;
					double total = 0.0;

					for (int k = 0; k < totalSets; k++)
					{
						total += getData(currSets->at(k), j);
					}

					avgData = total / totalSets;

					double span = 1.0;
					double med = 0.5;
					double threshold = (span / 3.0) / 2.0;

					if (avgData > med + threshold) // upper
					{
						dims[2].push_back(*(dataDimensions[j]->getName()));
					}
					else if (avgData <= med + threshold &&
						avgData >= med - threshold) // middle
					{
						dims[1].push_back(*(dataDimensions[j]->getName()));
					}
					else // lower
					{
						dims[0].push_back(*(dataDimensions[j]->getName()));
					}
				}
			}

			description += "--- " + *(currClass.getName()) + " ---\n\n";

			map<int, string> names = { {0, "lower"}, { 1,"middle" }, { 2,"upper" } };

			for (int j = 0; j < 3; j++)
			{
				if (dims[j].size() > 1)
				{
					description += "The data in dimensions ";

					for (int k = 0; k < dims[j].size() - 1; k++)
					{
						description += dims[j][k] + ", ";
					}

					description += "and " + dims[j][dims[j].size() - 1] + " are concentrated in the " + names[j] + " third.\n\n";
				}
				else if (dims[j].size() == 1)
				{
					description += "The data in dimension " + dims[j][0] + " are concentrated in the " + names[j] + " third.\n\n";

				}
				else
				{
					description += "No dimensions have their data concentrated in the " + names[j] + " third.\n\n";
				}
			}
		}
	}

	return description;
}

int DataInterface::getOverlappingCount(int pointIndex)
{
	int count = 0;

	for (int i = 0; i < getSetAmount(); i++)
	{
		if (i == pointIndex || getClassOfSet(pointIndex) == getClassOfSet(i)) continue;

		for (int j = 0; j < getDimensionAmount() - 1; j++)
		{
			double left = getData(pointIndex, j);
			double right = getData(pointIndex, j + 1);
			double otherLeft = getData(i, j);
			double otherRight = getData(i, j + 1);

			if (left <= otherLeft && right >= otherRight ||
				left >= otherLeft && right <= otherRight)
			{
				count++;
				break;
			}
		}
	}

	return count;
}

void DataInterface::createOverlaps() {

	double radius = 0.2;
	std::vector<int> selectedSets = std::vector<int>();

	for (int k = 0; k < getSetAmount(); k++)
	{
		string clusterName = to_string(k);
		selectedSets.clear();

		for (int i = 0; i < getSetAmount(); i++)
		{
			bool withinCube = true;
			for (int j = 0; j < getDimensionAmount(); j++)
			{
				if (!(getData(i, j) >= getData(k, j) - radius && getData(i, j) <= getData(k, j) + radius))
				{
					withinCube = false;
				}
			}
			if (withinCube)
			{
				selectedSets.push_back(i);
			}
		}

		ColorCustom clusterColor = ColorCustom();
		int originalSetClass = getClassOfSet(k);
		std::vector<double>* colorConponents = getClassColor(originalSetClass);
		clusterColor.setRed((*colorConponents)[0]);
		clusterColor.setGreen((*colorConponents)[1]);
		clusterColor.setBlue((*colorConponents)[2]);
		clusterColor.setAlpha((*colorConponents)[3]);

		overlaps.push_back(SetCluster(clusterColor, &selectedSets, &dataDimensions));
		overlaps[overlaps.size() - 1].setRadius(radius);
		overlaps[overlaps.size() - 1].setOriginalSet(k);
		overlaps[overlaps.size() - 1].setName(&clusterName);
		overlaps[overlaps.size() - 1].setDisplayed(true);
	}

}

void DataInterface::setOverlapMode(bool overlapMode) {
	this->overlapMode = overlapMode;
}

bool DataInterface::getOverlapMode() {
	return this->overlapMode;
}

void DataInterface::setNominalSetsMode(bool NominalSetsMode) {
	this->nominalSetsMode = NominalSetsMode;
}

bool DataInterface::getNominalSetsMode() {
	return this->nominalSetsMode;
}

vector<SetCluster> * DataInterface::getOverlaps() {
	return &this->overlaps;
}

int DataInterface::getImpurities(int index) {

	int origClass = getClassOfSet(overlaps[index].getOriginalSet());
	int count = 0;

	for (int i = 0; i < overlaps[index].getSetNumber(); i++)
	{
		int currClass = getClassOfSet(overlaps[index].getSets()->at(i));

		if (origClass == currClass) continue;

		count++;
	}

	return count;

}

map<std::string, double> DataInterface::getAboveOne() {
	return this->aboveOne;
}
