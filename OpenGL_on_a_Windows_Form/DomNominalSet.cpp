#include "stdafx.h"
#include "DomNominalSet.h"
#include "MonotoneBooleanChains.h"
#include <msclr\marshal_cppstd.h>
#include <math.h>
#include <cstdlib>

using namespace System::Windows::Forms;

//Default Constructor:
DomNominalSet::DomNominalSet()
{
	file = nullptr;
}

//Constructor:
DomNominalSet::DomNominalSet(DataInterface *file, double worldHeight, double worldWidth)
{
	//Save passed values.
	this->file = file;
	this->worldHeight = worldHeight;
	this->worldWidth = worldWidth;

	//Calculate data.
	//Get the frequency of values per class to use to calulate dominance percentage and 
	//overall block height:
	this->valueFreqPerClass = getValuePerClassFreq();

	//At this point, we have how often values show up for each class. Now we need to calculate the percentage of the block that the
	//dominant class will take up. To do this, we find dominant class nnumber and devide it by the number of times the value shows
	//up in the block that we are working with.
	this->classPercPerBlock = getClassPercPerBlock(this->valueFreqPerClass);

	//Now we have both the percentages of how much each dominant set will take up of the block as well as the value frequencies for each
	//class. Now we need to calculate the actual height of the blocks. To do this, we will be adding all the values together and then 
	//normalizing the ammount from 0 to 1 to be able to draw it using OpenGL.
	this->blockHeights = getBlockHeights(this->valueFreqPerClass);

	//At this point we have both the class freqencies by block as well as the block overall percentage so we can draw
	//the blocks. This means we know what percentage of the coordinate is made by the block and what percentage of 
	//each block will be filled by the dominant class.
	sortedByPurityVector = getSortByPurity(this->blockHeights, this->classPercPerBlock);

	//Sorting normally by freqency.
	//sortedByFreqVector = getSortByFreqency(this->blockHeights);

	//Sorting by class on top and bottom (2 class max, testing case).
	this->sortedByClassVector = getSortByClass(this->blockHeights, this->classPercPerBlock);

	//Calculate line positions.
	drawRectangles(sortedByPurityVector, classPercPerBlock, worldWidth);
	this->calculateLinePositions(worldWidth);
}

//ReCalculateData:
void DomNominalSet::reCalculateData()
{

	bool shiftVerticalMode = file->getShiftMode();
	bool shiftHorizontalMode = file->getReOrderMode();
	bool invertMode = file->getInvertMode();

	//If positions need to be recalculated.
	if (shiftVerticalMode || shiftHorizontalMode || invertMode)
	{
		this->valueFreqPerClass = getValuePerClassFreq();
		this->classPercPerBlock = getClassPercPerBlock(this->valueFreqPerClass);
		this->blockHeights = getBlockHeights(this->valueFreqPerClass);
		sortedByPurityVector = getSortByPurity(this->blockHeights, this->classPercPerBlock);
	}
}

//getClassPerPercBlock
//Desc: Calculates the percentage of the block that the dominant class will take up.
vector<vector<unordered_map<double, double>*>*>* DomNominalSet::getClassPercPerBlock(vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass)
{
	vector<vector<unordered_map<double, double>*>*>* toReturn = new vector<vector<unordered_map<double, double>*>*>();

	//===============BUILDING CLASSPERCPERBLOCK==============

	//Itterate over number of dimensions.
	for (int j = 0; j < this->file->getDimensionAmount(); j++)
	{
		//Add the vector of classes to the dimension.
		toReturn->push_back(new vector<unordered_map<double, double>*>());
		vector<unordered_map<double, double>*>* curDimensionVec = valueFreqPerClass->at(j);

		//If the dimension is visible.
		if (file->isDimensionVisible(j))
		{
			//Add classes to vector for this dimension.
			for (int k = 0; k < file->getClassAmount() - 2; k++) // -2 because of Default and 'class' Column.
			{
				toReturn->at(j)->push_back(new unordered_map<double, double>());
				unordered_map <double, double>* curMap = curDimensionVec->at(k);

				for (std::unordered_map<double, double>::iterator iter = curMap->begin(); iter != curMap->end(); ++iter)
				{
					double f = iter->first;
					double s = iter->second;

					//Add values.
					toReturn->at(j)->at(k)->insert({ f, s });

				}

			}
		}

	}

	//=====================================================


	//Itterate over dimensions.
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{
		//Get the dimension data.
		vector<unordered_map<double, double>*>* curDimensionVec = toReturn->at(i);
		unordered_map<double, double> dimensionCount;

		//If the dimension is visible.
		if (file->isDimensionVisible(i))
		{
			//Go over each map in the dimension vector and turn to percentages.
			for (int j = 0; j < curDimensionVec->size(); j++)
			{

				unordered_map<double, double>* temp = curDimensionVec->at(j);

				for (std::unordered_map<double, double>::iterator iter = temp->begin(); iter != temp->end(); ++iter)
				{
					if (dimensionCount.find(iter->first) == dimensionCount.end())
					{
						dimensionCount.insert({ iter->first, iter->second });
					}
					else
					{
						//Increment occurance of current data.
						std::unordered_map<double, double>::iterator it = dimensionCount.find(iter->first);
						it->second += iter->second;
					}
				}

			}

			for (int j = 0; j < curDimensionVec->size(); j++)
			{

				unordered_map<double, double>* curMap = curDimensionVec->at(j);

				//Go over map and calculate percentages.
				for (std::unordered_map<double, double>::iterator iter = curMap->begin(); iter != curMap->end(); ++iter)
				{
					double numOfKey = dimensionCount.find(iter->first)->second;

					iter->second = (iter->second / numOfKey);
				}

			}
		}

	}

	return toReturn;
}

//getBlockHeights
//Desc: Adds up all frequencies and calculates the height of each block normilzied from 0 to 1 to be drawn with viscanvas.
vector<unordered_map<double, double>*>* DomNominalSet::getBlockHeights(vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass)
{
	vector<unordered_map<double, double>*>* toReturn = new vector<unordered_map<double, double>*>();

	//Fill the vector with unordered maps:
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		toReturn->push_back(new unordered_map<double, double>());
	}

	//Go over attributes.
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		//If the dimension is visible.
		if (file->isDimensionVisible(i))
		{
			//Go over classes:
			vector<unordered_map<double, double>*>* curDimensionVec = valueFreqPerClass->at(i);

			//Combine all values:
			for (int j = 0; j < curDimensionVec->size(); j++)
			{

				unordered_map<double, double>* nextClass = curDimensionVec->at(j);

				for (std::unordered_map<double, double>::iterator iter = nextClass->begin(); iter != nextClass->end(); ++iter)
				{
					double key = iter->first;
					if (toReturn->at(i)->find(key) == toReturn->at(i)->end())
					{
						toReturn->at(i)->insert({ key, iter->second });
					}
					else
					{
						double curVal = toReturn->at(i)->at(key);
						toReturn->at(i)->at(key) = curVal + nextClass->at(key);
					}

				}

			}
		}

	}

	//===Normailize data===

	//Iterate over vector:
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{

		//If the dimension is visible.
		if (file->isDimensionVisible(i))
		{
			double allValues = 0; //Recording all values to section block heights.

			//Get current unordered map:
			unordered_map<double, double>* curMap = toReturn->at(i);

			//Iterate over unordered map to find valus.
			for (std::unordered_map<double, double>::iterator iter = curMap->begin(); iter != curMap->end(); ++iter)
			{
				double freq = iter->second;
				allValues += freq;
			}

			//Apply the normilized values.
			for (std::unordered_map<double, double>::iterator iter = curMap->begin(); iter != curMap->end(); ++iter)
			{
				iter->second = (iter->second / allValues);
			}
		}

	}

	return toReturn;
}

//getValuePerClassFreq
//Desc: Gets how often classes show up for each frequency.
vector<vector<unordered_map<double, double>*>*>* DomNominalSet::getValuePerClassFreq()
{
	vector<vector<unordered_map<double, double>*>*>* toReturn = new vector<vector<unordered_map<double, double>*>*>();

	//Itterate over number of dimensions.
	for (int j = 0; j < this->file->getDimensionAmount(); j++)
	{
		//Add the vector of classes to the dimension.
		toReturn->push_back(new vector<unordered_map<double, double>*>());

		//If the dimension is visible.
		if (file->isDimensionVisible(j))
		{
			//Add classes to vector for this dimension.
			for (int k = 0; k < file->getClassAmount() - 2; k++) // -2 because of Default and 'class' Column.
			{
				toReturn->at(j)->push_back(new unordered_map<double, double>());
			}
		}
	}

	//Itterate over all attributes:
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{
		//If the dimension is visible
		if (file->isDimensionVisible(i))
		{
			//Itterate over rows and add value to class vector.
			for (int j = 0; j < file->getSetAmount(); j++)
			{
				//Get the current value of the row at this attribute.
				double currentData = this->file->getData(j, i);
				//Get the current class of the row at this attribute.
				double currentClass = this->file->getClassOfSet(j) - 1;

				//Now we have the values of each class and how freqently they appear.
				//Now add them to a vector of maps.

				//Get current class map.
				unordered_map<double, double>* curMap = toReturn->at(i)->at(currentClass);

				//Check to see if current data is already in the unordered map.
				if (curMap->find(currentData) == curMap->end())
				{
					//If not insert it.
					curMap->insert({ currentData, 1 });
				}
				else
				{
					//Increment occurance of current data.
					std::unordered_map<double, double>::iterator it = curMap->find(currentData);
					it->second++;
				}

			}
		}

	}

	return toReturn;
}

//getSortByPurity
//Desc: sorts block heights by purity and puts small values to the top.
vector<vector<pair<double, double>>> DomNominalSet::getSortByPurity(vector<unordered_map<double, double>*>* blockHeights, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock)
{
	vector<vector<double>> keyOrder;
	vector<vector<pair<double, double>>> keyDomFreqs;
	vector<vector<pair<double, double>>> sortedVector;//Vector to hold sorted values.

	//Go over dimensions:
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		unordered_map<double, double>* curMap = blockHeights->at(i);
		unordered_map<double, double>::iterator it;

		keyDomFreqs.push_back(vector<pair<double, double>>());

		//Go over keys in dimension.
		for (it = curMap->begin(); it != curMap->end(); it++)
		{
			double key = it->first;
			double value = it->second;
			double domFreq = 0;

			//Get the class values and indavidual freq.
			vector<unordered_map<double, double>*>* curDimensionVec = classPercPerBlock->at(i);

			//Iterate over freq and determine the highest percent for the key.
			for (int m = 0; m < curDimensionVec->size(); m++)
			{
				unordered_map<double, double>* nextClass = curDimensionVec->at(m);

				if (nextClass->find(key) != nextClass->end())
				{
					if (nextClass->at(key) >= domFreq)
					{
						domFreq = nextClass->at(key);
					}
				}
			}

			//Add the found dominant frequency to the vector.
			keyDomFreqs.at(i).push_back({ domFreq, key });

		}

	}

	//Sort the keyDomFreqs:

	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		sort(keyDomFreqs.at(i).begin(), keyDomFreqs.at(i).end());

		if (!this->file->isDimensionInverted(i))
		{
			reverse(keyDomFreqs.at(i).begin(), keyDomFreqs.at(i).end());
		}

	}

	//Fill the sorted vector:
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		sortedVector.push_back(vector<pair<double, double>>());

		//Get current unordered map:
		unordered_map<double, double>* curMap = blockHeights->at(i);
		vector<pair<double, double>> curKeyDom = keyDomFreqs.at(i);

		unordered_map<double, double>::iterator it;

		//Go over values in keyDomFreq and find actual freqencies.
		for (int j = 0; j < curKeyDom.size(); j++)
		{
			double key = curKeyDom.at(j).second;

			//Find actual frequency of this key.

			for (it = curMap->begin(); it != curMap->end(); it++)
			{
				if (it->first == key)
				{
					sortedVector.at(i).push_back({ key, it->second });
					break;
				}
			}
		}

	}

	//At this point, the values are sorted from largest to smallest. Now we want to put the smaller frequencies on top
	//to reduce the occlusion of the mutliple small lines over large blocks.
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{

		vector<pair<double, double>> curDim = sortedVector.at(i);

		vector<pair<double, double>> larger;
		vector<pair<double, double>> smaller;

		vector<pair<double, double>>::iterator it;

		double smallFreqPerc = file->getFreqSmall() / 100.0;

		//Go over vector of <key, freq> pairs and make two lists.
		for (it = curDim.begin(); it != curDim.end(); it++)
		{
			if (it->second <= smallFreqPerc)//Set to zero for testing. Was at 0.20.
			{
				smaller.push_back(*it);
			}
			else
			{
				larger.push_back(*it);
			}
		}

		curDim.clear();

		//combine the lists.
		for (int j = 0; j < larger.size(); j++)
		{
			curDim.push_back(larger.at(j));
		}

		for (int j = 0; j < smaller.size(); j++)
		{
			curDim.push_back(smaller.at(j));
		}

		//Assign. 
		sortedVector.at(i) = curDim;

	}

	return sortedVector;

}

//getSortByFreqency
//Desc: Returns sorted vec of block heights by freq.
vector<vector<pair<double, double>>> DomNominalSet::getSortByFreqency(vector<unordered_map<double, double>*>* blockHeights)
{
	
	vector<pair<double, double>> sortVec;//Vector for sorting.
	vector<vector<pair<double, double>>> sortedVector;//Vector to hold sorted values.

	//Go over every attribute of the row.
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		//Get current unordered map:
		unordered_map<double, double>* curMap = blockHeights->at(i);
		vector<pair<double, double>> toInsert;

		unordered_map<double, double>::iterator it;

		//add all kv to a vector:
		for (it = curMap->begin(); it != curMap->end(); it++)
		{
			sortVec.push_back(make_pair(it->second, it->first));
		}

		//Sort the vector.
		sort(sortVec.begin(), sortVec.end());

		//Insert sorted values into a new vector.
		for (int k = sortVec.size() - 1; k >= 0; k--)
		{
			toInsert.push_back({ sortVec[k].second, sortVec[k].first });
		}

		//Insert sorted sub vector into vector.
		sortedVector.push_back(toInsert);

		sortVec.clear();
	}

	return sortedVector;
}

//getSortByClass
//Desc: Returns sorted vec of blocks by class.
vector<vector<pair<double, double>>> DomNominalSet::getSortByClass(vector<unordered_map<double, double>*>* blockHeights, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock)
{
	vector<vector<pair<double, double>>> sortedVector = getSortByPurity(blockHeights, classPercPerBlock);//Vector to hold sorted values.
	vector<vector<pair<double, double>>> testVector;

	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		//Add a vector to the recorded vector.
		testVector.push_back(vector<pair<double, double>>());

		//Get current vector (attribute we are working with making blocks):
		vector<pair<double, double>> curVec = sortedVector[i];

		//Iterate over vector to find valus.
		for (int j = 0; j < curVec.size(); j++)
		{
			//Get key and frequency and draw a rectangle.
			double key = curVec[j].first;
			double freq = curVec[j].second;
			double domPerc = 0;
			double dominantClass = -1;

			//Go over classes and find dominant class:
			vector<unordered_map<double, double>*>* curDimensionVec = classPercPerBlock->at(i);

			for (int m = 0; m < curDimensionVec->size(); m++)
			{
				unordered_map<double, double>* nextClass = curDimensionVec->at(m);

				if (nextClass->find(key) != nextClass->end())
				{
					if (nextClass->at(key) >= domPerc)
					{
						domPerc = nextClass->at(key);
						dominantClass = m + 1;

					}
				}
			}

			//Record what the dominant class was.
			testVector.at(i).push_back({ dominantClass, key });
		}

	}

	//Clear the sorted vector.
	for (int i = 0; i < sortedVector.size(); i++)
	{
		sortedVector.at(i).clear();
	}


	//Sort each coordinate by class.
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{
		sort(testVector.at(i).begin(), testVector.at(i).end());
		reverse(testVector.at(i).begin(), testVector.at(i).end());
	}


	//Get actual freq.
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{
		unordered_map<double, double>* curMap = blockHeights->at(i);
		vector<pair<double, double>> curTestVec = testVector.at(i);


		unordered_map<double, double>::iterator it;

		//Go over values in keyDomFreq and find actual freqencies.
		for (int j = 0; j < curTestVec.size(); j++)
		{
			double key = curTestVec.at(j).second;

			//Find actual frequency of this key.

			for (it = curMap->begin(); it != curMap->end(); it++)
			{
				if (it->first == key)
				{
					sortedVector.at(i).push_back({ key, it->second });
					break;
				}
			}
		}

		reverse(sortedVector.at(i).begin(), sortedVector.at(i).end());

	}

	return sortedVector;
}

//calculateLinePositions:
//Desc: calualtes the positon of lines between coordinates and saves the data to the class object.
void DomNominalSet::calculateLinePositions(double worldWidth)
{
	this->linePosiitons.clear();

	int dimensionCount = 0; // Variable for the dimension index.
	int colorChoice = file->getNominalColor();
	glLineWidth(3.0); //Seting line width.

	//Data used for drawing.
	vector<double> leftCoordinate = vector<double>();
	vector<double> rightCoordinate = vector<double>();
	vector<double> frequency = vector<double>();
	vector<double> classVec = vector<double>();
	vector<int> colorIdx = vector<int>();
	vector<int> leftData;
	vector<int> rightData;
	bool alreadyExists = false;

	//Create Array of coordinates and Counts.
	for (int i = 0; i < file->getDimensionAmount() - 1; i++) // file->getDimensionAmount()
	{
		if (file->getVisibleDimensionCount() < 2) break;

		leftCoordinate.clear();
		rightCoordinate.clear();
		frequency.clear();
		classVec.clear();
		colorIdx.clear();
		leftData.clear();
		rightData.clear();
		alreadyExists = false;

		int j = i + 1;
		while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
		if (j >= file->getDimensionAmount()) continue;

		//Itterate over sets.
		for (int k = 0; k < this->file->getSetAmount(); k++)
		{
			double left = file->getData(k, i);
			double right = file->getData(k, j);
			double classOfCur = file->getClassOfSet(k);

			//Get Dominant Class for both left and right.

			//Get current vecotor.
			vector<pair<double, double>> curVecLeft = this->sortedVector[i];
			vector<pair<double, double>> curVecRight = this->sortedVector[j];

			//Get dominant class vector:
			vector<pair<double, double>> domVecLeft = this->domClass[i];
			vector<pair<double, double>> domVecRight = this->domClass[j];

			//Get grey middle vector:
			vector<pair<double, double>> greyVecLeft = this->middleOther[i];
			vector<pair<double, double>> greyVecRight = this->middleOther[j];

			//Itterate over the vector to find the dominant class.
			double domClassNumLeft;
			for (int k = 0; k < domVecLeft.size(); k++)
			{
				pair<double, double> p = domVecLeft.at(k);

				if (p.first == left)
				{
					domClassNumLeft = p.second;
					break;
				}
			}

			//Itterate over the vector to find the dominant class.
			double domClassNumRight;
			for (int k = 0; k < domVecRight.size(); k++)
			{
				pair<double, double> p = domVecRight.at(k);

				if (p.first == right)
				{
					domClassNumRight = p.second;
					break;
				}
			}

			double leftPosition;
			double rightPosition;

			//Check where to draw the line for right and left (dominant class or non dominant class).
			if (classOfCur == domClassNumLeft)
			{
				//find value in curVec iterating over kv pairs:
				for (int k = 0; k < curVecLeft.size(); k++)
				{
					pair<double, double> p = curVecLeft.at(k);

					if (p.first == left)
					{
						leftPosition = p.second;
						break;
					}
				}
			}
			else
			{
				//find value in curVec iterating over kv pairs:
				for (int k = 0; k < greyVecLeft.size(); k++)
				{
					pair<double, double> p = greyVecLeft.at(k);

					if (p.first == left)
					{
						leftPosition = p.second;
						break;
					}
				}
			}

			if (classOfCur == domClassNumRight)
			{
				//find value in curVec iterating over kv pairs:
				for (int k = 0; k < curVecRight.size(); k++)
				{
					pair<double, double> p = curVecRight.at(k);

					if (p.first == right)
					{
						rightPosition = p.second;
						break;
					}
				}
			}
			else
			{
				//find value in curVec iterating over kv pairs:
				for (int k = 0; k < greyVecRight.size(); k++)
				{
					pair<double, double> p = greyVecRight.at(k);

					if (p.first == right)
					{
						rightPosition = p.second;
						break;
					}
				}
			}

			// see if exists already, if so increment count;else add and count = 1
			alreadyExists = false;
			for (int l = 0; l < frequency.size(); l++)
			{
				if (leftCoordinate[l] == leftPosition &&
					rightCoordinate[l] == rightPosition && classVec[l] == classOfCur)
				{
					alreadyExists = true;
					frequency[l] = frequency[l] + 1.0;
					break; // break out of l loop
				}
			} // end l loop

			if (!alreadyExists)
			{
				leftCoordinate.push_back(leftPosition);
				rightCoordinate.push_back(rightPosition);
				leftData.push_back(left);
				rightData.push_back(right);
				frequency.push_back(1.0);
				classVec.push_back(classOfCur);
				colorIdx.push_back(k);
			}

		} // end k loop

		//Add data to class object for drawing later:
		DomNomSetsLinesBetweenCords newData = DomNomSetsLinesBetweenCords();
		newData.setLeftCoordinate(leftCoordinate);
		newData.setRightCoordinate(rightCoordinate);
		newData.setLeftData(leftData);
		newData.setRightData(rightData);
		newData.setFrequency(frequency);
		newData.setClassVec(classVec);
		newData.setColorIdx(colorIdx);

		this->linePosiitons.push_back(newData);
	}
}

//drawVisualization:
//Desc: Draws the Dominant Nominal Sets Visualization by first drawing rectangles then lines.
GLvoid DomNominalSet::drawVisualization()
{
	reCalculateData();

	//If the coordinates are being reordered or shifted, only draw blocks.
	//This is to keep the program from lagging too much.
	if (file->getReOrderMode() == false && file->getShiftMode() == false && file->getInvertMode() == false)
	{
		drawRectangles(this->sortedByPurityVector, this->classPercPerBlock, this->worldWidth);
		drawLines(this->worldWidth);
		if (this->file->getDNSHideCoordinatesMode()) drawSelectorBoxes(this->worldWidth);
		if (ruleData.size() != 0 && file->getDNSRuleVisualizationMode()) visualizeRules();
		drawHoverInfo(this->worldWidth);

	}
	else
	{
		drawRectangles(this->sortedByPurityVector, this->classPercPerBlock, this->worldWidth);
		if (this->file->getDNSHideCoordinatesMode()) drawSelectorBoxes(this->worldWidth);
		drawHoverInfo(this->worldWidth);
	}

}

//drawRectangles:
//Desc: Draws the rectangles and calculates positions for lines.
GLvoid DomNominalSet::drawRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth)
{
	int dimensionCount = 0; // Variable for the dimension index.
	int colorChoice = file->getNominalColor();
	glLineWidth(3.0); //Seting line width.
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.0

	//====Draw the rectangles====.
	vector<vector<pair<double, double>>> middleOther; //Vector to hold possition of middle of other block.
	vector<vector<pair<double, double>>> domClass; // Vector to hold what the dominant class is for each attribute.

	//Go over every attribute.
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		//If the dimension is set to visible.
		if (this->file->isDimensionVisible(i))
		{
			//Calculate the hight of coordinate for all of the blocks to be in.
			int HEIGHT_OF_ALL_BLOCKS = (file->getDimensionShift(i) * (this->worldHeight * 0.5) + this->worldHeight * 0.75) - ((file->getDimensionShift(i) * (this->worldHeight * 0.5) + this->worldHeight * 0.1));

			//Get current vector (attribute we are working with making blocks):
			vector<pair<double, double>> curVec = sortedByPurityVector[i];

			//Values to calculate where to put values.
			double blockOffsetVertical = ((file->getDimensionShift(i) * (this->worldHeight * 0.5) + this->worldHeight * 0.1)); //Account for shifting.
			double prevHeight = ((file->getDimensionShift(i) * (this->worldHeight * 0.5) + this->worldHeight * 0.1));

			//Push back vectors:
			domClass.push_back(vector<pair<double, double>>());
			middleOther.push_back(vector<pair<double, double>>());

			//Iterate over vector to find valus.
			for (int j = 0; j < curVec.size(); j++)
			{
				//Get key and frequency and draw a rectangle.
				double key = curVec[j].first;
				double freq = curVec[j].second;
				double domPerc = 0;
				double dominantClass = -1;
				const double GRAY_VAL = 1;

				//Go over classes and find dominant class:
				vector<unordered_map<double, double>*>* curDimensionVec = classPercPerBlock->at(i);

				for (int m = 0; m < curDimensionVec->size(); m++)
				{
					unordered_map<double, double>* nextClass = curDimensionVec->at(m);

					if (nextClass->find(key) != nextClass->end())
					{
						if (nextClass->at(key) >= domPerc)
						{
							domPerc = nextClass->at(key);
							dominantClass = m + 1;

						}
					}
				}

				//Record what the dominant class was.
				domClass.at(i).push_back({ key, dominantClass });

				//Draw the dominant set rectangle.
				glBegin(GL_QUADS);

				//Set Color:
				std::vector<double>* colorOfCurrent = this->file->getClassColor(dominantClass);
				glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], 0.5);

				// draw bottom left
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					(blockOffsetVertical)
				);

				// draw top left
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					(((domPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);

				// draw top right
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					(((domPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);

				// draw bottom right
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					(blockOffsetVertical)
				);

				glEnd();

				//CHECK IF WE NEED GREEN BORDER.
				//Get perc from user.
				double percFU = file->getPurityPerc();

				//Find value using key.
				int originalVal = -1;
				for (int m = 0; m < file->getSetAmount(); m++)
				{
					double dataAtCurSet = file->getData(m, i);

					if (dataAtCurSet == key)
					{
						originalVal = file->getOriginalData(m, i);
						break;
					}
				}

				string *dimName = file->getDimensionName(i);

				if ((*dimName == "X5" && originalVal == 7) || (*dimName == "X7" && originalVal == 2) || (*dimName == "X9" && originalVal == 7) || (*dimName == "X11" && originalVal == 2))//((domPerc) >= (percFU / 100) && !(freq <= 0.014))
				{
					//==Draw border==:
					glBegin(GL_LINE_STRIP);

					glColor4d(0, 255, 0, GRAY_VAL);
					glLineWidth(.5);

					// draw bottom left
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						(blockOffsetVertical - 4)
					);

					// draw bottom right
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						(blockOffsetVertical - 4)
					);

					// draw top right
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-4)
					);


					// draw top left
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-4)
					);

					// draw bottom left
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						(blockOffsetVertical - 4)
					);

					glEnd();

				}

				//Draw the grey other class rectangle.
				glBegin(GL_QUADS);

				//Set Color
				glColor4d(0, 0, 0, 0.15);

				// draw bottom left
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					(((domPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);

				// draw bottom right
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					(((domPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);


				// draw top right
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
				);

				// draw top left
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
				);

				glEnd();

				//Check if we need to draw the border.
				if (!(freq <= 0.014))
				{
					//==Draw border==:
					glBegin(GL_LINE_STRIP);

					glColor4d(0, 0, 0, GRAY_VAL);
					glLineWidth(.5);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
					);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
					);

					glEnd();

				}
				else //We dont need to draw the devider.
				{

					//==Draw Sides==:
					glBegin(GL_LINE_STRIP);

					glColor4d(0, 0, 0, GRAY_VAL);

					glLineWidth(.5);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
					);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						(prevHeight - 2)
					);

					glEnd();

					//==Draw Sides==:
					glBegin(GL_LINE_STRIP);

					glColor4d(0, 0, 0, GRAY_VAL);
					glLineWidth(.5);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
					);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						(prevHeight - 2)
					);

					glEnd();

					if (j == curVec.size() - 1 || curVec.at(j + 1).second > 0.014)
					{
						//==Draw border==:
						glBegin(GL_LINE_STRIP);

						glColor4d(0, 0, 0, GRAY_VAL);
						glLineWidth(100);

						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
							((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
						);

						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
							((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
						);

						glEnd();
					}

				}

				//Put lines at intersection of dominant class and gray block.
				double firstColorPosition = (((domPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical));
				double secondColorPosition = (((domPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical));

				//make sure specific color always goes on top.
				if (dominantClass == 2)
				{
					//Mutate vector to draw lines in next step.
					curVec[j].second = firstColorPosition;
					sortedByPurityVector[i] = curVec;
					middleOther.at(i).push_back({ key, secondColorPosition });
				}
				else
				{
					//Mutate vector to draw lines in next step.
					curVec[j].second = secondColorPosition;
					sortedByPurityVector[i] = curVec;
					middleOther.at(i).push_back({ key, firstColorPosition });
				}

				//Record the previous height.
				blockOffsetVertical += (((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)) - prevHeight);
				prevHeight = blockOffsetVertical;

				//////////////////Old Calcualtions////////////////////////:

				/*
				//Mutate vector to draw lines in next step.
				curVec[j].second = ((((domPerc * freq) / 2) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical));
				sortedByPurityVector[i] = curVec;

				//Calculate the position of greyRec.
				double x = (((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2) - (((domPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))) / 2;
				double middleGrey = (x + (((domPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + blockOffsetVertical));

				//Add the middle of the grey block to middle.
				middleOther.at(i).push_back({ key, middleGrey });

				//Record the previous height.
				blockOffsetVertical += (((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)) - prevHeight);
				prevHeight = blockOffsetVertical;
				*/

			}

			//==Draw bottom border==:
			glBegin(GL_LINE_STRIP);

			glColor4d(0, 0, 0, 1);
			glLineWidth(.5);

			//Top Left Point:
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
				(((file->getDimensionShift(i) * (this->worldHeight * 0.5) + this->worldHeight * 0.1))) //Accounts for shift.
			);

			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
				(((file->getDimensionShift(i) * (this->worldHeight * 0.5) + this->worldHeight * 0.1))) //Accounts for shift.
			);

			glEnd();

			dimensionCount++;
		}
		else
		{
			//Push back vectors:
			domClass.push_back(vector<pair<double, double>>());
			middleOther.push_back(vector<pair<double, double>>());
			domClass.at(i).push_back({ -1, -1 });
			middleOther.at(i).push_back({ -1, -1 });

		}
	}

	//Record vectors for line drawing.
	this->middleOther = middleOther;
	this->domClass = domClass;
	this->sortedVector = sortedByPurityVector;
}

//drawLines:
//Desc: Draws line sections between coordinates in the OPENGL window.
GLvoid DomNominalSet::drawLines(double worldWidth)
{
	int dimensionCount = 0; // Variable for the dimension index.
	int colorChoice = file->getNominalColor();
	glLineWidth(3.0); //Seting line width.
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.

	//Get what mode is being used currently.
	bool shiftVerticalMode = file->getShiftMode();
	bool shiftHorizontalMode = file->getReOrderMode();
	bool invertMode = file->getInvertMode();

	//Data used for drawing.
	vector<double> leftCoordinate = vector<double>();
	vector<double> rightCoordinate = vector<double>();
	vector<double> frequency = vector<double>();
	vector<double> classVec = vector<double>();
	vector<int> colorIdx = vector<int>();
	vector<int> leftData;
	vector<int> rightData;

	//Data for linguistic desc.
	vector<boolean> remainingFullLines = vector<boolean>();
	bool alreadyExists = false;
	int numOfLinesSetTransparent = 0;
	int numSmallLines = 0; // count number of lines.
	int numNDPoints = 0;

	//Set up vector to make keep track of lines.
	for (int i = 0; i < this->file->getSetAmount(); i++)
	{
		remainingFullLines.push_back(true);
	}

	//Create Array of coordinates and Counts.
	for (int i = 0; i < file->getDimensionAmount() - 1; i++) // file->getDimensionAmount()
	{
		if (!(file->getDataDimensions()->at(i)->isVisible())) continue;
		if (file->getVisibleDimensionCount() < 2) break;

		leftCoordinate.clear();
		rightCoordinate.clear();
		frequency.clear();
		classVec.clear();
		colorIdx.clear();
		leftData.clear();
		rightData.clear();
		alreadyExists = false;

		int j = i + 1;
		while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
		if (j >= file->getDimensionAmount()) continue;

		//If these modes are active, we need to re-calculate line position.
		if (shiftHorizontalMode || shiftHorizontalMode || invertMode)
		{
			calculateLinePositions(worldWidth);
		}

		//Get coordinate pair data.
		DomNomSetsLinesBetweenCords coordPairData = this->linePosiitons.at(i);
		leftCoordinate = coordPairData.getLeftCoordinate();
		rightCoordinate = coordPairData.getRightCoordinate();
		frequency = coordPairData.getFreqency();
		classVec = coordPairData.getClassVec();
		colorIdx = coordPairData.getcolorIdx();
		leftData = coordPairData.getLeftData();
		rightData = coordPairData.getRightData();

		//Keep track of how many small lines are shown.
		numSmallLines += frequency.size();

		// draw the data between these two dimensions, scaling width to count
		for (int k = 0; k < frequency.size(); k++)
		{
			int classVal = classVec.at(k);
			vector<double>* color = file->getSetColor(colorIdx[k]);

			double alpha = 1.0;// Alpha here incase we want to use it.
			double trans = file->getTranspLineThresh();
			double transparentPercent = (trans / 100.0);

			//If the line is less then 10% of the class AND the number of lines is greater then 6.
			if (frequency[k] / file->getSetAmount(classVal) <= transparentPercent && frequency.size() > 6)
			{
				alpha = 0.1; //Make alpha low.
				numOfLinesSetTransparent++;//Increment the amount of transparent lines.

				//Get left and right data of this freq.
				int left = leftData[k];
				int right = rightData[k];

				//Itterate over the sets and check to see if any data matches the left and right coordinates.
				for (int m = 0; m < remainingFullLines.size(); m++)
				{
					//Get left and right of the current.
					double curLeft = file->getData(m, i);
					double curRight = file->getData(m, j);

					//Check to see if the current set is one of the sets being set transparent.
					if (curLeft == left && curRight == right)
					{
						remainingFullLines.at(m) = false;
					}

				}

			}

			//Check to see if we have less then percent, record:
			if (alpha != 0.1)
			{
				numNDPoints += frequency.at(k);
			}

			glColor4d((*color)[0], (*color)[1], (*color)[2], alpha);
			double width = (frequency[k] / (file->getSetAmount(classVal) / 15));
			glLineWidth(width);
			double leftData = leftCoordinate[k];
			double rightData = rightCoordinate[k];

			double leftCoordinateX = (-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1));
			leftCoordinateX = leftCoordinateX * -1;
			double rightCoordinateX = (-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 2));
			rightCoordinateX = rightCoordinateX * -1;

			//===Determine the equation of the perpendicular line=== (Y = mX + b);
			//Start with calculating the slope: (m)
			double slope = (leftData - rightData) / (leftCoordinateX - rightCoordinateX);
			double perpSlope = 0;
			if (slope != 0)
			{
				width = width / 2; //Adjust the width for drawing since the line width is drawn both ways from the center.

				//TWO CASES: 
				//1.) Slope > 0.
				//2.) Slope < 0.

				//Perpendicular slope is opposite reciprical.
				perpSlope = 1 / (slope * -1);

				if (slope < 0) // 1.) Slope > 0
				{
					//Determine what (b) is using one of the given points.
					double bLeft = leftData - (perpSlope * leftCoordinateX); //Where leftData is Y and leftCoordinateX is X.

					//We now have m and b. Thus, if we have a x position we can find a y position.
					//With our width as our distance, we will use the equation of a circle to calculate the x position
					//on the line that is distance d from the origin point (left point.

					//Calculate the x position.
					double newXPositionLeft;
					if (classVal == 2)
					{
						newXPositionLeft = leftCoordinateX + sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}
					else
					{
						newXPositionLeft = leftCoordinateX - sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}

					//Calculate the y position.
					double newYPositionLeft = (perpSlope * newXPositionLeft) + bLeft;

					//===DO THE SAME FOR THE RIGHT===

					//Determine what (b) is using one of the given points.
					double bRight = rightData - (perpSlope * rightCoordinateX); //Where leftData is Y and leftCoordinateX is X.

					//We now have m and b. Thus, if we have a x position we can find a y position.
					//With our width as our distance, we will use the equation of a circle to calculate the x position
					//on the line that is distance d from the origin point (left point.

					//Calculate the x position.
					double newXPositionRight = 0;
					if (classVal == 2)
					{
						newXPositionRight = rightCoordinateX + sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}
					else
					{
						newXPositionRight = rightCoordinateX - sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}

					//Calculate the y position.
					double newYPositionRight = (perpSlope * newXPositionRight) + bRight;

					// draw a line using both points
					glBegin(GL_LINE_STRIP);
					glVertex2d(-newXPositionLeft, newYPositionLeft);
					glVertex2d(-newXPositionRight, newYPositionRight);
					glEnd();

				}
				else //2.) SLOPE < 0
				{
					//Determine what (b) is using one of the given points.
					double bLeft = leftData - (perpSlope * leftCoordinateX); //Where leftData is Y and leftCoordinateX is X.

					//We now have m and b. Thus, if we have a x position we can find a y position.
					//With our width as our distance, we will use the equation of a circle to calculate the x position
					//on the line that is distance d from the origin point (left point.

					//Calculate the x position.
					double newXPositionLeft;
					if (classVal == 2)
					{
						newXPositionLeft = leftCoordinateX - sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}
					else
					{
						newXPositionLeft = leftCoordinateX + sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}

					//Calculate the y position.
					double newYPositionLeft = (perpSlope * newXPositionLeft) + bLeft;

					//===DO THE SAME FOR THE RIGHT===

					//Determine what (b) is using one of the given points.
					double bRight = rightData - (perpSlope * rightCoordinateX); //Where leftData is Y and leftCoordinateX is X.

					//We now have m and b. Thus, if we have a x position we can find a y position.
					//With our width as our distance, we will use the equation of a circle to calculate the x position
					//on the line that is distance d from the origin point (left point.

					//Calculate the x position.
					double newXPositionRight = 0;
					if (classVal == 2)
					{
						newXPositionRight = rightCoordinateX - sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}
					else
					{
						newXPositionRight = rightCoordinateX + sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}

					//Calculate the y position.
					double newYPositionRight = (perpSlope * newXPositionRight) + bRight;

					// draw a line using both points
					glBegin(GL_LINE_STRIP);
					glVertex2d(-newXPositionLeft, newYPositionLeft);
					glVertex2d(-newXPositionRight, newYPositionRight);
					glEnd();
				}

			}
			else //Slope == 0. Shift Y value.
			{
				if (width <= 2) width = 5;
				if (classVal == 2)
				{
					// draw a line using both points
					glBegin(GL_LINE_STRIP);
					glVertex2d(-leftCoordinateX, leftData + (width / 2) + 1);
					glVertex2d(-rightCoordinateX, rightData + (width / 2) + 1); //aDDED +1 
					glEnd();
				}
				else
				{
					// draw a line using both points
					glBegin(GL_LINE_STRIP);
					glVertex2d(-leftCoordinateX, leftData - (width / 2));
					glVertex2d(-rightCoordinateX, rightData - (width / 2));
					glEnd();
				}

			} // End of slope being 0.

			/////////////////////////Old Drawing://///////////////
			/*
			// draw a line using both points
			glBegin(GL_LINE_STRIP);
			glVertex2d(-leftCoordinateX, leftData);
			glVertex2d(-rightCoordinateX, rightData);
			glEnd();
			*/
			
		}

		dimensionCount++;

	}

	//Add the number of lines set transparent to the dominant nominal sets.
	file->setDNSLinesTransparent(numOfLinesSetTransparent);
	file->setDNSNumSmallLines(numSmallLines);
	file->setDNSnDPointsVisualized(numNDPoints);

	int numFullSets = 0;
	for (int i = 0; i < remainingFullLines.size(); i++)
	{
		if (remainingFullLines.at(i) == true)
		{
			numFullSets++;
		}
	}

	file->setDNSNumSetsVisualized(numFullSets);
}

//drawSelectorBoxes:
//Desc: Draws marker for selected and unselected dimensions. Currently only used with hiding dimensions.
GLvoid DomNominalSet::drawSelectorBoxes(double worldWidth)
{
	int dimensionCount = 0; // Variable for the dimension index.
	glLineWidth(2.0); //Seting line width.
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.0

	//Go over every attribute.
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		//If the dimension is set to be hidden.
		if (this->file->dimensionInHideListDNS(i))
		{
			//Draw a box.
			glBegin(GL_QUADS);

			//Set Color:
			glColor4d(0.0, 0.0, 1.0, 1.0);//Blue

			// draw bottom left
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 5),
				(-10)
			);

			// draw top left
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 5),
				(10)
			);

			// draw top right
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 5),
				(10)
			);

			// draw bottom right
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 5),
				(-10)
			);

			glEnd();

		}
		else
		{
			//Draw a box.
			glBegin(GL_LINE_STRIP);

			//Set Color:
			glColor4d(0.0, 0.0, 1.0, 1.0);//Blue

			// draw bottom left
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 5),
				(-10)
			);

			// draw top left
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 5),
				(10)
			);

			// draw top right
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 5),
				(10)
			);

			// draw bottom right
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 5),
				(-10)
			);

			// draw bottom left
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 5),
				(-10)
			);

			glEnd();

		}

		dimensionCount++;
	}
}

//drawHoverInfo:
//Desc: Draws info about blocks on hover of mouse over the block.
GLvoid DomNominalSet::drawHoverInfo(double worldWidth)
{
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Value fro drawing.

	//Check to see if the dimensions being hovered over is in the dimensions.
	if (file->getDimensionHover() >= 0 && file->getDimensionHover() < file->getDimensionAmount())
	{
		string temp = "Number of cases in each block in dimension " + to_string(file->getDimensionHover() + 1) + ": ";

		vector<unordered_map<double, double>*>* valuesPerDimension = new vector<unordered_map<double, double>*>();

		//Fill the vector with unordered maps:
		for (int i = 0; i < this->file->getDimensionAmount(); i++)
		{
			valuesPerDimension->push_back(new unordered_map<double, double>());
		}

		//Go over attributes.
		for (int i = 0; i < this->file->getDimensionAmount(); i++)
		{

			//Go over classes:
			vector<unordered_map<double, double>*>* curDimensionVec = valueFreqPerClass->at(i);

			//Combine all values:
			for (int j = 0; j < curDimensionVec->size(); j++)
			{

				unordered_map<double, double>* nextClass = curDimensionVec->at(j);

				for (std::unordered_map<double, double>::iterator iter = nextClass->begin(); iter != nextClass->end(); ++iter)
				{
					//Add the values together.
					double key = iter->first;
					if (valuesPerDimension->at(i)->find(key) == valuesPerDimension->at(i)->end())
					{
						valuesPerDimension->at(i)->insert({ key, iter->second });
					}
					else
					{
						double curVal = valuesPerDimension->at(i)->at(key);
						valuesPerDimension->at(i)->at(key) = curVal + nextClass->at(key);
					}

				}//End of iteration over values in curDimesnionVec.

			}//End of iteration over each attribue value freq per class.

		}//End of iteration over attributes.

		//Iterate over the combined values at the current hovered dimension.
		auto iterator = valuesPerDimension->at(file->getDimensionHover())->begin();
		for (auto iterator = valuesPerDimension->at(file->getDimensionHover())->begin(); iterator != valuesPerDimension->at(file->getDimensionHover())->end(); iterator++)
		{
			//Add the values to the output string.
			auto next = ++iterator;//Check the next position.
			iterator--;//Return iterator to current element.
			if(next == valuesPerDimension->at(file->getDimensionHover())->end()) temp += to_string(int(iterator->second)) + ".\n";
			else temp += to_string(int(iterator->second)) + ", ";
		}

		//Check to see if there are other UI in the way.
		int shiftDownVal = 0;
		if (file->getDNSHideCoordinatesMode())
		{
			shiftDownVal = 40;
		}

		//Draw text of block cases.:
		const char* text = temp.c_str();
		glColor4d(0.0, 0.0, 0.0, 1.0);//Black
		glRasterPos2d((-worldWidth / 2.0) + ((xAxisIncrement)) - 10, (0 - shiftDownVal));
		int length = (int)std::strlen(text);
		glListBase(2000);
		glCallLists(length, GL_UNSIGNED_BYTE, text);
		glEnd();

		//Check to see if DNS Rule Visualziation is selected.
		if (file->getDNSRuleVisualizationMode() == true)
		{
			//Check to see if any rules are visualizaed on this coordinate.
			unordered_map<int, string> rulesByCoordinate = file->getDNSRulesByCoordinate();
			int decVal = 1;

			//Iterate over rules and see if any are from the current hovered dimension.
			for (auto it = rulesByCoordinate.begin(); it != rulesByCoordinate.end(); it++)
			{
				//If this key is the coordinate, add the rule.
				if (it->first == file->getDimensionHover())
				{
					//Get current rule.
					temp = it->second;
					string token = "";
					//Parse the rule so it does not go off the screen.
					for (int i = 0; i < temp.size() - 1; i++)
					{
						token += temp.at(i);

						//If we get to seventy chars,
						if (temp.at(i) == '.' && temp.at(i+1) == ' ')
						{
							//Draw text:
							const char* text = token.c_str();
							glColor4d(0.0, 0.0, 0.0, 1.0);//Black
							glRasterPos2d((-worldWidth / 2.0) + ((xAxisIncrement)) - 10, (-25 * decVal));
							int length = (int)std::strlen(text);
							glListBase(2000);
							glCallLists(length, GL_UNSIGNED_BYTE, text);
							glEnd();

							token = "";
							decVal++;
							
							i++; // skips next " ".
						}

					}

					if (token != "")
					{
						//Draw text:
						const char* text = token.c_str();
						glColor4d(0.0, 0.0, 0.0, 1.0);//Black
						glRasterPos2d((-worldWidth / 2.0) + ((xAxisIncrement)) - 10, (-25 * decVal));
						int length = (int)std::strlen(text);
						glListBase(2000);
						glCallLists(length, GL_UNSIGNED_BYTE, text);
						glEnd();

						decVal++;
						token = "";
					}

				}
			}
		}

	}// End of dimesion inclusion check.

}

//drawColorPercentVisualization:
//Desc: Draws a more traditional Nominal set visualization where each class color is present. Still allows dominant class outline with green border.
GLvoid DomNominalSet::drawColorPercentVisualization()
{
	drawColorPercentRectangles(sortedByPurityVector, this->classPercPerBlock, worldWidth);
	drawColorPercentLines(this->worldWidth);
}

//drawColorPercentRectangles:
//Desc: Draws the rectangles with each class color filled by freq.
GLvoid DomNominalSet::drawColorPercentRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth)
{
	int dimensionCount = 0; // Variable for the dimension index.
	int colorChoice = file->getNominalColor();
	glLineWidth(3.0); //Seting line width.
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.0

	//Make a vector to hold positions of blocks.
	vector<unordered_map<double,unordered_map<double, double>>> classColorPositionByBlock;

	//====Draw the rectangles====.
	const int HEIGHT_OF_ALL_BLOCKS = 435;

	//Go over every attribute.
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		//Add the dimension to the vector.
		classColorPositionByBlock.push_back(unordered_map<double, unordered_map<double,double>>());

		//Get current vector (attribute we are working with making blocks):
		vector<pair<double, double>> curVec = sortedByPurityVector[i];

		//Values to calculate where to put values.
		double blockOffsetVertical = (80 + (file->getDimensionShift(i) * (this->worldHeight * 0.5))); //Account for shifting.
		double prevHeight = (80 + (file->getDimensionShift(i) * (this->worldHeight * 0.5)));

		//Iterate over vector to find valus.
		for (int j = 0; j < curVec.size(); j++)
		{
			//Get key and frequency and draw a rectangle.
			double key = curVec[j].first;
			double freq = curVec[j].second;
			double domPerc = 0;
			double dominantClass = -1;
			const double GRAY_VAL = 1;

			//Go over classes and find dominant class:
			//Get the current dimensions class percentages.
			vector<unordered_map<double, double>*>* curDimensionVec = classPercPerBlock->at(i);

			for (int m = 0; m < curDimensionVec->size(); m++)
			{
				unordered_map<double, double>* nextClass = curDimensionVec->at(m);

				if (nextClass->find(key) != nextClass->end())
				{
					if (nextClass->at(key) >= domPerc)
					{
						domPerc = nextClass->at(key);
						dominantClass = m + 1;
					}
				}
			}

			//Get vector for storing positions.
			unordered_map<double, double> currentBlockColorPositions;
			
			//Value to record offset of each color.
			double offSetColor = blockOffsetVertical;

			//Testing:
			int testing = 80;

			//Iterate over classes and draw color perc.
			for (int m = 0; m < curDimensionVec->size(); m++)
			{
				unordered_map<double, double>* nextClass = curDimensionVec->at(m);
				if (nextClass->find(key) != nextClass->end())
				{

					double classPerc = nextClass->at(key);
					double classNum = m + 1;

					double topPixelPosition = (((classPerc * freq) * HEIGHT_OF_ALL_BLOCKS) + (offSetColor));
					double middlePixelPosition = ((((classPerc/2) * freq) * HEIGHT_OF_ALL_BLOCKS) + (offSetColor));

					//Where to draw lines.
					//Block Height:
					double currentBlockHeight = (freq * HEIGHT_OF_ALL_BLOCKS);
					const double INCDEC = (currentBlockHeight / 100);
					if (m == 0)
					{
						//Record the position of this class block.
						currentBlockColorPositions.insert({ classNum, topPixelPosition});
						testing = topPixelPosition;
					}
					else
					{
						//Record the position of this class block.
						currentBlockColorPositions.insert({ classNum, testing});
					}

					//Draw current rectangle.
					glBegin(GL_QUADS);

					//Set Color:
					std::vector<double>* colorOfCurrent = this->file->getClassColor(classNum);
					glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], 0.5);

					// draw bottom left
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						(offSetColor)
					);

					// draw top left
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						(topPixelPosition)
					);

					// draw top right
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						(topPixelPosition)
					);

					// draw bottom right
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						(offSetColor)
					);

					glEnd();

					//===CHECK IF WE NEED GREEN BORDER===
					
					//Get perc from user.
					double percFU = file->getPurityPerc();

					if ((domPerc) >= (percFU / 100) && !(freq <= 0.014) && domPerc == classPerc)
					{
						//==Draw border==:
						glBegin(GL_LINE_STRIP);

						glColor4d(0, 255, 0, GRAY_VAL);
						glLineWidth(.5);

						// draw bottom left
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
							(offSetColor)
						);

						// draw top left
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
							(topPixelPosition)
						);

						// draw top right
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
							(topPixelPosition)
						);

						// draw bottom right
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
							(offSetColor)
						);

						// draw bottom left
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
							(offSetColor + 3)
						);

						glEnd();

					}

					offSetColor = offSetColor + (((classPerc * freq) * HEIGHT_OF_ALL_BLOCKS));
				}
		
			}
			
			//Save the generated values.
			classColorPositionByBlock.at(i).insert({ key, currentBlockColorPositions });

			//Check if we need to draw the border.
			if (!(freq <= 0.014))
			{
				//==Draw border==:
				glBegin(GL_LINE_STRIP);

				glColor4d(0, 0, 0, 1);
				glLineWidth(.5);

				//Top Left Point:
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
				);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
				);

				//Top Right Point:
				glEnd();

			}
			else //We dont need to draw the devider.
			{

				//==Draw Sides==:
				glBegin(GL_LINE_STRIP);

				glColor4d(0, 0, 0, 1);

				glLineWidth(.5);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					(prevHeight - 2)
				);

				glEnd();

				//==Draw Sides==:
				glBegin(GL_LINE_STRIP);

				glColor4d(0, 0, 0, 1);
				glLineWidth(.5);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					(prevHeight - 3)
				);

				glEnd();

				if (j == curVec.size() - 1 || curVec.at(j + 1).second > 0.014)
				{
					//==Draw border==:
					glBegin(GL_LINE_STRIP);

					glColor4d(0, 0, 0, 1);
					glLineWidth(.5);

					//Top Left Point:
					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
					);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
					);

					//Top Right Point:
					glEnd();
				}

			}

			//Record the previous height of the block.
			blockOffsetVertical += (((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)) - prevHeight);
			prevHeight = blockOffsetVertical;

		}

		//==Draw bottom border==:
		glBegin(GL_LINE_STRIP);

		glColor4d(0, 0, 0, 1);
		glLineWidth(.5);

		//Top Left Point:
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
			((80 + (file->getDimensionShift(i) * (this->worldHeight * 0.5)))) //Accounts for shift.
		);

		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
			((80 + (file->getDimensionShift(i) * (this->worldHeight * 0.5)))) //Accounts for shift.
		);

		glEnd();
		//====================


		dimensionCount++;
	}

	this->classColorPositionByBlock = classColorPositionByBlock;
}

//drawColorPercentLines:
//Desc: Draws the lines for the color percent visualization.
GLvoid DomNominalSet::drawColorPercentLines(double worldWidth)
{
	int dimensionCount = 0; // Variable for the dimension index.
	int colorChoice = file->getNominalColor();
	glLineWidth(3.0); //Seting line width.
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.0

	vector<unordered_map<double, unordered_map<double, double>>> classColorPositionByBlock = this->classColorPositionByBlock;

	vector<double> leftCoordinate = vector<double>();
	vector<double> rightCoordinate = vector<double>();
	vector<double> frequency = vector<double>();
	vector<double> classVec = vector<double>();
	vector<int> colorIdx = vector<int>();
	bool alreadyExists = false;
	int numOfLinesSetTransparent = 0;
	int numSmallLines = 0; // count number of lines.

	//Create Array of coordinates and Counts.
	for (int i = 0; i < file->getDimensionAmount() - 1; i++) // file->getDimensionAmount()
	{
		if (!(file->getDataDimensions()->at(i)->isVisible())) continue;
		if (file->getVisibleDimensionCount() < 2) break;

		leftCoordinate.clear();
		rightCoordinate.clear();
		frequency.clear();
		classVec.clear();
		colorIdx.clear();
		alreadyExists = false;

		int j = i + 1;
		while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
		if (j >= file->getDimensionAmount()) continue;

		unordered_map<double, unordered_map<double, double>> leftCoordinateBlocks = classColorPositionByBlock.at(i);
		unordered_map<double, unordered_map<double, double>>  rightCoordinateBlocks = classColorPositionByBlock.at(j);

		//Itterate over sets.
		for (int k = 0; k < this->file->getSetAmount(); k++)
		{
			double left = file->getData(k, i);
			double right = file->getData(k, j);
			double classOfCur = file->getClassOfSet(k);

			double leftPosition = leftCoordinateBlocks.at(left).at(classOfCur);
			double rightPosition = rightCoordinateBlocks.at(right).at(classOfCur);

			// see if exists already, if so increment count;else add and count = 1
			alreadyExists = false;
			for (int l = 0; l < frequency.size(); l++)
			{
				if (leftCoordinate[l] == leftPosition &&
					rightCoordinate[l] == rightPosition && classVec[l] == classOfCur)
				{
					alreadyExists = true;
					frequency[l] = frequency[l] + 1.0;
					break; // break out of l loop
				}
			} // end l loop

			if (!alreadyExists)
			{
				leftCoordinate.push_back(leftPosition);
				rightCoordinate.push_back(rightPosition);
				frequency.push_back(1.0);
				classVec.push_back(classOfCur);
				colorIdx.push_back(k);
			}

		} // end k loop

		// draw the data between these two dimensions, scaling width to count
		for (int k = 0; k < frequency.size(); k++)
		{
			//Keep track of how many small lines are shown.
			numSmallLines += frequency.size();

			int classVal = classVec.at(k);
			vector<double>* color = file->getSetColor(colorIdx[k]);

			double alpha = 1.0;// Alpha here incase we want to use it.

			glColor4d((*color)[0], (*color)[1], (*color)[2], alpha);
			double width = (frequency[k] / (file->getSetAmount(classVal) / 15));
			glLineWidth(width);
			double leftData = leftCoordinate[k];
			double rightData = rightCoordinate[k];

			double leftCoordinateX = (-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1));
			leftCoordinateX = leftCoordinateX * -1;
			double rightCoordinateX = (-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 2));
			rightCoordinateX = rightCoordinateX * -1;
			
			// draw a line using both points
			glBegin(GL_LINE_STRIP);
			glVertex2d(-leftCoordinateX, leftData);
			glVertex2d(-rightCoordinateX, rightData);
			glEnd();
		}

		dimensionCount++;
	}

	//Add the number of lines set transparent to the dominant nominal sets.
	file->setDNSLinesTransparent(numOfLinesSetTransparent);
	file->setDNSNumSmallLines(numSmallLines);
}

//determineRules:
//Desc: Iterates over data and determines logical classification rules to add to the linguistic description.
vector<string> DomNominalSet::determineRules()
{
	vector<string> toReturn;

	//Record the number of rules.
	int ruleCount = 1;

	//Record what cases are in each rule.
	vector<vector<int>>RuleCaseCounts;

	//Iterate over the dimensions.
	for (int i = 0; i < file->getDimensionAmount() - 1; i++)
	{
		//Make sure we don't go out of bounds.
		if (!(file->getDataDimensions()->at(i)->isVisible())) continue;
		if (file->getVisibleDimensionCount() < 2) break;

		int j = i + 1;
		while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
		if (j >= file->getDimensionAmount()) continue;

		//Get Blocks from left and right coordinates.
		vector<unordered_map<double, double>*>* leftCoordinateBlocks = classPercPerBlock->at(i);
		vector<unordered_map<double, double>*>* rightCoordinateBlocks = classPercPerBlock->at(j);

		//Get Cord names:
		string firstCord = *(file->getDimensionName(i));
		string secondCord = *(file->getDimensionName(j));

		//Go over classes and check for rules.
		for (int k = 0; k < file->getClassAmount() - 2; k++)
		{
			//Get class as string
			string classNumber = to_string(k + 1);
			string otherClassNumber = "2";
			if (classNumber == "2")
			{
				otherClassNumber = "1";
			}

			//Get frequencies of this class for each block in both coordinates.
			unordered_map <double, double>* leftCoordinateFreqForCurClass = leftCoordinateBlocks->at(k);
			unordered_map <double, double>* rightCoordinateFreqForCurClass = rightCoordinateBlocks->at(k);

			//Iterate over each block combination.
			for (auto leftCoordIt = leftCoordinateFreqForCurClass->begin(); leftCoordIt != leftCoordinateFreqForCurClass->end(); leftCoordIt++)
			{

				//Count for R3 Condition.
				int r3CountTotal = 0;
				int r3PredictedCorrectly = 0;
				int r3IncorreclyPredicted = 0;
				unordered_map<double, double> r3ClassCounts;
				vector<int>r3Cases;

				//Calculate the precision of the current block.
				double runningTotalPrecision = leftCoordIt->second * 100.0;

				for (auto rightCoordIt = rightCoordinateFreqForCurClass->begin(); rightCoordIt != rightCoordinateFreqForCurClass->end(); rightCoordIt++)
				{
					//Nomralized data:
					double left = leftCoordIt->first;
					double right = rightCoordIt->first;

					double leftFreqForCur = leftCoordIt->second;
					double rightFreqForCur = rightCoordIt->second;

					//===Check if these freqencies satisfy any rules.===

					//R1: Line is going from dominantly current class to dominantly current class (Result: current class):
					if (leftFreqForCur >= 0.85 && rightFreqForCur >= 0.85)
					{
						//Data:
						int correcltyPredicted = 0;
						int incorreclyPredicted = 0;
						int totalPredicted = 0;
						unordered_map<double, double> classCounts;
						vector<int>cases;

						//Determine the number of sets this rule effects.
						for (int r = 0; r < file->getSetAmount(); r++)
						{
							double curLeft = file->getData(r, i);
							double curRight = file->getData(r, j);
							double curClass = file->getClassOfSet(r);

							//If this set is the same data as the current blocks.
							if (curLeft == left && curRight == right && curClass == (k+1))
							{
								correcltyPredicted++;
								totalPredicted++;
								cases.push_back(r);
							}
							else if (curLeft == left && curRight == right && curClass != (k+1))
							{
								incorreclyPredicted;
								totalPredicted++;
								cases.push_back(r);
							}

							//Record numbers of sets in each class.
							if (classCounts.find(curClass) == classCounts.end())
							{
								classCounts.insert({ curClass, 1 });
							}
							else
							{
								double temp = classCounts.at(curClass);
								temp++;
								classCounts.at(curClass) = temp;
							}
						}

						if (totalPredicted != 0)
						{
							//Calculate Precision and Coverage.
							double Precision = (double(correcltyPredicted) / double(totalPredicted)) * 100.0;
							double classCoverage = (double(totalPredicted) / double(classCounts.at(k + 1))) * 100.0;
							double correctClassCoverage = (double(correcltyPredicted) / double(classCounts.at(k + 1))) * 100.0;

							//Record the rule if the precision in increased by combining these blocks.
							if (Precision > (leftFreqForCur * 100.0))
							{
								//Record rule in rule data.
								double leftPosition = 0;
								double rightPosition = 0;

								vector<pair<double, double>> currentVecLeft = sortedVector.at(i);
								for (int c = 0; c < currentVecLeft.size(); c++)
								{
									if (currentVecLeft.at(c).first == left)
									{
										leftPosition = currentVecLeft.at(c).second;
										break;
									}
								}

								vector<pair<double, double>> currentVecRight = sortedVector.at(j);
								for (int c = 0; c < currentVecRight.size(); c++)
								{
									if (currentVecRight.at(c).first == right)
									{
										rightPosition = currentVecRight.at(c).second;
										break;
									}
								}

								pair<double, double> values;
								values.first = leftPosition;
								values.second = rightPosition;

								pair<double, pair<double, double>> toAdd;
								toAdd.first = i;
								toAdd.second = values;

								ruleData.push_back(toAdd);

								RuleCaseCounts.push_back(cases);
								string rule = to_string(ruleCount) + ": If A is in " + firstCord + " in a block dominantly class " + classNumber +
									" and A is in " + secondCord + " in a block dominantly class " + classNumber +
									" then A is class " + classNumber + "\n" +
									"Precision = " + to_string(Precision) + "% \n";

								//Add predictions:
								rule += "Predicted C" + classNumber + " = " + to_string(correcltyPredicted) +
									", Predicted C" + otherClassNumber + " = " + to_string(totalPredicted - correcltyPredicted) + "\n";

								//Add coverages:
								rule += "Class coverage = " + to_string(classCoverage) + "%\n";
								rule += "Correct class coverage = " + to_string(correctClassCoverage) + "%\n";

								rule += "Correcly Predicted = " + to_string(correcltyPredicted) + ", Total Predicted = " + to_string(totalPredicted) + "\n\n";

								ruleCount++;
								toReturn.push_back(rule);
							}

						}

					}

					//R2: Line is going from dominantly current class to dominantly other class (Result: other class):
					if (leftFreqForCur >= 0.85 && rightFreqForCur <= 0.15)
					{
						//Data:
						int correcltyPredicted = 0;
						int incorreclyPredicted = 0;
						int totalPredicted = 0;
						unordered_map<double, double> classCounts;
						vector<int>cases;

						int otherClass = 0;
						if (k == 0)
						{
							otherClass = 1;
						}

						//Determine the number of sets this rule effects.
						for (int r = 0; r < file->getSetAmount(); r++)
						{
							double curLeft = file->getData(r, i);
							double curRight = file->getData(r, j);
							double curClass = file->getClassOfSet(r);

							//If this set is the same data as the current blocks.
							if (curLeft == left && curRight == right && curClass == (otherClass + 1))
							{
								correcltyPredicted++;
								totalPredicted++;
								cases.push_back(r);
							}
							else if (curLeft == left && curRight == right && curClass != (otherClass + 1))
							{
								incorreclyPredicted;
								totalPredicted++;
								cases.push_back(r);
							}

							//Record numbers of sets in each class.
							if (classCounts.find(curClass) == classCounts.end())
							{
								classCounts.insert({ curClass, 1 });
							}
							else
							{
								double temp = classCounts.at(curClass);
								temp++;
								classCounts.at(curClass) = temp;
							}

						}

						if (totalPredicted != 0)
						{
							double Precision = (double(correcltyPredicted) / double(totalPredicted)) * 100.0;
							
							if (Precision > (leftFreqForCur * 100.0))
							{
								//Record rule in rule data.
								double leftPosition = 0;
								double rightPosition = 0;

								vector<pair<double, double>> currentVecLeft = sortedVector.at(i);
								for (int c = 0; c < currentVecLeft.size(); c++)
								{
									if (currentVecLeft.at(c).first == left)
									{
										leftPosition = currentVecLeft.at(c).second;
										break;
									}
								}


								vector<pair<double, double>> currentVecRight = sortedVector.at(j);
								for (int c = 0; c < currentVecRight.size(); c++)
								{
									if (currentVecRight.at(c).first == right)
									{
										rightPosition = currentVecRight.at(c).second;
										break;
									}
								}

								pair<double, double> values;
								values.first = leftPosition;
								values.second = rightPosition;

								pair<double, pair<double, double>> toAdd;
								toAdd.first = i;
								toAdd.second = values;

								ruleData.push_back(toAdd);

								//Calculate Precision and Coverage.
								double classCoverage = (double(totalPredicted) / double(classCounts.at(otherClass + 1))) * 100.0;
								double correctClassCoverage = (double(correcltyPredicted) / double(classCounts.at(otherClass + 1))) * 100.0;
								RuleCaseCounts.push_back(cases);

								string rule = to_string(ruleCount) + ": If A is in " + firstCord + " in a block dominantly class " + classNumber +
									" and A is in " + secondCord + " in a block dominantly class " + otherClassNumber +
									" then A is class " + otherClassNumber + "\n" +
									"Precision = " + to_string(Precision) + "% \n";

								//Add predictions:
								rule += "Predicted C" + classNumber + " = " + to_string(correcltyPredicted) +
									", Predicted C" + otherClassNumber + " = " + to_string(totalPredicted - correcltyPredicted) + "\n";

								//Add coverages:
								rule += "Class coverage = " + to_string(classCoverage) + "%\n";
								rule += "Correct class coverage = " + to_string(correctClassCoverage) + "%\n";

								rule += "Correcly Predicted = " + to_string(correcltyPredicted) + ", Total Predicted = " + to_string(totalPredicted) + "\n\n";

								ruleCount++;
								toReturn.push_back(rule);
							}
						}

					}

					//R3: If line is going from dominantly current class to not dominantly other class. (Result: Current Class)
					if (leftFreqForCur >= 0.85 && rightFreqForCur > 0.15)
					{
						double currentPairTotal = 0;
						double currentPairCorrect = 0;
						double currentPairIncorrect = 0;
						vector<int> currentPairCases;

						//Record for these left and right values.
						for (int r = 0; r < file->getSetAmount(); r++)
						{

							int x = file->getSetAmount();
							double curLeft = file->getData(r, i);
							double curRight = file->getData(r, j);
							double curClass = file->getClassOfSet(r);

							//If this set is the same data as the current blocks.
							if (curLeft == left && curRight == right && curClass == (k + 1))
							{
								currentPairCorrect++;
								currentPairTotal++;
								currentPairCases.push_back(r);
							}
							else if (curLeft == left && curRight == right && curClass != (k + 1))
							{
								currentPairIncorrect++;
								currentPairTotal++;
								currentPairCases.push_back(r);
							}

						}

						//Check to see if this rule will positivly effect the overall precision:
						//Calculate combinded precision:
						double combinedCorrect = r3PredictedCorrectly + currentPairCorrect;
						double combinedTotal = r3CountTotal + currentPairTotal;

						//Make sure any cases were predicted to avoid dbz.
						if (combinedTotal != 0)
						{
							double combinedPrecision = (combinedCorrect / combinedTotal) * 100.0;

							//If this pair make the precision increase, then add the values to it.
							if (combinedPrecision > runningTotalPrecision)
							{
								r3PredictedCorrectly += currentPairCorrect;
								r3IncorreclyPredicted += currentPairIncorrect;
								r3CountTotal += currentPairTotal;

								for (int m = 0; m < currentPairCases.size(); m++)
								{
									r3Cases.push_back(currentPairCases.at(m));
								}

								//Record rule in rule data.
								pair<double, double> values;

								double leftPosition = 0;
								double rightPosition = 0;
								double dominantLeft = -1;
								double dominantRight = -1;

								//Find what the dominant class is for left and right data
								vector<pair<double, double>>leftDC = domClass.at(i);
								for (int c = 0; c < leftDC.size(); c++)
								{
									pair<double, double> p = leftDC.at(c);

									if (p.first == left)
									{
										dominantLeft = p.second;
										break;
									}
								}

								vector<pair<double, double>>rightDC = domClass.at(j);
								for (int c = 0; c < rightDC.size(); c++)
								{
									pair<double, double> p = rightDC.at(c);

									if (p.first == right)
									{
										dominantRight = p.second;
										break;
									}
								}

								if (dominantLeft == k + 1)
								{
									//Determine the positions.
									vector<pair<double, double>> currentVecLeft = sortedVector.at(i);
									for (int c = 0; c < currentVecLeft.size(); c++)
									{
										if (currentVecLeft.at(c).first == left)
										{
											leftPosition = currentVecLeft.at(c).second;
											break;
										}
									}
								}
								else
								{
									vector<pair<double, double>> currentVecLeft = middleOther.at(i);
									for (int c = 0; c < currentVecLeft.size(); c++)
									{
										if (currentVecLeft.at(c).first == right)
										{
											leftPosition = currentVecLeft.at(c).second;
											break;
										}
									}
								}

								if (dominantRight == k + 1)
								{
									//Determine the positions.
									vector<pair<double, double>> currentVecRight = sortedVector.at(j);
									for (int c = 0; c < currentVecRight.size(); c++)
									{
										if (currentVecRight.at(c).first == right)
										{
											rightPosition = currentVecRight.at(c).second;
											break;
										}
									}
								}
								else
								{
									vector<pair<double, double>> currentVecRight = middleOther.at(j);
									for (int c = 0; c < currentVecRight.size(); c++)
									{
										if (currentVecRight.at(c).first == right)
										{
											rightPosition = currentVecRight.at(c).second;
											break;
										}
									}
								}

								values.first = leftPosition;
								values.second = rightPosition;

								pair<double, pair<double, double>> toAdd;
								toAdd.first = i;
								toAdd.second = values;

								ruleData.push_back(toAdd);
							}
						}

					}
				}

				//Record rule for R3:
				if (r3CountTotal != 0)
				{
					RuleCaseCounts.push_back(r3Cases);
					//find how many sets are in each class.
					for (int r = 0; r < file->getSetAmount(); r++)
					{
						double curClass = file->getClassOfSet(r);

						//Record numbers of sets in each class.
						if (r3ClassCounts.find(curClass) == r3ClassCounts.end())
						{
							r3ClassCounts.insert({ curClass, 1 });
						}
						else
						{
							double temp = r3ClassCounts.at(curClass);
							temp++;
							r3ClassCounts.at(curClass) = temp;
						}

					}

					//Calculate Precision and Coverage.
					double Precision = (double(r3PredictedCorrectly) / double(r3CountTotal)) * 100.0;
							double classCoverage = (double(r3CountTotal) / double(r3ClassCounts.at(k + 1))) * 100.0;
							double correctClassCoverage = (double(r3PredictedCorrectly) / double(r3ClassCounts.at(k + 1))) * 100.0;

					string newRule = to_string(ruleCount) + ": If A is in " + firstCord + " in a block dominantly class " + classNumber +
						" and A is not in " + secondCord + " in a block dominantly class " + otherClassNumber +
						" then A is class " + classNumber + "\n" + 
						"Precision = " + to_string(Precision) + "% \n";

					//Add predictions:
					newRule += "Predicted C" + classNumber + " = " + to_string(r3PredictedCorrectly) +
						", Predicted C" + otherClassNumber + " = " + to_string(r3CountTotal - r3PredictedCorrectly) + "\n";

					//Add coverages:
					newRule += "Class coverage = " + to_string(classCoverage) + "%\n";
					newRule += "Correct class coverage = " + to_string(correctClassCoverage) + "%\n";

					newRule += "Correcly Predicted = " + to_string(r3PredictedCorrectly) + ", Total Predicted = " + to_string(r3CountTotal) + "\n\n";

					ruleCount++;
					toReturn.push_back(newRule);
				}

			}
		}

	}

	/*
	//Calculate overlap percentage and unique cases #.
	double overlapTable[100][100];
	int numUniqueCases[100][100];

	//Open File:
	SaveFileDialog^ sfd = gcnew SaveFileDialog;
	sfd->InitialDirectory = ""; // NOTE "c:\\" for future reference.
	sfd->Filter = "Text Files (*.csv, *.txt, *text)|*.csv, *txt, *text|All Files (*.*)|*.*";
	sfd->FilterIndex = 1;
	sfd->RestoreDirectory = true;

	System::String^ path;

	if (sfd->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		path = sfd->FileName;
	}

	//Write Data:
	std::ofstream outFile;
	std::string outDir = msclr::interop::marshal_as<std::string>(path);
	std::string temp = "";

	outFile.open(outDir);

	//Add top lables:
	outFile << " ,";
	for (int i = 1; i < ruleCount; i++)
	{
		outFile << msclr::interop::marshal_as<std::string>("Rule " + i + ",");
	}
	outFile << "\n";

	//Itterate over rows:
	for (int i = 0; i < ruleCount - 1; i++)
	{	
		vector<int>columnCases = RuleCaseCounts.at(i);

		outFile << "Rule " + to_string(i+1) + ",";

		for (int j = 0; j < ruleCount - 1; j++)
		{
			
			vector<int>rowCases = RuleCaseCounts.at(j);

			int overlapCount = 0;

			//Go over column cases and check if they are in the row cases.
			for (int k = 0; k < columnCases.size(); k++)
			{
				for (int m = 0; m < rowCases.size(); m++)
				{
					if (rowCases.at(m) == columnCases.at(k))
					{
						overlapCount++;
					}
				}
			}

			//Calculate how many total cases are in both rules.
			int totalCases = rowCases.size() + columnCases.size() - (overlapCount);

			//Calculate the remaining cases after removed overlap:
			int remainingCases = rowCases.size() + columnCases.size() - (overlapCount * 2);

			//Record the number Unique Cases.
			numUniqueCases[i][j] = totalCases;

			//Record percent.
			if (remainingCases != 0)
			{
				overlapTable[i][j] = (double(overlapCount) / double(totalCases)) * 100.0;
			}
			else
			{
				overlapTable[i][j] = -1;
			}

			//Calculate the number of values if just one rule is used.
			int columnOnly = columnCases.size() - overlapCount;
			int rowOnly = rowCases.size() - overlapCount;

			outFile << "" + to_string(overlapTable[i][j]) + " | " + to_string(totalCases) + 
				" | R:" + to_string(columnOnly) +" | C:"+ to_string(rowOnly) +" | OL:" + to_string(overlapCount) +" ,";
		}

		outFile << "\n";
	}

	//Write to the file:
	outFile.close();

	*/

	return toReturn;
}

//ruleGenerationSequential:
//Desc: Generates rules sequentially so that no rules overlap with a goal to get highest coverage.
vector<string> DomNominalSet::ruleGenerationSequential()
{
	ruleData.clear();//Clear the previous rule data if any.

	//Consts:
	const double FREQ_THRESHOLD = 0.85; //Block frequency for rule generation.
	const double PRECISION_THRESHOLD = 95.0; //Threashold for keeping a rule. 

	vector<string> toReturn; //Vector to hold the rules after generating them.
	vector<int> casesAlreadyUsed; //Vector to hold the cases that are already conained in other rules.
	vector<int> currentCasesAlreadyUsed; //Vector to hold the cases that are in the best current rule.
	double highestPrecision = 0; //Var to keep track of highest precision.
	double highestCoverage = 0; //Var to keep track of highest coverage.
	pair<double, pair<double, double>> currentBestRuleData; //Data recorded for best rule to be visualized.
	currentBestRuleData.first = -1; //Marking unused.
	string currentBestRuleGenerated = ""; //Holds the best rule generated.
	int ruleCount = 1; //Keep track of what number of rule we are on.
	bool ruleGenerated = true;
	int numRulesGenerated = 0;
	unordered_map<int, string> rulesByCoordinate;
	vector<vector<int>>RuleCaseCounts; //Record what cases are in each rule (Overlap Table).
	
	//=====Rule Generation=====//

	//While there are still cases to make a rule out of and we previously created a rule.
	while (casesAlreadyUsed.size() < file->getSetAmount() && ruleGenerated == true)
	{
		ruleGenerated = false;//Set flag to false.

		//Iterate over the dimensions.
		for (int i = 0; i < file->getDimensionAmount() - 1; i++)
		{
			//Make sure we don't go out of bounds and dimensions are visible.
			if (!(file->getDataDimensions()->at(i)->isVisible())) continue;
			if (file->getVisibleDimensionCount() < 2) break;
			int j = i + 1;
			while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
			if (j >= file->getDimensionAmount()) continue;

			//Get class frequency per block per coordinate for our current left and right coordinates.
			vector<unordered_map<double, double>*>* leftCoordinateBlocks = classPercPerBlock->at(i);
			vector<unordered_map<double, double>*>* rightCoordinateBlocks = classPercPerBlock->at(j);

			//Get coordinate names as strings.
			string firstCord = *(file->getDimensionName(i));
			string secondCord = *(file->getDimensionName(j));

			//Iterate over the number of classes there are and try to determine rules for each class.
			for (int k = 1; k < file->getClassAmount() - 1; k++) // -2 for default and 'class' 
			{
				//Record class numbers as strings.
				string currentClassAsString = to_string(k);
				int currentClassAsInt = k;
				vector<string> otherClassesAsStrings;
				vector<int> otherClassesAsInts;
				for (int m = 1; m < file->getClassAmount() - 1; m++)
				{
					if (m == k) continue;
					otherClassesAsStrings.push_back(to_string(m));
					otherClassesAsInts.push_back(m);
				}

				//Get frequencies for the left and rights coordinates blocks for the class we are working with.
				unordered_map <double, double>* leftCoordinateBlocksFreqsForCurClass = leftCoordinateBlocks->at(k - 1);
				unordered_map <double, double>* rightCoordinateBlocksFreqsForCurClass = rightCoordinateBlocks->at(k - 1);

				//Iterate over each block combination between the two cooridnates and check if there are rules.
				for (auto leftCoordinateIt = leftCoordinateBlocksFreqsForCurClass->begin();
					leftCoordinateIt != leftCoordinateBlocksFreqsForCurClass->end(); leftCoordinateIt++)
				{
					for (auto rightCoordinateIt = rightCoordinateBlocksFreqsForCurClass->begin();
						rightCoordinateIt != rightCoordinateBlocksFreqsForCurClass->end(); rightCoordinateIt++)
					{
						//Bool to check if the rule cannot be used.
						bool ruleIncludesCaseAlreadyUsed = false;

						//Normalized Attribute Values:
						double leftAttributeVal = leftCoordinateIt->first;
						double rightAttributeVal = rightCoordinateIt->first;

						//Frequency of this current class (k) in the left and right block pair.
						double freqOfCurClassLeft = leftCoordinateIt->second;
						double freqOfCurClassRight = rightCoordinateIt->second;

						//===Check to see if these frequencies pass any conditions===//

						//Condition 1 - dominantly current class to dominantly current class. Result - current class:
						if (freqOfCurClassLeft >= FREQ_THRESHOLD && freqOfCurClassRight >= FREQ_THRESHOLD)
						{
							//Values to hold correctly predicted, incorrectly predicted, and total predicted.
							int correctlyPredicted = 0;
							int incorreclyPredicted = 0;
							int totalPredicted = 0;
							int correctlyPredictedFirst = 0;//Value for only single element.
							int totalPredictedFirst = 0;//Value for only single element.
							vector<int> numberOfCasesPerClass;
							vector<int> casesInRule;

							//Fill the vector with the number of classes.
							for (int m = 0; m < file->getClassAmount() - 2; m++)
							{
								numberOfCasesPerClass.push_back(0);
							}

							//Determine what cases satisfy this condition and how many are predicted correctly and incorrectly.
							for (int m = 0; m < file->getSetAmount(); m++)
							{
								//Check to see if the case has been used in a previous rule, flag.
								bool hasBeenUsed = false;

								//Get data about currrent set.
								double curSetLeft = file->getData(m, i);
								double curSetRight = file->getData(m, j);
								int curSetClass = file->getClassOfSet(m);

								//Check if the value is in the first element to be able to check if both attributes together is better
								//than just the first attibute.
								if (curSetLeft == leftAttributeVal && curSetClass == currentClassAsInt)
								{
									correctlyPredictedFirst++;
									totalPredictedFirst++;
								}
								else
								{
									totalPredictedFirst++;
								}

								//If the attribute values are the same and the class is the expected result.
								if (curSetLeft == leftAttributeVal && curSetRight == rightAttributeVal &&
									curSetClass == currentClassAsInt)
								{
									correctlyPredicted++;
									totalPredicted++;
									casesInRule.push_back(m);

									//Check if current case has already been used.
									for (int n = 0; n < casesAlreadyUsed.size(); n++)
									{
										if (m == casesAlreadyUsed.at(n))
										{
											hasBeenUsed = true;
											break;
										}
									}

								}
								//If the attribute values are the same and the class is not the expected result.
								else if (curSetLeft == leftAttributeVal && curSetRight == rightAttributeVal &&
									curSetClass != currentClassAsInt)
								{
									incorreclyPredicted++;
									totalPredicted++;
									casesInRule.push_back(m);

									//Check if current case has already been used.
									for (int n = 0; n < casesAlreadyUsed.size(); n++)
									{
										if (m == casesAlreadyUsed.at(n))
										{
											hasBeenUsed = true;
											break;
										}
									}
								}

								//Record the number of sets in each class to be able to in each class.
								numberOfCasesPerClass.at(int(curSetClass) - 1) += 1;

								//If a case in this rule has been used before, then we cannot include it.
								if (hasBeenUsed)
								{
									ruleIncludesCaseAlreadyUsed = true;
									break;
								}

							}//End of iteration over sets.

							//If the rule includes a case that is in an already generated rule, continue to try next combination.
							if (ruleIncludesCaseAlreadyUsed)
							{
								ruleIncludesCaseAlreadyUsed = false;
								continue;
							}

							//Check to see if the precision is above the prevision requirement and the rules precision / coverage is greater
							//then the current best rule generated.
							if (totalPredicted != 0)
							{
								//Calculate precision of only first attribute and pir of attributes.
								double precision = (double(correctlyPredicted) / double(totalPredicted)) * 100.0;
								double precisionFirst = (double(correctlyPredictedFirst) / double(totalPredictedFirst)) * 100.0;

								//If the second attribute inclusion increases precision.
								if (precision > precisionFirst)
								{

									if (precision >= PRECISION_THRESHOLD)
									{
										numRulesGenerated++;
									}

									double totalCoverage = (double(totalPredicted) / double(file->getSetAmount())) * 100.0;
									if (precision >= PRECISION_THRESHOLD && precision >= highestPrecision && totalCoverage >= highestCoverage)
									{
										//Compute other values needed for rule description.
										double classCoverage = (double(correctlyPredicted) / double(numberOfCasesPerClass.at(currentClassAsInt - 1))) * 100.0;
										double correctClassCoverage = (double(correctlyPredicted) / double(numberOfCasesPerClass.at(currentClassAsInt - 1))) * 100.0;

										//Make this rule the new optimal rule generated.
										string ruleStatement = "If set A goes to a block in " + firstCord + " that is dominantly class " + currentClassAsString +
											" and A goes to a block in " + secondCord + " that is dominantly class " + currentClassAsString + " then set A is" +
											" class: " + currentClassAsString + ". \n" +
											"Predicted correctly: " + to_string(correctlyPredicted) + ", predicted incorrectly: " + to_string(incorreclyPredicted) + ". \n" +
											"Total predicted: " + to_string(totalPredicted) + ", Precision = " + to_string(precision) + "%. \n" +
											"Class Coverage = " + to_string(classCoverage) + "%, Correct Class Coverage = " + to_string(correctClassCoverage) + "%. \n" +
											"Total Coverage = " + to_string(totalCoverage) + "%. \n" +
											"Left: " + to_string(leftAttributeVal) + " Right: " + to_string(rightAttributeVal) + "\n\n";

										//Record the current highest precision, current highest coverage, and current cases covered.
										highestPrecision = precision;
										highestCoverage = totalCoverage;
										currentCasesAlreadyUsed = casesInRule;
										currentBestRuleGenerated = ruleStatement;
										ruleGenerated = true; // Set flag that a rule was generated.

										//Determine the position if this line for rule visuazliation and save it under the current best rule.
										double leftPosition = 0;
										double rightPosition = 0;
										vector<pair<double, double>> currentVecRight = sortedVector.at(j);
										vector<pair<double, double>> currentVecLeft = sortedVector.at(i);
										for (int c = 0; c < currentVecLeft.size(); c++)
										{
											if (currentVecLeft.at(c).first == leftAttributeVal)
											{
												leftPosition = currentVecLeft.at(c).second;
												break;
											}
										}
										for (int c = 0; c < currentVecRight.size(); c++)
										{
											if (currentVecRight.at(c).first == rightAttributeVal)
											{
												rightPosition = currentVecRight.at(c).second;
												break;
											}
										}

										pair<double, double> values;
										values.first = leftPosition;
										values.second = rightPosition;
										pair<double, pair<double, double>> toAdd;
										toAdd.first = i;
										toAdd.second = values;
										currentBestRuleData = toAdd;

									}
								}
								
							}//End of total predicted != 0.

						}//End of Condition 1.

						//Condition 2 - dominantly current class to dominantly other class. Result - other class:
						if (freqOfCurClassLeft >= FREQ_THRESHOLD && freqOfCurClassRight <= (1 - FREQ_THRESHOLD))
						{
							//We need to check this for each class that is not the current class.
							for (int q = 0; q < otherClassesAsInts.size(); q++)
							{
								//If the current 'other' class is the class we are currently using, skip.
								if (otherClassesAsInts.at(q) == currentClassAsInt) continue;

								//Get the number of the other class.
								int otherClassAsInt = otherClassesAsInts.at(q);
								string otherClassAsString = to_string(otherClassAsInt);

								//Values to hold correctly predicted, incorrectly predicted, and total predicted.
								int correctlyPredicted = 0;
								int incorreclyPredicted = 0;
								int totalPredicted = 0;
								int correctlyPredictedFirst = 0;//Value for first attribute.
								int totalPredictedFirst = 0;//Value for first attribute.
								vector<int> numberOfCasesPerClass;
								vector<int> casesInRule;

								//Fill the vector with the number of classes.
								for (int m = 0; m < file->getClassAmount() - 2; m++)
								{
									numberOfCasesPerClass.push_back(0);
								}

								//Determine what cases satisfy this condition and how many are predicted correctly and incorrectly.
								for (int m = 0; m < file->getSetAmount(); m++)
								{
									//Check to see if the case has been used in a previous rule, flag.
									bool hasBeenUsed = false;

									//Get data about currrent set.
									double curSetLeft = file->getData(m, i);
									double curSetRight = file->getData(m, j);
									int curSetClass = file->getClassOfSet(m);

									//Check if the value is in the first element to be able to check if both attributes together is better
									//than just the first attibute.
									if (curSetLeft == leftAttributeVal && curSetClass == otherClassAsInt)
									{
										correctlyPredictedFirst++;
										totalPredictedFirst++;
									}
									else
									{
										totalPredictedFirst++;
									}

									//If the attribute values are the same and the class is the expected result.
									if (curSetLeft == leftAttributeVal && curSetRight == rightAttributeVal &&
										curSetClass == otherClassAsInt)
									{
										correctlyPredicted++;
										totalPredicted++;
										casesInRule.push_back(m);

										//Check if current case has already been used.
										for (int n = 0; n < casesAlreadyUsed.size(); n++)
										{
											if (m == casesAlreadyUsed.at(n))
											{
												hasBeenUsed = true;
												break;
											}
										}

									}
									//If the attribute values are the same and the class is not the expected result.
									else if (curSetLeft == leftAttributeVal && curSetRight == rightAttributeVal &&
										curSetClass != otherClassAsInt)
									{
										incorreclyPredicted++;
										totalPredicted++;
										casesInRule.push_back(m);

										//Check if current case has already been used.
										for (int n = 0; n < casesAlreadyUsed.size(); n++)
										{
											if (m == casesAlreadyUsed.at(n))
											{
												hasBeenUsed = true;
												break;
											}
										}
									}

									//Record the number of sets in each class to be able to in each class.
									numberOfCasesPerClass.at(int(curSetClass) - 1) += 1;

									//If a case in this rule has been used before, then we cannot include it.
									if (hasBeenUsed)
									{
										ruleIncludesCaseAlreadyUsed = true;
										break;
									}

								}//End of iteration over sets.
							
								//If the rule includes a case that is in an already generated rule, continue to try next combination.
								if (ruleIncludesCaseAlreadyUsed)
								{
									ruleIncludesCaseAlreadyUsed = false;
									continue;
								}

								//Check to see if the precision is above the prevision requirement and the rules precision / coverage is greater
								//then the current best rule generated.
								if (totalPredicted != 0)
								{
									double precision = (double(correctlyPredicted) / double(totalPredicted)) * 100.0;
									double precisionFirst = (double(correctlyPredictedFirst) / double(totalPredictedFirst)) * 100.0;
								
									//If the precision of only the first attribute is less then the two combined.
									if (precision > precisionFirst)
									{

										if (precision >= PRECISION_THRESHOLD)
										{
											numRulesGenerated++;
										}

										double totalCoverage = (double(totalPredicted) / double(file->getSetAmount())) * 100.0;
										if (precision >= PRECISION_THRESHOLD && precision >= highestPrecision && totalCoverage >= highestCoverage)
										{
											//Compute other values needed for rule description.
											double classCoverage = (double(correctlyPredicted) / double(numberOfCasesPerClass.at(otherClassAsInt - 1))) * 100.0;
											double correctClassCoverage = (double(correctlyPredicted) / double(numberOfCasesPerClass.at(otherClassAsInt - 1))) * 100.0;

											//Make this rule the new optimal rule generated.
											string ruleStatement = "If set A goes to a block in " + firstCord + " that is dominantly class " + currentClassAsString +
												" and A goes to a block in " + secondCord + " that is dominantly class " + otherClassAsString + " then set A is" +
												" class: " + otherClassAsString + ". \n" +
												"Predicted correctly: " + to_string(correctlyPredicted) + ", predicted incorrectly: " + to_string(incorreclyPredicted) + ". \n" +
												"Total predicted: " + to_string(totalPredicted) + ", Precision = " + to_string(precision) + "%. \n" +
												"Class Coverage = " + to_string(classCoverage) + "%, Correct Class Coverage = " + to_string(correctClassCoverage) + "%. \n" +
												"Total Coverage = " + to_string(totalCoverage) + "%. \n" +
												"Left: " + to_string(leftAttributeVal) + " Right: " + to_string(rightAttributeVal) + "\n\n";

											//Record the current highest precision, current highest coverage, and current cases covered.
											highestPrecision = precision;
											highestCoverage = totalCoverage;
											currentCasesAlreadyUsed = casesInRule;
											currentBestRuleGenerated = ruleStatement;
											ruleGenerated = true; // Set flag that a rule was generated.

											//Determine the position of the rule line and save it under the current best rule data.
											double leftPosition = 0;
											double rightPosition = 0;

											vector<pair<double, double>> currentVecRight = middleOther.at(j);
											vector<pair<double, double>> currentVecLeft = sortedVector.at(i);
											for (int c = 0; c < currentVecLeft.size(); c++)
											{
												if (currentVecLeft.at(c).first == leftAttributeVal)
												{
													leftPosition = currentVecLeft.at(c).second;
													break;
												}
											}
											for (int c = 0; c < currentVecRight.size(); c++)
											{
												if (currentVecRight.at(c).first == rightAttributeVal)
												{
													rightPosition = currentVecRight.at(c).second;
													break;
												}
											}

											pair<double, double> values;
											values.first = leftPosition;
											values.second = rightPosition;
											pair<double, pair<double, double>> toAdd;
											toAdd.first = i;
											toAdd.second = values;
											currentBestRuleData = toAdd;

										}
									}

								}//End of total predicted != 0.

							}//End of iteration over other classes.

						}//End of Condition 2.

						//Condition 3 - dominantly current class to not dominantly other class. Result - current class class:
						if (freqOfCurClassLeft >= FREQ_THRESHOLD && freqOfCurClassRight > (1 - FREQ_THRESHOLD))
						{
							//Values to hold correctly predicted, incorrectly predicted, and total predicted.
							int correctlyPredicted = 0;
							int incorreclyPredicted = 0;
							int totalPredicted = 0;
							int correctlyPredictedFirst = 0;//Value for first attribute.
							int totalPredictedFirst = 0;//Value for first attribute.
							vector<int> numberOfCasesPerClass;
							vector<int> casesInRule;

							//Fill the vector with the number of classes.
							for (int m = 0; m < file->getClassAmount() - 2; m++)
							{
								numberOfCasesPerClass.push_back(0);
							}

							//Determine what cases satisfy this condition and how many are predicted correctly and incorrectly.
							for (int m = 0; m < file->getSetAmount(); m++)
							{
								//Check to see if the case has been used in a previous rule, flag.
								bool hasBeenUsed = false;

								//Get data about currrent set.
								double curSetLeft = file->getData(m, i);
								double curSetRight = file->getData(m, j);
								int curSetClass = file->getClassOfSet(m);


								//Check if the value is in the first element to be able to check if both attributes together is better
								//than just the first attibute.
								if (curSetLeft == leftAttributeVal && curSetClass == currentClassAsInt)
								{
									correctlyPredictedFirst++;
									totalPredictedFirst++;
								}
								else
								{
									totalPredictedFirst++;
								}

								//If the attribute values are the same and the class is the expected result.
								if (curSetLeft == leftAttributeVal && curSetRight == rightAttributeVal &&
									curSetClass == currentClassAsInt)
								{
									correctlyPredicted++;
									totalPredicted++;
									casesInRule.push_back(m);

									//Check if current case has already been used.
									for (int n = 0; n < casesAlreadyUsed.size(); n++)
									{
										if (m == casesAlreadyUsed.at(n))
										{
											hasBeenUsed = true;
											break;
										}
									}

								}
								//If the attribute values are the same and the class is not the expected result.
								else if (curSetLeft == leftAttributeVal && curSetRight == rightAttributeVal &&
									curSetClass != currentClassAsInt)
								{
									incorreclyPredicted++;
									totalPredicted++;
									casesInRule.push_back(m);

									//Check if current case has already been used.
									for (int n = 0; n < casesAlreadyUsed.size(); n++)
									{
										if (m == casesAlreadyUsed.at(n))
										{
											hasBeenUsed = true;
											break;
										}
									}
								}

								//Record the number of sets in each class to be able to in each class.
								numberOfCasesPerClass.at(int(curSetClass) - 1) += 1;

								//If a case in this rule has been used before, then we cannot include it.
								if (hasBeenUsed)
								{
									ruleIncludesCaseAlreadyUsed = true;
									break;
								}

							}//End of iteration over sets.

							//If the rule includes a case that is in an already generated rule, continue to try next combination.
							if (ruleIncludesCaseAlreadyUsed)
							{
								ruleIncludesCaseAlreadyUsed = false;
								continue;
							}

							//Check to see if the precision is above the prevision requirement and the rules precision / coverage is greater
							//then the current best rule generated.
							if (totalPredicted != 0)
							{
								double precision = (double(correctlyPredicted) / double(totalPredicted)) * 100.0;
								double precisionFirst = (double(correctlyPredictedFirst) / double(totalPredictedFirst)) * 100.0;

								//If the combined attribute precision is greater then the first.
								if (precision > precisionFirst)
								{

									if (precision >= PRECISION_THRESHOLD)
									{
										numRulesGenerated++;
									}

									double totalCoverage = (double(totalPredicted) / double(file->getSetAmount())) * 100.0;
									if (precision >= PRECISION_THRESHOLD && precision >= highestPrecision && totalCoverage >= highestCoverage)
									{
										//Compute other values needed for rule description.
										double classCoverage = (double(correctlyPredicted) / double(numberOfCasesPerClass.at(currentClassAsInt - 1))) * 100.0;
										double correctClassCoverage = (double(correctlyPredicted) / double(numberOfCasesPerClass.at(currentClassAsInt - 1))) * 100.0;

										//Get other classes as a list of strings.
										string otherClassesAsList = "";
										for (int m = 0; m < otherClassesAsStrings.size() - 1; m++)
										{
											otherClassesAsList += otherClassesAsStrings.at(m) + ", ";
										}
										otherClassesAsList += otherClassesAsStrings.at(otherClassesAsStrings.size() - 1);

										//Make this rule the new optimal rule generated.
										string ruleStatement = "If set A goes to a block in " + firstCord + " that is dominantly class " + currentClassAsString +
											" and A does not go to a block in " + secondCord + " that is dominantly class " + otherClassesAsList + " then set A is" +
											" class: " + currentClassAsString + ". \n" +
											"Predicted correctly: " + to_string(correctlyPredicted) + ", predicted incorrectly: " + to_string(incorreclyPredicted) + ". \n" +
											"Total predicted: " + to_string(totalPredicted) + ", Precision = " + to_string(precision) + "%. \n" +
											"Class Coverage = " + to_string(classCoverage) + "%, Correct Class Coverage = " + to_string(correctClassCoverage) + "%. \n" +
											"Total Coverage = " + to_string(totalCoverage) + "%. \n" +
											"Left: " + to_string(leftAttributeVal) + " Right: " + to_string(rightAttributeVal) + "\n\n";

										//Record the current highest precision, current highest coverage, and current cases covered.
										highestPrecision = precision;
										highestCoverage = totalCoverage;
										currentCasesAlreadyUsed = casesInRule;
										currentBestRuleGenerated = ruleStatement;
										ruleGenerated = true; // Set flag that a rule was generated.

										//Determine the position of the rule line and save it under the current best rule data.
										double leftPosition = 0;
										double rightPosition = 0;

										vector<pair<double, double>> currentVecRight = middleOther.at(j);
										vector<pair<double, double>> currentVecLeft = sortedVector.at(i);
										for (int c = 0; c < currentVecLeft.size(); c++)
										{
											if (currentVecLeft.at(c).first == leftAttributeVal)
											{
												leftPosition = currentVecLeft.at(c).second;
												break;
											}
										}
										for (int c = 0; c < currentVecRight.size(); c++)
										{
											if (currentVecRight.at(c).first == rightAttributeVal)
											{
												rightPosition = currentVecRight.at(c).second;
												break;
											}
										}

										pair<double, double> values;
										values.first = leftPosition;
										values.second = rightPosition;
										pair<double, pair<double, double>> toAdd;
										toAdd.first = i;
										toAdd.second = values;
										currentBestRuleData = toAdd;

									}

								}

							}//End of total predicted != 0.

						}//End of Condition 3.

						//=====SINGLE ATTRIBUTE RULES=====//
						//Condition 4 - Left attribute val goes through a block dominantly current class. Result - currentClass:
						if (freqOfCurClassLeft >= FREQ_THRESHOLD)
						{
							//Values to hold correctly predicted, incorrectly predicted, and total predicted.
							int correctlyPredicted = 0;
							int incorreclyPredicted = 0;
							int totalPredicted = 0;
							vector<int> numberOfCasesPerClass;
							vector<int> casesInRule;

							//Fill the vector with the number of classes.
							for (int m = 0; m < file->getClassAmount() - 2; m++)
							{
								numberOfCasesPerClass.push_back(0);
							}

							//Determine what cases satisfy this condition and how many are predicted correctly and incorrectly.
							for (int m = 0; m < file->getSetAmount(); m++)
							{
								//Check to see if the case has been used in a previous rule, flag.
								bool hasBeenUsed = false;

								//Get data about currrent set.
								double curSetLeft = file->getData(m, i);
								double curSetRight = file->getData(m, j);
								int curSetClass = file->getClassOfSet(m);

								//If the attribute values are the same and the class is the expected result.
								if (curSetLeft == leftAttributeVal && curSetClass == currentClassAsInt)
								{
									correctlyPredicted++;
									totalPredicted++;
									casesInRule.push_back(m);

									//Check if current case has already been used.
									for (int n = 0; n < casesAlreadyUsed.size(); n++)
									{
										if (m == casesAlreadyUsed.at(n))
										{
											hasBeenUsed = true;
											break;
										}
									}

								}
								//If the attribute values are the same and the class is not the expected result.
								else if (curSetLeft == leftAttributeVal && curSetClass != currentClassAsInt)
								{
									incorreclyPredicted++;
									totalPredicted++;
									casesInRule.push_back(m);

									//Check if current case has already been used.
									for (int n = 0; n < casesAlreadyUsed.size(); n++)
									{
										if (m == casesAlreadyUsed.at(n))
										{
											hasBeenUsed = true;
											break;
										}
									}
								}

								//Record the number of sets in each class to be able to in each class.
								numberOfCasesPerClass.at(int(curSetClass) - 1) += 1;

								//If a case in this rule has been used before, then we cannot include it.
								if (hasBeenUsed)
								{
									ruleIncludesCaseAlreadyUsed = true;
									break;
								}

							}//End of iteration over sets.

							//If the rule includes a case that is in an already generated rule, continue to try next combination.
							if (ruleIncludesCaseAlreadyUsed)
							{
								ruleIncludesCaseAlreadyUsed = false;
								continue;
							}

							//Check to see if the precision is above the prevision requirement and the rules precision / coverage is greater
							//then the current best rule generated.
							if (totalPredicted != 0)
							{
								
								double precision = (double(correctlyPredicted) / double(totalPredicted)) * 100.0;
								double totalCoverage = (double(totalPredicted) / double(file->getSetAmount())) * 100.0;

								if (precision >= PRECISION_THRESHOLD)
								{
									numRulesGenerated++;
								}

								if (precision >= PRECISION_THRESHOLD && precision >= highestPrecision && totalCoverage >= highestCoverage)
								{
									//Compute other values needed for rule description.
									double classCoverage = (double(correctlyPredicted) / double(numberOfCasesPerClass.at(currentClassAsInt - 1))) * 100.0;
									double correctClassCoverage = (double(correctlyPredicted) / double(numberOfCasesPerClass.at(currentClassAsInt - 1))) * 100.0;

									//Make this rule the new optimal rule generated.
									string ruleStatement = "If set A goes to a block in " + firstCord + " that is dominantly class " + currentClassAsString +
										" then set A is class: " + currentClassAsString + ". \n" +
										"Predicted correctly: " + to_string(correctlyPredicted) + ", predicted incorrectly: " + to_string(incorreclyPredicted) + ". \n" +
										"Total predicted: " + to_string(totalPredicted) + ", Precision = " + to_string(precision) + "%. \n" +
										"Class Coverage = " + to_string(classCoverage) + "%, Correct Class Coverage = " + to_string(correctClassCoverage) + "%. \n" +
										"Total Coverage = " + to_string(totalCoverage) + "%. \n" +
										"Left: " + to_string(leftAttributeVal) + "\n\n";

									//Record the current highest precision, current highest coverage, and current cases covered.
									highestPrecision = precision;
									highestCoverage = totalCoverage;
									currentCasesAlreadyUsed = casesInRule;
									currentBestRuleGenerated = ruleStatement;
									ruleGenerated = true; // Set flag that a rule was generated.

									//Determine the position if this line for rule visuazliation and save it under the current best rule.
									double leftPosition = 0;
									double rightPosition = 0;
									vector<pair<double, double>> currentVecRight = sortedVector.at(j);
									vector<pair<double, double>> currentVecLeft = sortedVector.at(i);
									for (int c = 0; c < currentVecLeft.size(); c++)
									{
										if (currentVecLeft.at(c).first == leftAttributeVal)
										{
											leftPosition = currentVecLeft.at(c).second;
											break;
										}
									}
									for (int c = 0; c < currentVecRight.size(); c++)
									{
										if (currentVecRight.at(c).first == rightAttributeVal)
										{
											rightPosition = currentVecRight.at(c).second;
											break;
										}
									}

									pair<double, double> values;
									values.first = leftPosition;
									values.second = -1;//Label this as a single attribute rule.
									pair<double, pair<double, double>> toAdd;
									toAdd.first = i;
									toAdd.second = values;
									currentBestRuleData = toAdd;

								}
							
							}//End of total predicted != 0.

						}//End of Condition 4.

					}//End of right coordinate block iteration.

				}//End of left coordinate block iteration.

			}//End of iterating classes.

		}//End of iterating dimensions.

		//If we generated a rule.
		if (ruleGenerated == true)
		{
			//Record the top rule and reset values for next.
			currentBestRuleGenerated = "Rule " + to_string(ruleCount) + ".) " + currentBestRuleGenerated;//Add rule num.
			toReturn.push_back(currentBestRuleGenerated);
			highestPrecision = 0;
			highestCoverage = 0;
			RuleCaseCounts.push_back(currentCasesAlreadyUsed);//Add the cases in this rule.
			for (int i = 0; i < currentCasesAlreadyUsed.size(); i++) //Add cases from this rule to case pool.
			{
				casesAlreadyUsed.push_back(currentCasesAlreadyUsed.at(i));
			}
			currentCasesAlreadyUsed.clear();

			if (currentBestRuleData.first != -1)
			{
				ruleData.push_back(currentBestRuleData); //Record best rules data to visualize.
				//Record rule for hover data.
				rulesByCoordinate.insert({ currentBestRuleData.first, currentBestRuleGenerated }); //First coordinate.
				rulesByCoordinate.insert({ (currentBestRuleData.first) + 1, currentBestRuleGenerated }); // Second coordinate.
			}
			currentBestRuleData.first = -1; //Mark curbestruledata to not be used yet.
			ruleCount++;
		}

	}//End of while loop.

	//Set rules generated by coordinate so they can be displayed by hover.
	file->setDNSRulesByCoordinate(rulesByCoordinate);

	//Record how many casea total are covered by all the generated rules.
	toReturn.push_back("Total number of cases covered by all rules: " + to_string(casesAlreadyUsed.size()) + "\n\n");

	/*

	//=====Overlap Table=====//



	//Calculate overlap percentage and unique cases #.
	double overlapTable[100][100];
	int numUniqueCases[100][100];

	//Open File:
	SaveFileDialog^ sfd = gcnew SaveFileDialog;
	sfd->InitialDirectory = ""; // NOTE "c:\\" for future reference.
	sfd->Filter = "Text Files (*.csv, *.txt, *text)|*.csv, *txt, *text|All Files (*.*)|*.*";
	sfd->FilterIndex = 1;
	sfd->RestoreDirectory = true;

	System::String^ path;

	if (sfd->ShowDialog() == System::Windows::Forms::DialogResult::OK)
	{
		path = sfd->FileName;
	}

	//Write Data:
	std::ofstream outFile;
	std::string outDir = msclr::interop::marshal_as<std::string>(path);
	std::string temp = "";

	outFile.open(outDir);

	//Add top lables:
	outFile << " ,";
	for (int i = 1; i < ruleCount; i++)
	{
		outFile << msclr::interop::marshal_as<std::string>("Rule " + i + ",");
	}
	outFile << "\n";

	//Itterate over rows:
	for (int i = 0; i < ruleCount - 1; i++)
	{
		vector<int>columnCases = RuleCaseCounts.at(i);

		outFile << "Rule " + to_string(i+1) + ",";

		for (int j = 0; j < ruleCount - 1; j++)
		{

			vector<int>rowCases = RuleCaseCounts.at(j);

			int overlapCount = 0;

			//Go over column cases and check if they are in the row cases.
			for (int k = 0; k < columnCases.size(); k++)
			{
				for (int m = 0; m < rowCases.size(); m++)
				{
					if (rowCases.at(m) == columnCases.at(k))
					{
						overlapCount++;
					}
				}
			}

			//Calculate how many total cases are in both rules.
			int totalCases = rowCases.size() + columnCases.size() - (overlapCount);

			//Calculate the remaining cases after removed overlap:
			int remainingCases = rowCases.size() + columnCases.size() - (overlapCount * 2);

			//Record the number Unique Cases.
			numUniqueCases[i][j] = totalCases;

			//Record percent.
			if (remainingCases != 0)
			{
				overlapTable[i][j] = (double(overlapCount) / double(totalCases)) * 100.0;
			}
			else
			{
				overlapTable[i][j] = -1;
			}

			//Calculate the number of values if just one rule is used.
			int columnOnly = columnCases.size() - overlapCount;
			int rowOnly = rowCases.size() - overlapCount;

			outFile << "" + to_string(overlapTable[i][j]) + " | " + to_string(totalCases) +
				" | R:" + to_string(columnOnly) +" | C:"+ to_string(rowOnly) +" | OL:" + to_string(overlapCount) +" ,";
		}

		outFile << "\n";
	}

	//Write to the file:
	outFile.close();



	//======================//

	*/

	return toReturn;
}

//MTBRGSequential:
//Desc: Generates rules sequentailly to ensure there is little overlap. Uses Pareto front rules and applies thresholds.
vector<string> DomNominalSet::MTBRGSequential(double precisionThresh, vector<vector<int>>groups, int targetClass)
{
	vector<string> toReturn;
	vector<int> allGroupCases;
	vector<DNSRule> allGroupRules;
	vector<DNSRule> allGeneratedRules;
	vector<int> targetCasesCovered;
	vector<int> nonTargetCasesCovered;
	int numCasesInTarget = 0;
	const double COVERAGETHRESHOLD = 0.5;//%
	bool fullyCovered = false;
	vector<int> coorainteAppearance;

	//Fill coordinate appearance with 0s.
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{
		coorainteAppearance.push_back(0);
	}

	//Determine total number of cases in target class.
	for (int i = 0; i < file->getSetAmount(); i++)
	{
		if (file->getClassOfSet(i) == targetClass) numCasesInTarget++;
	}

	//Go over all groups and generate all rules.
	for (int n = 0; n < groups.size(); n++)
	{
		vector<DNSRule> curGroupRules = MTBRuleGeneration(precisionThresh, groups.at(n), COVERAGETHRESHOLD, targetClass, numCasesInTarget);

		//Store all rules.
 		for (int i = 0; i < curGroupRules.size(); i++)
		{
			allGeneratedRules.push_back(curGroupRules.at(i));
		}
	}


	//Combine Rules:

	allGeneratedRules = combineRulesGenerated(allGeneratedRules, targetClass, numCasesInTarget, precisionThresh);

	//==================Selection Process=========================//

	string toAdd;
	vector<int> casesCovered;
	vector<int> incorrectCases;
	vector<DNSRule> selectedRules;

	//Create vector of pairs of coverage and rules.
	vector<pair<double, int>> coveragePerRuleIndex;
	for (int i = 0; i < allGeneratedRules.size(); i++)
	{
		DNSRule curRule = allGeneratedRules.at(i);
		double curCov = curRule.getTotalCoverage();

		pair<double, int> newPair;
		newPair.first = curCov;
		newPair.second = i;

		coveragePerRuleIndex.push_back(newPair);
	}

	//Sort the pairs and reverse so we have highest coverage in decending order.
	sort(coveragePerRuleIndex.begin(), coveragePerRuleIndex.end());
	reverse(coveragePerRuleIndex.begin(), coveragePerRuleIndex.end());

	//Select the rule with the highest coverage.
	int curRuleIndex = 0;
	while ((targetCasesCovered.size() != numCasesInTarget) && (selectedRules.size() != allGeneratedRules.size()) && curRuleIndex != coveragePerRuleIndex.size())
	{
		//Current rule info.
		int ruleIndex = coveragePerRuleIndex.at(curRuleIndex).second;
		DNSRule curRule = allGeneratedRules.at(ruleIndex);
		vector<int> curRuleCases = allGeneratedRules.at(ruleIndex).getCasesUsed();

		//=========================Conditions ==============================//
		//Condition 1: The addition of this rule to the final rules must contribute newly predicted
		// target class cases.
		//Condition 2: The number of new non target cases used must be less then the new target cases used.

		//Determine how many cases in the current rule are the target class.
		vector<int> curRuleTargetIDs;
		vector<int> curRuleNonTargetIDs;
		for (int i = 0; i < curRuleCases.size(); i++)
		{
			if (file->getClassOfSet(curRuleCases.at(i)) == targetClass)
			{
				curRuleTargetIDs.push_back(curRuleCases.at(i));
			}
			else
			{
				curRuleNonTargetIDs.push_back(curRuleCases.at(i));
			}
		}

		//Determine if there are any new target cases between this rule and the target cases already covered.
		vector<int> targetCasesNotContianed;
		for (int i = 0; i < curRuleTargetIDs.size(); i++)
		{
			bool notContained = true;
			for (int j = 0; j < targetCasesCovered.size(); j++)
			{
				if (targetCasesCovered.at(j) == curRuleTargetIDs.at(i))
				{
					notContained = false;
					break;
				}
			}

			if (notContained)
			{
				targetCasesNotContianed.push_back(curRuleTargetIDs.at(i));
			}
		}

		vector<int> nonTargetCasesNotContianed;
		for (int i = 0; i < curRuleNonTargetIDs.size(); i++)
		{
			bool notContained = true;
			for (int j = 0; j < nonTargetCasesCovered.size(); j++)
			{
				if (nonTargetCasesCovered.at(j) == curRuleNonTargetIDs.at(i))
				{
					notContained = false;
					break;
				}
			}

			if (notContained)
			{
				nonTargetCasesNotContianed.push_back(curRuleNonTargetIDs.at(i));
			}
		}

		double newTotal = (nonTargetCasesCovered.size() + nonTargetCasesNotContianed.size()) + (targetCasesCovered.size() + targetCasesNotContianed.size());
		double newPrecision = ((targetCasesCovered.size() + targetCasesNotContianed.size()) / newTotal) * 100.0;
		double prevPrecision;
		
		//Starting calc
		if (curRuleIndex == 0)
		{
			prevPrecision = 0;
		}

		double t1 = (targetCasesCovered.size() + nonTargetCasesCovered.size());
		double t2 = targetCasesCovered.size();
		prevPrecision = (t2 / t1) * 100.0;

		//===================================================================//

		//Check to see if this is the first rule we are adding. If so, make sure that it has a high enough precision.
		//If the first rule has not been selected, make sure it is 100% precision. If not continue to the next.
		/*
		if (selectedRules.size() == 0)
		{
			if (newPrecision != 100)
			{
				curRuleIndex++;
				continue;
			}

		}
		*/

		//If there are new target cases in this class, record.
		if (targetCasesNotContianed.size() != 0 && newPrecision >= prevPrecision)
		{
			//Record rule.
			selectedRules.push_back(allGeneratedRules.at(ruleIndex));

			//Record incorrect cases.
			for (int i = 0; i < curRuleCases.size(); i++)
			{
				//If this is an incorrect case.
				if (this->file->getClassOfSet(curRuleCases.at(i)) != targetClass)
				{
					//Check if in our list already.
					bool isContained = false;
					for (int j = 0; j < incorrectCases.size(); j++)
					{
						if (incorrectCases.at(j) == curRuleCases.at(i))
						{
							isContained = true;
							break;
						}
					}

					if (!isContained)
					{
						incorrectCases.push_back(curRuleCases.at(i));
					}

				}
			}

			//Record how many cases have been covered of the target class.
			for (int i = 0; i < curRuleCases.size(); i++)
			{
				bool notContained = true;
				for (int j = 0; j < targetCasesCovered.size(); j++)
				{
					if (targetCasesCovered.at(j) == curRuleCases.at(i))
					{
						notContained = false;
						break;
					}
				}

				if (notContained)
				{
					//If this is a case for the target class:
					if (file->getClassOfSet(curRuleCases.at(i)) == targetClass)
					{
						targetCasesCovered.push_back(curRuleCases.at(i));
					}
				}
			}

			//Record how many cases have been covered of the non target classes.
			for (int i = 0; i < curRuleCases.size(); i++)
			{
				bool notContained = true;
				for (int j = 0; j < nonTargetCasesCovered.size(); j++)
				{
					if (nonTargetCasesCovered.at(j) == curRuleCases.at(i))
					{
						notContained = false;
						break;
					}
				}

				if (notContained)
				{
					//If this is a case for the target class:
					if (file->getClassOfSet(curRuleCases.at(i)) != targetClass)
					{
						nonTargetCasesCovered.push_back(curRuleCases.at(i));
					}
				}
			}

			//Record all cases covered overall.
			for (int i = 0; i < curRuleCases.size(); i++)
			{
				int ruleID = curRuleCases.at(i);
				bool notContained = true;
				for (int j = 0; j < casesCovered.size(); j++)
				{
					if (casesCovered.at(j) == ruleID)
					{
						notContained = false;
						break;
					}
				}

				if (notContained)
				{
					casesCovered.push_back(ruleID);
				}
			}

			//Record if fully covered.
			if (targetCasesCovered.size() == numCasesInTarget)
			{
				fullyCovered = true;
				break;
			}

		}//End of if rule has new target cases.

		curRuleIndex++;

	}// End of while iterating over rules.

	//============================================================================//

	//Generate rules to correct incorrectly predicted cases.
	
	//Go over all groups and generate all rules.
	allGeneratedRules.clear();
	const int NON_TARGET_CLASS = 2;
	for (int n = 0; n < groups.size(); n++)
	{
		vector<DNSRule> curGroupRules = MTBRuleGeneration(precisionThresh, groups.at(n), COVERAGETHRESHOLD, NON_TARGET_CLASS, numCasesInTarget);

		//Store all rules.
		for (int i = 0; i < curGroupRules.size(); i++)
		{
			allGeneratedRules.push_back(curGroupRules.at(i));
		}
	}

	//Go over all generated rules for non-target class and see if any will reduce the number of incorrect cases.
	for (int i = 0; i < allGeneratedRules.size(); i++)
	{
		DNSRule curRule = allGeneratedRules.at(i);
		vector<int> curRuleCases = curRule.getCasesUsed();
		double curRulePrecision = curRule.getPrecision();

		//If this is a 100% precision rule,
		if (curRulePrecision == 100)
		{
			vector<int> curRuleCases = curRule.getCasesUsed();

			//Determine if any incorrect cases are covered,
			int numCasesIncorrect = 0; //Count of how many cases were predicted incorrectly.
			for (int j = 0; j < curRuleCases.size(); j++)
			{
				for (int k = 0; k < incorrectCases.size(); k++)
				{
					if (curRuleCases.at(j) == incorrectCases.at(k))
					{
						numCasesIncorrect++;
						break;
					}
				}
			}

			//If this is a benifical rule,
			if (numCasesIncorrect != 0)
			{
				//Add this rule to our final rules.
				selectedRules.push_back(curRule);

				//Remove all newly covered incorrectly predicted cases.
				for (int j = 0; j < curRuleCases.size(); j++)
				{
					bool needsRemoved = false;
					auto removeIt = incorrectCases.begin();
					for (auto it = incorrectCases.begin(); it != incorrectCases.end(); it++)
					{
						if (curRuleCases.at(j) == *it)
						{
							needsRemoved = true;
							break;
						}
						removeIt++;
					}

					if (needsRemoved)
					{
						incorrectCases.erase(removeIt);
					}
				}
			}
		}

		//End addition of rules if all incorrect cases are covered.
		if (incorrectCases.size() == 0)
		{
			break;
		}
	}

	//Add group rules to all rules.
	for (int i = 0; i < selectedRules.size(); i++)
	{
		allGroupRules.push_back(selectedRules.at(i));
	}

	//record all cases covered.
	for (int i = 0; i < casesCovered.size(); i++)
	{
		bool notContained = true;
		for (int j = 0; j < allGroupCases.size(); j++)
		{
			if (allGroupCases.at(j) == casesCovered.at(i))
			{
				notContained = false;
				break;
			}
		}

		if (notContained)
		{
			allGroupCases.push_back(casesCovered.at(i));
		}
	}

	int numCasesClass1 = 0;
	int numCasesClass2 = 0;

	for (int i = 0; i < casesCovered.size(); i++)
	{
		int curClass = file->getClassOfSet(casesCovered.at(i));
		if (curClass == 1)
		{
			numCasesClass1++;
		}
		else if (curClass == 2)
		{
			numCasesClass2++;
		}
	}

	//Determine how many cases of the target class are covered.
	int casesInTargetClass = 0;
	for (int i = 0; i < allGroupCases.size(); i++)
	{
		if (file->getClassOfSet(allGroupCases.at(i)) == targetClass)
		{
			casesInTargetClass++;
		}
	}

	toReturn.push_back(("\nAll Generated rules: " + to_string(allGroupRules.size()) + " All cases covered: " +
		to_string(allGroupCases.size()) + " Total target class cases: " + to_string(casesInTargetClass) +
		" Total incorrectly predicted cases: " + to_string(incorrectCases.size()) + "\n\n"));

	//Print out all rules in a readable format:
	for (int i = 0; i < allGroupRules.size(); i++)
	{
		DNSRule curRule = allGroupRules.at(i);

		toReturn.push_back("Rule " + to_string(i + 1) + "\n");
		toReturn.push_back("Class predicted: " + to_string(curRule.getRuleClass()) + "\n");
		toReturn.push_back("Precision: " + to_string(curRule.getPrecision()) + "%\n");
		toReturn.push_back("Coverage: " + to_string(curRule.getTotalCoverage()) + "%\n");
		toReturn.push_back("Total cases predicted: " + to_string(curRule.getTotalCases()) + "\n");
		toReturn.push_back("Correctly predicted: " + to_string(curRule.getCorrectCases()) + "\n");
		toReturn.push_back("Incorrectly predicted: " + to_string(curRule.getIncorrectCases()) + "\n");
		toReturn.push_back("Attributes and coordinates used: \n");
		vector<int> curCoord = curRule.getCoordinatesUsed();
		vector<double> curAttri = curRule.getAttributesUsed();
		for (int j = 0; j < curCoord.size() - 1; j++)
		{
			string s = to_string(curAttri.at(j));
			toReturn.push_back("X" + to_string(curCoord.at(j) + 1) + " -> " + s + "\n");
		}
		string s = to_string(curAttri.at(curCoord.size() - 1));
		toReturn.push_back("X" + to_string(curCoord.at(curCoord.size() - 1) + 1) + " -> " + s + "\n\n");

		toReturn.push_back("Negated Attributes: \n");
		vector<double> negatedAttri = curRule.getNegatedAttributesUsed();

		if (negatedAttri.size() >= 1)
		{
			for (int j = 0; j < negatedAttri.size() - 1; j++)
			{
				toReturn.push_back(to_string(negatedAttri.at(j)) + ", ");
			}
			toReturn.push_back(to_string(negatedAttri.at(negatedAttri.size() - 1)) + "\n\n\n");
		}
	}

	//Count the number of times each cordinate shows up.
	for (int i = 0; i < allGroupRules.size(); i++)
	{
		DNSRule r = allGroupRules.at(i);

		vector<int> ruleAttributes = r.getCoordinatesUsed();
		for (int j = 0; j < ruleAttributes.size(); j++)
		{
			//Increment count by one.
			coorainteAppearance.at(ruleAttributes.at(j)) = (coorainteAppearance.at(ruleAttributes.at(j)) + 1);
		}
	}

	toAdd = "";
	toAdd += "Coordinate Appearance = ";
	for (int i = 0; i < coorainteAppearance.size() - 1; i++)
	{
		toAdd += "X" + to_string(i + 1) + ": " + to_string(coorainteAppearance.at(i)) + ", ";
	}
	toAdd += "X" + to_string(coorainteAppearance.size()) + ": " + to_string(coorainteAppearance.at(coorainteAppearance.size() - 1)) + "\n ";

	toReturn.push_back(toAdd);

	return toReturn;
}//End of rule generation sequential all attributes.

//combineRulesGenerated:
//Desc: Takes a list of generated rules and adds simple combinations of rules to the end.
vector<DNSRule> DomNominalSet::combineRulesGenerated(vector<DNSRule> allGeneratedRules, int targetClass, int numCasesInTargetClass, double precThresh)
{

	vector<DNSRule> basicCombinations;

	//Create vector of pairs complexity of rules.
	vector<pair<int, int>> complexityPerRuleIndex;
	for (int i = 0; i < allGeneratedRules.size(); i++)
	{
		DNSRule curRule = allGeneratedRules.at(i);
		vector<int> curRuleCoordinates = curRule.getCoordinatesUsed();
		vector<double> curRuleNegatedPositions = curRule.getNegatedAttributesUsed();

		int ruleComplexity = 0;

		for (int j = 0; j < curRuleCoordinates.size(); j++)
		{
			//Find if this is a negated cooridnate.
			bool isNegated = false;
			for (int k = 0; k < curRuleNegatedPositions.size(); k++)
			{
				if (curRuleNegatedPositions.at(k) == j)
				{
					isNegated = true;
					break;
				}
			}

			if (isNegated)
			{
				//Determine number of unique values in coordinate.
				vector<double> coordValues;
				for (int k = 0; k < file->getSetAmount(); k++)
				{
					double curData = file->getData(k, curRuleCoordinates.at(j));
					bool isContained = false;
					for (int m = 0; m < coordValues.size(); m++)
					{
						if (coordValues.at(m) == curData)
						{
							isContained = true;
							break;
						}
					}

					if (!isContained)
					{
						coordValues.push_back(curData);
					}
				}
				ruleComplexity = ruleComplexity + coordValues.size();
			}
			else
			{
				ruleComplexity++;
			}
		}

		pair<int, int> newPair;
		newPair.first = ruleComplexity;
		newPair.second = i;

		complexityPerRuleIndex.push_back(newPair);
	}

	//Sort the pairs and reverse so we have lowest complexity in ascending order.
	sort(complexityPerRuleIndex.begin(), complexityPerRuleIndex.end());

	for(int n = 0; n < complexityPerRuleIndex.size(); n++)
	{
		//Current rule info.
		int ruleIndex = complexityPerRuleIndex.at(n).second;
		DNSRule curRule = allGeneratedRules.at(ruleIndex);
		vector<int> curRuleCases = allGeneratedRules.at(ruleIndex).getCasesUsed();

		//Determine if any other rules can be combined with this current rule.
		bool ruleCombined = false;
		DNSRule combinedRule = curRule;
		
		for (int i = 0; i < complexityPerRuleIndex.size(); i++)
		{
			//Dont combine with self.
			if (i == n) continue;
			int candidateRuleIndex = complexityPerRuleIndex.at(i).second;
			DNSRule candidateRule = allGeneratedRules.at(candidateRuleIndex);

			//Get data of the two rules used to determine if they can be combined.
			vector<int> candidateCoordinatesUsed = candidateRule.getCoordinatesUsed();
			vector<int> curRuleCoordinatesUsed = combinedRule.getCoordinatesUsed();
			vector<double> candidateCoordinateNegated = candidateRule.getNegatedAttributesUsed();
			vector<double> curRuleCoordinateNegated = combinedRule.getNegatedAttributesUsed();

			//Check to see if rule can be combined.
			bool canBeCombined = true;
			for (int j = 0; j < curRuleCoordinatesUsed.size(); j++)
			{
				canBeCombined = true;

				//Check to see if this is a negated coordinate.
				bool curRuleCoordIsNegated = false;
				for (int m = 0; m < curRuleCoordinateNegated.size(); m++)
				{
					if (curRuleCoordinateNegated.at(m) == j)
					{
						curRuleCoordIsNegated = true;
						break;
					}
				}

				//Check to see if the candidate rule has this coordinate.
				bool hasCoordinate = false;
				for (int k = 0; k < candidateCoordinatesUsed.size(); k++)
				{
					hasCoordinate = false;
					//If the corodinate is contained.
					if (curRuleCoordinatesUsed.at(j) == candidateCoordinatesUsed.at(k))
					{
						//check if candidtae rule is negated.
						bool candidateCoordIsNegated = false;
						for (int m = 0; m < candidateCoordinateNegated.size(); m++)
						{
							if (candidateCoordinateNegated.at(m) == k)
							{
								candidateCoordIsNegated = true;
								break;
							}
						}

						//If both of the coordinates are negated:
						//if (candidateCoordIsNegated && curRuleCoordIsNegated)
						//{
							//hasCoordinate = true;
						//}
						//If both of the coordinates are not negated:
						if (!candidateCoordIsNegated && !curRuleCoordIsNegated)
						{
							hasCoordinate = true;
						}
										
					}

					if (!hasCoordinate)
					{
						canBeCombined = false;
						break;
					}
				}

				if (!hasCoordinate)
				{
					canBeCombined = false;
					break;
				}
			}

			//If the two rules can be combined, combine them and add to final list.
			if (canBeCombined)
			{
				//Determine coordinates used.
				vector<int> combinedRuleCoordinatesUsed;
				for (int j = 0; j < curRuleCoordinatesUsed.size(); j++)
				{
					combinedRuleCoordinatesUsed.push_back(curRuleCoordinatesUsed.at(j));
				}

				for (int j = 0; j < candidateCoordinatesUsed.size(); j++)
				{
					combinedRuleCoordinatesUsed.push_back(candidateCoordinatesUsed.at(j));
				}
				combinedRule.setCoordinatesUsed(combinedRuleCoordinatesUsed);

				//Determine what attributes were used.
				vector<double> combinedAttributesUsed;
				vector<double> curRuleAttributesUsed = combinedRule.getAttributesUsed();
				vector<double> candidtateRuleAttirbutesUsed = candidateRule.getAttributesUsed();
				for (int j = 0; j < curRuleAttributesUsed.size(); j++)
				{
					combinedAttributesUsed.push_back(curRuleAttributesUsed.at(j));
				}

				for (int j = 0; j < candidtateRuleAttirbutesUsed.size(); j++)
				{
					combinedAttributesUsed.push_back(candidtateRuleAttirbutesUsed.at(j));
				}
				combinedRule.setAttributesUsed(combinedAttributesUsed);

				//Determine negated attributes;
				vector<double> combinedNegatedAttributes;
				for (int j = 0; j < curRuleCoordinateNegated.size(); j++)
				{
					combinedNegatedAttributes.push_back(curRuleCoordinateNegated.at(j));
				}

				for (int j = 0; j < candidateCoordinateNegated.size(); j++)
				{
					combinedNegatedAttributes.push_back(candidateCoordinateNegated.at(j) + curRuleCoordinatesUsed.size());
				}
				combinedRule.setNegatedAttributesUsed(combinedNegatedAttributes);

				//========Recalculate==========//

				//Find larges value.
				int largestCoord = 0;
				for (int j = 0; j < combinedRuleCoordinatesUsed.size(); j++)
				{

					if (combinedRuleCoordinatesUsed.at(j) > largestCoord)
					{
						largestCoord = combinedRuleCoordinatesUsed.at(j);
					}
				
				}

				//Start by combining the attributes with theirt attribute values.
				vector<vector<double>> attributesAndAttributeValues(largestCoord + 1);
				for (int j = 0; j < combinedRuleCoordinatesUsed.size(); j++)
				{
					attributesAndAttributeValues.at(combinedRuleCoordinatesUsed.at(j)).push_back(combinedAttributesUsed.at(j));
				}

				//Determine cases covered by iterating over sets.
				int correctlyPredicted = 0;
				int incorrectlyPredicted = 0;
				int totalPredicted = 0;
				vector<int> casesUsed;
				for (int j = 0; j < file->getSetAmount(); j++)
				{

					//Check each coordinate grouping.
					bool doesSatisfy = true;
					for (int k = 0; k < combinedRuleCoordinatesUsed.size(); k++)
					{

						double originalDataAtCoordForSet = file->getOriginalData(j, combinedRuleCoordinatesUsed.at(k));

						//Get attributes.
						vector<double> attributesForCurCoord = attributesAndAttributeValues.at(combinedRuleCoordinatesUsed.at(k));

						//Determine if attributes are all negated or all non negated.
						bool negatedAttri = false;
						for (int m = 0; m < combinedNegatedAttributes.size(); m++)
						{
							if (combinedNegatedAttributes.at(m) == k)
							{
								negatedAttri = true;
								break;
							}
						}

						//Handle negated attribute values different then normal attribute values.
						if (!negatedAttri)
						{
							bool correct = false;
							for (int m = 0; m < attributesForCurCoord.size(); m++)
							{
								if (attributesForCurCoord.at(m) == originalDataAtCoordForSet)
								{
									correct = true;
								}
							}

							if (!correct)
							{
								doesSatisfy = false;
								break;
							}
						}
					}
				
					if (doesSatisfy)
					{
						totalPredicted++;
						casesUsed.push_back(j);

						if (file->getClassOfSet(j) == targetClass)
						{
							correctlyPredicted++;
						}
						else
						{
							incorrectlyPredicted++;
						}
					}
				
				}//End iterating over sets.

				//Calculate and set values.
				double newCoverage = (double(totalPredicted) / double(numCasesInTargetClass)) * 100.0;
				double newPrecision = (double(correctlyPredicted) / double(totalPredicted)) * 100.0;
				combinedRule.setTotalCoverage(newCoverage);
				combinedRule.setPrecision(newPrecision);
				combinedRule.setCorrectCases(correctlyPredicted);
				combinedRule.setIncorrectCases(incorrectlyPredicted);
				combinedRule.setTotalCases(totalPredicted);
				combinedRule.setCasesUsed(casesUsed);

				//Record to add at the end.
				basicCombinations.push_back(combinedRule);

			}//End of rule combination.

		}//End of testing combining rules.

	}// End of iterating over rules.

	//Add all combined rules to the passed rules.

	for (int i = 0; i < basicCombinations.size(); i++)
	{
		allGeneratedRules.push_back(basicCombinations.at(i));
	}

	return allGeneratedRules;
}

//linguisticDesc
//Desc: Creates the linguistic description for the visualization. Result is a formatted string to be added to the linguistic desc.
string DomNominalSet::linguisticDesc()
{
	//String to add to the descrption:
	string toReturn = "--- Dominant Nominal Sets ---\n";
	
	
	/*

	//Get sorted vector to use.
	vector<vector<pair<double, double>>> sortedVector = getSortByFreqency(blockHeights);

	vector<vector<int>> coordinateGroups;

	//Create a vector of vectors to hold the dimensions to hold the values of class frequencies.
	vector<vector<unordered_map<double, double>*>*>* classValFreq = new vector<vector<unordered_map<double, double>*>*>();

	//We will be working with two classes for now.

	//Itterate over number of dimensions.
	for (int j = 0; j < file->getDimensionAmount(); j++)
	{
		//Add the vector of classes to the dimension.
		classValFreq->push_back(new vector<unordered_map<double, double>*>());

		//Add classes to vector for this dimension.
		for (int k = 0; k < file->getClassAmount() - 2; k++) // -2 because of Default and 'class' Column.
		{
			classValFreq->at(j)->push_back(new unordered_map<double, double>());
		}
	}

	//Itterate over all attributes:
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{

		//Itterate over rows and add value to class vector.
		for (int j = 0; j < file->getSetAmount(); j++)
		{
			//Get the current value of the row at this attribute.
			double currentData = file->getData(j, i);
			//Get the current class of the row at this attribute.
			double currentClass = file->getClassOfSet(j) - 1;

			//Now we have the values of each class and how freqently they appear.
			//Now add them to a vector of maps.

			//Get current class map.
			unordered_map<double, double>* curMap = classValFreq->at(i)->at(currentClass);

			//Check to see if current data is already in the unordered map.
			if (curMap->find(currentData) == curMap->end())
			{
				//If not insert it.
				curMap->insert({ currentData, 1 });
			}
			else
			{
				//Increment occurance of current data.
				std::unordered_map<double, double>::iterator it = curMap->find(currentData);
				it->second++;
			}

		}

	}

	//Fill vector:
	for (int i = 0; i < 4; i++)
	{
		coordinateGroups.push_back(vector<int>());
	}

	for (int i = 1; i <= sortedVector.size(); i++)
	{
		vector<pair<double, double>> currentCoordinate = sortedVector.at(i - 1);

		//check top pair and add to group.
		double topPerc = currentCoordinate.at(0).second;
		if (topPerc >= 0.75)
		{
			coordinateGroups.at(0).push_back(i);
		}
		else if (topPerc < 0.75 && topPerc >= 0.50)
		{
			coordinateGroups.at(1).push_back(i);
		}
		else if (topPerc < 0.50 && topPerc >= 0.25)
		{
			coordinateGroups.at(2).push_back(i);
		}
		else
		{
			coordinateGroups.at(3).push_back(i);
		}
	}

	//Add results to the output string.
	for (int i = 0; i < 4; i++)
	{
		string s = "Coordinates with dominant value ";

		switch (i)
		{
		case 0:
			s += "75% or greater : ";
			break;
		case 1:
			s += "50% to 74% : ";
			break;
		case 2:
			s += "25% ot 49% : ";
			break;
		case 3:
			s += "less then 25% : ";
			break;
		default:
			break;
		}

		//Add coordinates
		for (int j = 0; j < coordinateGroups.at(i).size() - 1; j++)
		{
			s += "X" + to_string(coordinateGroups.at(i).at(j)) + ", ";
		}

		s += "X" + to_string(coordinateGroups.at(i).at(coordinateGroups.at(i).size() - 1)) + ".\n";

		toReturn += s;
	}

	vector<vector<pair<double, double>>> middleOther; //Vector to hold possition of middle of other block.
	vector<vector<pair<double, double>>> domClass; // Vector to hold what the dominant class is for each attribute.
	int currDim = 0;

	//Go over every attribute.
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{

		//Get current vector (attribute we are working with making blocks):
		vector<pair<double, double>> curVec = sortedVector[i];

		//Values to calculate where to put values.
		double blockOffsetVertical = 80;
		double prevHeight = 80;

		//Push back vectors:
		domClass.push_back(vector<pair<double, double>>());
		middleOther.push_back(vector<pair<double, double>>());

		//Iterate over vector to find valus.
		for (int j = 0; j < curVec.size(); j++)
		{
			double key = curVec[j].first;
			double freq = curVec[j].second;
			double dominantFreq = 0;

			//Go over classes and find dominant class:
			vector<unordered_map<double, double>*>* curDimensionVec = this->valueFreqPerClass->at(i);

			for (int m = 0; m < curDimensionVec->size(); m++)
			{
				unordered_map<double, double>* nextClass = curDimensionVec->at(m);

				if (nextClass->find(key) != nextClass->end())
				{
					if (nextClass->at(key) >= dominantFreq) dominantFreq = nextClass->at(key);
				}
			}

			//Dominant freq over 80%.
			if ((dominantFreq / freq) >= 0.8 && freq > 0.10)
			{
				double calc = (dominantFreq / freq) * 100;
				int res = calc;

				toReturn += msclr::interop::marshal_as<std::string>("X" + (i + 1) + ", block, " + (j + 1) + " has a purity of " + res + "%\n");
			}

			//Total freq over 75%.
			if (freq >= 0.75 && !freq <= 0.014)
			{
				double calc = freq * 100;
				int res = calc;
				toReturn += msclr::interop::marshal_as<std::string>("X" + (i + 1) + ", block, " + (j + 1) + " has a total frequency of " + res + "%\n");
			}

			//Small freq block exists.
			if (freq <= 0.014 && currDim != i)
			{
				currDim = i;
				toReturn += msclr::interop::marshal_as<std::string>("X" + (i + 1) + " has a small frequency block.\n");
			}

		}

	}


	*/


	//Get Rules.
	vector<string> rules = file->getDNSRulesGenerated();

	for (int i = 0; i < rules.size(); i++)
	{
		toReturn += rules.at(i);
	}

	toReturn += msclr::interop::marshal_as<std::string>("Number of distinct segments of polylines not shown: " + file->getDNSLinesTransparent() + "\n");
	toReturn += "Total number of distinct segments of polylines: " + to_string(file->getDNSNumSmallLines()) + "\n";
	int percent = ((double(file->getDNSLinesTransparent()) / double(file->getDNSNumSmallLines())) * 100.0);
	toReturn += "Percentage of distinct segments of polylines not shown: " + to_string(percent) + "%\n";
	double numnDPont = (file->getDNSnDPointsVisualized());
	double dimAM = ((file->getDimensionAmount() - 1) * file->getSetAmount());
	int nDPercent = ((numnDPont / dimAM) * 100.0);
	toReturn += "Percentage of n-D points visualized: " + to_string(nDPercent) + "%\n";
	toReturn += msclr::interop::marshal_as<std::string>("Number of full n-D points being visualized: " + file->getDNSNumSetsVisualized() + "\n");

	toReturn += "---------------------------------------\n\n";

	return toReturn;

}

//drawGrayRectangles:
//Desc: Draws the gray rectangles for gray block visualization.
GLvoid DomNominalSet::drawGrayRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth)
{
	int dimensionCount = 0; // Variable for the dimension index.
	int colorChoice = file->getNominalColor();
	glLineWidth(3.0); //Seting line width.
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.0

	//====Draw the rectangles====.
	const int HEIGHT_OF_ALL_BLOCKS = 435;
	vector<vector<pair<double, double>>> middleOther; //Vector to hold possition of middle of other block.
	vector<vector<pair<double, double>>> domClass; // Vector to hold what the dominant class is for each attribute.

	//Go over every attribute.
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{

		//Get current vector (attribute we are working with making blocks):
		vector<pair<double, double>> curVec = sortedByPurityVector[i];

		//Values to calculate where to put values.
		double blockOffsetVertical = 80;
		double prevHeight = 80;

		//Push back vectors:
		domClass.push_back(vector<pair<double, double>>());
		middleOther.push_back(vector<pair<double, double>>());

		//Iterate over vector to find valus.
		for (int j = 0; j < curVec.size(); j++)
		{
			//Get key and frequency and draw a rectangle.
			double key = curVec[j].first;
			double freq = curVec[j].second;
			double domPerc = 0;
			double dominantClass = -1;
			const double GRAY_VAL = 1;

			//Go over classes and find dominant class:
			vector<unordered_map<double, double>*>* curDimensionVec = classPercPerBlock->at(i);

			for (int m = 0; m < curDimensionVec->size(); m++)
			{
				unordered_map<double, double>* nextClass = curDimensionVec->at(m);

				if (nextClass->find(key) != nextClass->end())
				{
					if (nextClass->at(key) >= domPerc)
					{
						domPerc = nextClass->at(key);
						dominantClass = m + 1;
					}
				}
			}

			//Record what the dominant class was.
			domClass.at(i).push_back({ key, dominantClass });

			//Draw the grey other class rectangle.
			glBegin(GL_QUADS);

			//Set Color
			glColor4d(0, 0, 0, 0.15);

			// draw bottom left
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
				(blockOffsetVertical)
			);

			// draw bottom right
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
				(blockOffsetVertical)
			);

			// draw top right
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
				((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
			);

			// draw top left
			glVertex2d(
				((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
				((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
			);

			glEnd();

			//CHECK IF WE NEED GREEN BORDER.
			//Get perc from user.
			double percFU = file->getPurityPerc();

			if ((domPerc) >= (percFU / 100) && !(freq <= 0.014))
			{
				//==Draw border==:
				glBegin(GL_LINE_STRIP);

				glColor4d(0, 255, 0, GRAY_VAL);
				glLineWidth(.5);

				// draw bottom left
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					(blockOffsetVertical - 4)
				);

				// draw bottom right
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					(blockOffsetVertical - 4)
				);

				// draw top right
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-4)
				);


				// draw top left
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-4)
				);

				// draw bottom left
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					(blockOffsetVertical - 4)
				);

				glEnd();
			}

			//Check if we need to draw the border.
			if (!(freq <= 0.014))
			{
				//==Draw border==:
				glBegin(GL_LINE_STRIP);

				glColor4d(0, 0, 0, GRAY_VAL);
				glLineWidth(.5);

				//Top Left Point:
				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
				);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
				);

				//Top Right Point:
				glEnd();

			}
			else //We must draw the border.
			{

				//==Draw Side Right==:
				glBegin(GL_LINE_STRIP);

				glColor4d(0, 0, 0, GRAY_VAL);

				glLineWidth(.5);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					(prevHeight - 2)
				);

				glEnd();

				//==Draw Side Left==:
				glBegin(GL_LINE_STRIP);

				glColor4d(0, 0, 0, GRAY_VAL);
				glLineWidth(.5);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					(prevHeight - 3)
				);

				glEnd();
				 
				if (j == curVec.size() - 1 || curVec.at(j+1).second > 0.014)
				{
					//==Draw border==:
					glBegin(GL_LINE_STRIP);

					glColor4d(0, 0, 0, GRAY_VAL);
					glLineWidth(100);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
					);

					glVertex2d(
						((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
					);

					glEnd();
				}

			}

			//Claculate the block Height:
			double currentBlockHeight = (freq * HEIGHT_OF_ALL_BLOCKS);
			const double INCDEC = (currentBlockHeight / 100) * 2;
			double middleBlockHeight = (currentBlockHeight / 2);
			double firstColorPosition = (middleBlockHeight) + blockOffsetVertical;
			double secondColorPosition = (middleBlockHeight) + blockOffsetVertical;

			//make sure specific color always goes on top.
			if (dominantClass == 2)
			{
				//Mutate vector to draw lines in next step.
				curVec[j].second = firstColorPosition;
				sortedByPurityVector[i] = curVec;
				middleOther.at(i).push_back({ key, secondColorPosition });
			}
			else
			{
				//Mutate vector to draw lines in next step.
				curVec[j].second = secondColorPosition;
				sortedByPurityVector[i] = curVec;
				middleOther.at(i).push_back({ key, firstColorPosition });
			}

			//Record the previous height.
			blockOffsetVertical += (((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)) - prevHeight);
			prevHeight = blockOffsetVertical;

		}

		//==Draw bottom border==:
		glBegin(GL_LINE_STRIP);

		glColor4d(0, 0, 0, 1);
		glLineWidth(.5);

		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
			(80)
		);

		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
			(80)
		);

		glEnd();

		dimensionCount++;
	}

	//Record vectors for line drawing.
	this->middleOther = middleOther;
	this->domClass = domClass;
	this->sortedVector = sortedByPurityVector;
}

//drawGrayLines:
//Desc: Algroithm for drawing lines in gray block visualization.
GLvoid DomNominalSet::drawGrayLines(double worldWidth)
{
	int dimensionCount = 0; // Variable for the dimension index.
	int colorChoice = file->getNominalColor();
	glLineWidth(3.0); //Seting line width.
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.

	vector<double> leftCoordinate = vector<double>();
	vector<double> rightCoordinate = vector<double>();
	vector<double> frequency = vector<double>();
	vector<double> classVec = vector<double>();
	vector<int> colorIdx = vector<int>();
	vector<int> leftData;
	vector<int> rightData;
	vector<boolean> remainingFullLines = vector<boolean>();
	bool alreadyExists = false;
	int numOfLinesSetTransparent = 0;
	int numSmallLines = 0; // count number of lines.
	int numNDPoints = 0;

	//Set up vector to make keep track of lines.
	for (int i = 0; i < this->file->getSetAmount(); i++)
	{
		remainingFullLines.push_back(true);
	}

	//Create Array of coordinates and Counts.
	for (int i = 0; i < file->getDimensionAmount() - 1; i++) // file->getDimensionAmount()
	{
		if (!(file->getDataDimensions()->at(i)->isVisible())) continue;
		if (file->getVisibleDimensionCount() < 2) break;

		leftCoordinate.clear();
		rightCoordinate.clear();
		frequency.clear();
		classVec.clear();
		colorIdx.clear();
		leftData.clear();
		rightData.clear();
		alreadyExists = false;

		int j = i + 1;
		while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
		if (j >= file->getDimensionAmount()) continue;

		//Itterate over sets.
		for (int k = 0; k < this->file->getSetAmount(); k++)
		{
			double left = file->getData(k, i);
			double right = file->getData(k, j);
			double classOfCur = file->getClassOfSet(k);

			//Get Dominant Class for both left and right.

			//Get current vecotor.
			vector<pair<double, double>> curVecLeft = this->sortedVector[i];
			vector<pair<double, double>> curVecRight = this->sortedVector[j];

			//Get dominant class vector:
			vector<pair<double, double>> domVecLeft = this->domClass[i];
			vector<pair<double, double>> domVecRight = this->domClass[j];

			//Get grey middle vector:
			vector<pair<double, double>> greyVecLeft = this->middleOther[i];
			vector<pair<double, double>> greyVecRight = this->middleOther[j];

			//Itterate over the vector to find the dominant class.
			double domClassNumLeft;
			for (int k = 0; k < domVecLeft.size(); k++)
			{
				pair<double, double> p = domVecLeft.at(k);

				if (p.first == left)
				{
					domClassNumLeft = p.second;
					break;
				}
			}

			//Itterate over the vector to find the dominant class.
			double domClassNumRight;
			for (int k = 0; k < domVecRight.size(); k++)
			{
				pair<double, double> p = domVecRight.at(k);

				if (p.first == right)
				{
					domClassNumRight = p.second;
					break;
				}
			}

			double leftPosition;
			double rightPosition;

			//Check where to draw the line for right and left (dominant class or non dominant class).
			if (classOfCur == domClassNumLeft)
			{
				//find value in curVec iterating over kv pairs:
				for (int k = 0; k < curVecLeft.size(); k++)
				{
					pair<double, double> p = curVecLeft.at(k);

					if (p.first == left)
					{
						leftPosition = p.second;
						break;
					}
				}
			}
			else
			{
				//find value in curVec iterating over kv pairs:
				for (int k = 0; k < greyVecLeft.size(); k++)
				{
					pair<double, double> p = greyVecLeft.at(k);

					if (p.first == left)
					{
						leftPosition = p.second;
						break;
					}
				}
			}

			if (classOfCur == domClassNumRight)
			{
				//find value in curVec iterating over kv pairs:
				for (int k = 0; k < curVecRight.size(); k++)
				{
					pair<double, double> p = curVecRight.at(k);

					if (p.first == right)
					{
						rightPosition = p.second;
						break;
					}
				}
			}
			else
			{
				//find value in curVec iterating over kv pairs:
				for (int k = 0; k < greyVecRight.size(); k++)
				{
					pair<double, double> p = greyVecRight.at(k);

					if (p.first == right)
					{
						rightPosition = p.second;
						break;
					}
				}
			}

			// see if exists already, if so increment count;else add and count = 1
			alreadyExists = false;
			for (int l = 0; l < frequency.size(); l++)
			{
				if (leftCoordinate[l] == leftPosition &&
					rightCoordinate[l] == rightPosition && classVec[l] == classOfCur)
				{
					alreadyExists = true;
					frequency[l] = frequency[l] + 1.0;
					break; // break out of l loop
				}
			} // end l loop

			if (!alreadyExists)
			{
				leftCoordinate.push_back(leftPosition);
				rightCoordinate.push_back(rightPosition);
				leftData.push_back(left);
				rightData.push_back(right);
				frequency.push_back(1.0);
				classVec.push_back(classOfCur);
				colorIdx.push_back(k);
			}

		} // end k loop

		//Keep track of how many small lines are shown.
		numSmallLines += frequency.size();

		// draw the data between these two dimensions, scaling width to count
		for (int k = 0; k < frequency.size(); k++)
		{
			int classVal = classVec.at(k);
			vector<double>* color = file->getSetColor(colorIdx[k]);

			double alpha = 1.0;// Alpha here incase we want to use it.
			double trans = file->getTranspLineThresh();
			double transparentPercent = (trans / 100.0);

			//If the line is less then 10% of the class.
			if (frequency[k] / file->getSetAmount(classVal) <= transparentPercent)
			{
				alpha = 0.1; //Make alpha low.
				numOfLinesSetTransparent++;//Increment the amount of transparent lines.

				//Get left and right data of this freq.
				int left = leftData[k];
				int right = rightData[k];

				//Itterate over the sets and check to see if any data matches the left and right coordinates.
				for (int m = 0; m < remainingFullLines.size(); m++)
				{
					//Get left and right of the current.
					double curLeft = file->getData(m, i);
					double curRight = file->getData(m, j);

					//Check to see if the current set is one of the sets being set transparent.
					if (curLeft == left && curRight == right)
					{
						remainingFullLines.at(m) = false;
					}

				}

			}

			//Check to see if we have less then percent, record:
			if (alpha != 0.1)
			{
				numNDPoints += frequency.at(k);
			}

			glColor4d((*color)[0], (*color)[1], (*color)[2], alpha);
			double width = (frequency[k] / (file->getSetAmount(classVal) / 15));
			glLineWidth(width);
			double leftData = leftCoordinate[k];
			double rightData = rightCoordinate[k];

			double leftCoordinateX = (-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1));
			leftCoordinateX = leftCoordinateX * -1;
			double rightCoordinateX = (-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 2));
			rightCoordinateX = rightCoordinateX * -1;

			//===Determine the equation of the perpendicular line=== (Y = mX + b);
			//Start with calculating the slope: (m)
			double slope = (leftData - rightData) / (leftCoordinateX - rightCoordinateX);
			double perpSlope = 0;
			if (slope != 0)
			{
				width = width / 2; //Adjust the width for drawing since the line width is drawn both ways from the center.

				//TWO CASES: 
				//1.) Slope > 0.
				//2.) Slope < 0.

				//Perpendicular slope is opposite reciprical.
				perpSlope = 1 / (slope * -1);

				if (slope < 0) // 1.) Slope > 0
				{
					//Determine what (b) is using one of the given points.
					double bLeft = leftData - (perpSlope * leftCoordinateX); //Where leftData is Y and leftCoordinateX is X.

					//We now have m and b. Thus, if we have a x position we can find a y position.
					//With our width as our distance, we will use the equation of a circle to calculate the x position
					//on the line that is distance d from the origin point (left point.

					//Calculate the x position.
					double newXPositionLeft;
					if (classVal == 2)
					{
						newXPositionLeft = leftCoordinateX + sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}
					else
					{
						newXPositionLeft = leftCoordinateX - sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}

					//Calculate the y position.
					double newYPositionLeft = (perpSlope * newXPositionLeft) + bLeft;

					//===DO THE SAME FOR THE RIGHT===

					//Determine what (b) is using one of the given points.
					double bRight = rightData - (perpSlope * rightCoordinateX); //Where leftData is Y and leftCoordinateX is X.

					//We now have m and b. Thus, if we have a x position we can find a y position.
					//With our width as our distance, we will use the equation of a circle to calculate the x position
					//on the line that is distance d from the origin point (left point.

					//Calculate the x position.
					double newXPositionRight = 0;
					if (classVal == 2)
					{
						newXPositionRight = rightCoordinateX + sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}
					else
					{
						newXPositionRight = rightCoordinateX - sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}

					//Calculate the y position.
					double newYPositionRight = (perpSlope * newXPositionRight) + bRight;

					// draw a line using both points
					glBegin(GL_LINE_STRIP);
					glVertex2d(-newXPositionLeft, newYPositionLeft);
					glVertex2d(-newXPositionRight, newYPositionRight);
					glEnd();

				}
				else //2.) SLOPE < 0
				{
					//Determine what (b) is using one of the given points.
					double bLeft = leftData - (perpSlope * leftCoordinateX); //Where leftData is Y and leftCoordinateX is X.

					//We now have m and b. Thus, if we have a x position we can find a y position.
					//With our width as our distance, we will use the equation of a circle to calculate the x position
					//on the line that is distance d from the origin point (left point.

					//Calculate the x position.
					double newXPositionLeft;
					if (classVal == 2)
					{
						newXPositionLeft = leftCoordinateX - sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}
					else
					{
						newXPositionLeft = leftCoordinateX + sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}

					//Calculate the y position.
					double newYPositionLeft = (perpSlope * newXPositionLeft) + bLeft;

					//===DO THE SAME FOR THE RIGHT===

					//Determine what (b) is using one of the given points.
					double bRight = rightData - (perpSlope * rightCoordinateX); //Where leftData is Y and leftCoordinateX is X.

					//We now have m and b. Thus, if we have a x position we can find a y position.
					//With our width as our distance, we will use the equation of a circle to calculate the x position
					//on the line that is distance d from the origin point (left point.

					//Calculate the x position.
					double newXPositionRight = 0;
					if (classVal == 2)
					{
						newXPositionRight = rightCoordinateX - sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}
					else
					{
						newXPositionRight = rightCoordinateX + sqrt((width * width) / (1 + (perpSlope * perpSlope)));
					}

					//Calculate the y position.
					double newYPositionRight = (perpSlope * newXPositionRight) + bRight;

					// draw a line using both points
					glBegin(GL_LINE_STRIP);
					glVertex2d(-newXPositionLeft, newYPositionLeft);
					glVertex2d(-newXPositionRight, newYPositionRight);
					glEnd();
				}

			}
			else //Slope == 0. Shift Y value.
			{
				if (width <= 2) width = 5;
				if (classVal == 2)
				{
					// draw a line using both points
					glBegin(GL_LINE_STRIP);
					glVertex2d(-leftCoordinateX, leftData + (width / 2));
					glVertex2d(-rightCoordinateX, rightData + (width / 2));
					glEnd();
				}
				else
				{
					// draw a line using both points
					glBegin(GL_LINE_STRIP);
					glVertex2d(-leftCoordinateX, leftData - (width / 2));
					glVertex2d(-rightCoordinateX, rightData - (width / 2));
					glEnd();
				}

			}

		}

		dimensionCount++;

	}

	//Add the number of lines set transparent to the dominant nominal sets.
	file->setDNSLinesTransparent(numOfLinesSetTransparent);
	file->setDNSNumSmallLines(numSmallLines);
	file->setDNSnDPointsVisualized(numNDPoints);

	int numFullSets = 0;
	for (int i = 0; i < remainingFullLines.size(); i++)
	{
		if (remainingFullLines.at(i) == true)
		{
			numFullSets++;
		}
	}

	file->setDNSNumSetsVisualized(numFullSets);
}

//MTBRuleGenerationAlgorithm
//Desc: Algorithm for generating all possible rule with combinations of coordinates using Monotonoicity / MTBChains.
//This one has easier requirements.
vector<DNSRule> DomNominalSet::MTBRuleGeneration(double PrecThresh, vector<int> group, double covThresh, int targetClass, int totalCasesInTarget)
{
	//Local Vars:
	string rulesToString;
	vector<int> casesCovered;
	vector<DNSRule> DNSRulesGenerated;
	vector<DNSRule> finalRules;
	bool ruleGenerated = false;
	double PRECISION_THRESHOLD = PrecThresh;//%
	double COVERAGE_THRESHOLD = covThresh;//%
	double highestCoverage = 0.0;
	vector<int> casesCoveredByAllRules;

	//Get MTBC object:
	MonotoneBooleanChains MTBC = MonotoneBooleanChains(group.size());

	//Loop until break.
	while (true)
	{
		//Get the first pair of coordinates to check for rules.
		string linkValue = MTBC.getNextLink();

		//If the entire chain is resolved.
		if (linkValue == "-1") break;
		else if (stoi(linkValue, 0, 2) == 0)//If the value is all zeros,
		{
			MTBC.giveAnswer(false);
			continue;
		}
		else //check chain.
		{
			double currentChainHighestCoverage = 0.0;

			//Determine what coordinates are being used for generating rules.
			vector<int> coordinatesToUse;
			for (int i = 0; i < linkValue.size(); i++)
			{
				if (linkValue.at(i) == '1') coordinatesToUse.push_back((group.at(i) - 1));
			}

			//Now we have a list of coordinates, we have to start with the first two and see if rules can be generated.
			//Then, move to the next and keep checking if rules can be generated.

			bool linkGeneratedRule = false;
			bool ruleGeneratedGreaterThen95 = false;
			
			vector<vector<double>> attributeCombinations;
			vector<vector<double>> newAttributeCombinations;

			//Iterate over the coordinates:
			for (int i = 0; i < coordinatesToUse.size(); i++)
			{
				int curCoordinateIndex = coordinatesToUse.at(i);
				unordered_map<double, double> curCoordinateBlocks; //We wil just be using the fist class.

				//Iterate over the number of classes and add any val.
				for (int j = 0; j < file->getClassAmount() - 2; j++)
				{
					unordered_map<double, double> curClassAttributesAtCoord = *(classPercPerBlock->at(curCoordinateIndex)->at(j));
					for (auto it = curClassAttributesAtCoord.begin(); it != curClassAttributesAtCoord.end(); it++)
					{
						double key = it->first;
						if (curCoordinateBlocks.find(key) == curCoordinateBlocks.end())
						{
							curCoordinateBlocks.insert({ key, 0 });
						}
					}
				}
				
				//If this is the first coordinate, just fill.
				if (i == 0)
				{
					//Add all possible attributes.
					for (auto blockIt = curCoordinateBlocks.begin(); blockIt != curCoordinateBlocks.end(); blockIt++)
					{
						vector<double> toAdd;
						toAdd.push_back(blockIt->first);
						attributeCombinations.push_back(toAdd);
					}
				}
				else //If this is not the first coordinate 
				{
					//Iterate over the current combinantions
					for (int j = 0; j < attributeCombinations.size(); j++)
					{
						vector<double> currentComb = attributeCombinations.at(j);

						//Iterate over the attributes in this coordinate.
						for (auto blockIt = curCoordinateBlocks.begin(); blockIt != curCoordinateBlocks.end(); blockIt++)
						{
							vector<double> toAdd = currentComb;
							toAdd.push_back(blockIt->first);
							newAttributeCombinations.push_back(toAdd);
						}

					}

					//Clear temp NewAtttributeCombos and reassign.
					attributeCombinations = newAttributeCombinations;
					newAttributeCombinations.clear();
				}

			}//End iterate over coords.

			//=====Check For Rules=====//

			//Iterate over the attribute combinantions.
			for (int i = 0; i < attributeCombinations.size(); i++)
			{
				
				//======Negation attribute combination Tests========//

				vector<double> currentAttributeCombinantion = attributeCombinations.at(i);

				for (int x = 0; x < currentAttributeCombinantion.size(); x++)
				{
					//Generate all further combinations.
					vector <double> curNegatedGroup;
					for (int y = x; y < currentAttributeCombinantion.size(); y++)
					{
						curNegatedGroup.push_back(y);

						vector<int> casesInRule;
						vector<int> correclyPredictedCases;
						int predictedCorrecly = 0;
						int predictedIncorrectly = 0;
						int predictedTotal = 0;
						double precision = 0.0;
						double coverage = 0.0;

						casesInRule.clear();//clear for each class.

						//Iterate over the sets:
						for (int k = 0; k < file->getSetAmount(); k++)
						{
							//Check if the set goes through each attirbute and is in the right class.
							bool goesToEachAttribute = true;
							double currentSetClass = file->getClassOfSet(k);

							for (int m = 0; m < coordinatesToUse.size(); m++)
							{
								//Check to see if this coord position contains one of our negated attribtues.
								bool isNegatedCoordAttri = false;
								for (auto it = curNegatedGroup.begin(); it != curNegatedGroup.end(); it++)
								{
									if (*it == m)
									{
										isNegatedCoordAttri = true;
										break;
									}
								}

								if (isNegatedCoordAttri)
								{
									// == for negation. This means that the set would not work.
									//double curVal = file->getData(k, coordinatesToUse.at(m));
									if (file->getData(k, coordinatesToUse.at(m)) == currentAttributeCombinantion.at(m))
									{
										goesToEachAttribute = false;
										break;
									}
								}
								else
								{
									if (file->getData(k, coordinatesToUse.at(m)) != currentAttributeCombinantion.at(m))
									{
										goesToEachAttribute = false;
										break;
									}
								}

							}//End of iterating over coords.

							double currentSetValueSee = file->getData(k, coordinatesToUse.at(0));

							//Record predicted.
							if (goesToEachAttribute && currentSetClass == targetClass)
							{
								predictedCorrecly++;
								predictedTotal++;
								casesInRule.push_back(k);
								correclyPredictedCases.push_back(k);
							}

							if (goesToEachAttribute && currentSetClass != targetClass)
							{
								predictedIncorrectly++;
								predictedTotal++;
								casesInRule.push_back(k);
							}

						}//End iterating over sets.

						linkGeneratedRule = true;

						//Check if the precision passes the threshold.
						if (predictedTotal != 0)
						{
							precision = (double(predictedCorrecly) / double(predictedTotal)) * 100.0;
							coverage = (double(predictedTotal) / double(totalCasesInTarget)) * 100.0;

							if (precision >= PRECISION_THRESHOLD && coverage >= COVERAGE_THRESHOLD)
							{
								//Generate correct attribute values.
								vector<double> decodedAttributeVals(currentAttributeCombinantion.size());
								for (int k = 0; k < coordinatesToUse.size(); k++)
								{
									for (int m = 0; m < file->getSetAmount(); m++)
									{
										double normalized = file->getData(m, coordinatesToUse.at(k));

										if (normalized == currentAttributeCombinantion.at(k))
										{
											decodedAttributeVals.at(k) = file->getOriginalData(m, coordinatesToUse.at(k));
										}
									}
								}

								//Generate correct negated attribute values.
								vector<double> decodedNegated;
								for (int k = 0; k < curNegatedGroup.size(); k++)
								{
									decodedNegated.push_back(curNegatedGroup.at(k));
								}

								DNSRule newRule;
								newRule.setAttributesUsed(decodedAttributeVals);
								newRule.setNegatedAttributesUsed(decodedNegated);
								newRule.setCoordinatesUsed(coordinatesToUse);
								newRule.setCasesUsed(casesInRule);
								newRule.setCorrectCases(predictedCorrecly);
								newRule.setIncorrectCases(predictedIncorrectly);
								newRule.setTotalCases(predictedTotal);
								newRule.setRuleClass(targetClass);
								newRule.setPrecision(precision);
								newRule.setTotalCoverage(coverage);
								newRule.setRuleClass(targetClass);
								DNSRulesGenerated.push_back(newRule);

								//Check to see if the precision is high enough to expand upwards.
								if (precision >= 95.0)
								{
									ruleGeneratedGreaterThen95 = true;
								}

								//Check to see if the coverage is the highest so far.
								if (coverage > currentChainHighestCoverage)
								{
									currentChainHighestCoverage = coverage;
								}

								//Record the newly covered cases to the total cases covered. Check to see if we
								//have reached full coverage of the target class. If so, return the current rules generated.

								//Iterate over the cases in the rule.
								for (int j = 0; j < correclyPredictedCases.size(); j++)
								{
									int curCaseID = correclyPredictedCases.at(j);
									bool notContained = true;
									for (int k = 0; k < casesCoveredByAllRules.size(); k++)
									{
										if (curCaseID == casesCoveredByAllRules.at(k))
										{
											notContained = false;
											break;
										}
									}

									//Add to the cases covered by all rules.
									if (notContained)
									{
										casesCoveredByAllRules.push_back(curCaseID);
									}
								}
							}
						}

						predictedCorrecly = 0;
						predictedIncorrectly = 0;
						predictedTotal = 0;
						precision = 0.0;

					}
				}

				//========================================================//

	
				//======Normal attribute combination Test========//
				currentAttributeCombinantion = attributeCombinations.at(i);
				vector<int> casesInRule;
				vector<int> correclyPredictedCases;
				int predictedCorrecly = 0;
				int predictedIncorrectly = 0;
				int predictedTotal = 0;
				double precision = 0.0;
				double coverage = 0.0;

				casesInRule.clear();//clear for each class.

					//Iterate over the sets:
				for (int k = 0; k < file->getSetAmount(); k++)
				{
					//Check if the set goes through each attirbute and is in the right class.
					bool goesToEachAttribute = true;
					double currentSetClass = file->getClassOfSet(k);

					for (int m = 0; m < coordinatesToUse.size(); m++)
					{

						if (file->getData(k, coordinatesToUse.at(m)) != currentAttributeCombinantion.at(m))
						{
							goesToEachAttribute = false;
							break;
						}

					}//End of iterating over coords.

					//Record predicted.
					if (goesToEachAttribute && (currentSetClass == (targetClass)))
					{
						predictedCorrecly++;
						predictedTotal++;
						casesInRule.push_back(k);
						correclyPredictedCases.push_back(k);
					}

					if (goesToEachAttribute && (currentSetClass != (targetClass)))
					{
						predictedIncorrectly++;
						predictedTotal++;
						casesInRule.push_back(k);
					}

				}//End iterating over sets.

				linkGeneratedRule = true;

				//Check if the precision passes the threshold.
				if (predictedTotal != 0)
				{
					precision = (double(predictedCorrecly) / double(predictedTotal)) * 100.0;
					coverage = (double(predictedTotal) / double(totalCasesInTarget)) * 100.0;

					if (precision >= PRECISION_THRESHOLD && coverage >= COVERAGE_THRESHOLD)
					{
						//Generate correct attribute values.
						vector<double> decodedAttributeVals(currentAttributeCombinantion.size());
						for (int k = 0; k < coordinatesToUse.size(); k++)
						{
							for (int m = 0; m < file->getSetAmount(); m++)
							{
								double normalized = file->getData(m, coordinatesToUse.at(k));

								if (normalized == currentAttributeCombinantion.at(k))
								{
									decodedAttributeVals.at(k) = file->getOriginalData(m, coordinatesToUse.at(k));
								}
							}
						}

						DNSRule newRule;
						newRule.setAttributesUsed(decodedAttributeVals);
						newRule.setCoordinatesUsed(coordinatesToUse);
						newRule.setCasesUsed(casesInRule);
						newRule.setCorrectCases(predictedCorrecly);
						newRule.setIncorrectCases(predictedIncorrectly);
						newRule.setTotalCases(predictedTotal);
						newRule.setRuleClass(targetClass);
						newRule.setPrecision(precision);
						newRule.setTotalCoverage(coverage);
						newRule.setRuleClass(targetClass);
						DNSRulesGenerated.push_back(newRule);

						if (precision >= 95.0)
						{
							ruleGeneratedGreaterThen95 = true;
						}

						if (coverage > currentChainHighestCoverage)
						{
							currentChainHighestCoverage = coverage;
						}

						//Record the newly covered cases to the total cases covered. Check to see if we
						//have reached full coverage of the target class. If so, return the current rules generated.

						//Iterate over the cases in the rule.
						for (int j = 0; j < correclyPredictedCases.size(); j++)
						{
							int curCaseID = correclyPredictedCases.at(j);
							bool notContained = true;
							for (int k = 0; k < casesCoveredByAllRules.size(); k++)
							{
								if (curCaseID == casesCoveredByAllRules.at(k))
								{
									notContained = false;
									break;
								}
							}

							//Add to the cases covered by all rules.
							if (notContained)
							{
								casesCoveredByAllRules.push_back(curCaseID);
							}
						}
					}
				}

				predictedCorrecly = 0;
				predictedIncorrectly = 0;
				predictedTotal = 0;
				precision = 0.0;

			}//End of iterating over attribute combinantions.

			//Answer the chain.
			if (linkGeneratedRule)
			{
				if (ruleGeneratedGreaterThen95 && currentChainHighestCoverage > highestCoverage)
				{
					MTBC.giveAnswer(true);
					highestCoverage = currentChainHighestCoverage;
				}
				else
				{
					MTBC.markAsEvaluated();
				}

			}

			//=========================//

		}//End if chain check.

		//Check if we have covered all cases.
		if (casesCoveredByAllRules.size() == totalCasesInTarget)
		{
			break;
		}

	}//End of While True.

	//=====Keep rules that satisfy thresholds=====//


	//Go over all rules generated.
	for (int i = 0; i < DNSRulesGenerated.size(); i++)
	{
		double currentRulePrecision = DNSRulesGenerated.at(i).getPrecision();
		double currentRuleCoverage = DNSRulesGenerated.at(i).getTotalCoverage();

		//If the rule suprpasses thresholds, record:
		if (currentRulePrecision >= PRECISION_THRESHOLD && currentRuleCoverage >= COVERAGE_THRESHOLD)
		{
			finalRules.push_back(DNSRulesGenerated.at(i));

			vector<int> curRuleCases = DNSRulesGenerated.at(i).getCasesUsed();

			//Add all cases to cases covered.
			for (int j = 0; j < curRuleCases.size(); j++)
			{
				bool notContained = true;
				for (int k = 0; k < casesCovered.size(); k++)
				{
					if (curRuleCases.at(j) == casesCovered.at(k))
					{
						notContained = false;
						break;
					}
				}

				if (notContained)
				{
					casesCovered.push_back(curRuleCases.at(j));
				}
			}
		}
	}

	//============================================//
	return(finalRules);

}//End of MTBRuleGeneration.

//MTBRuleGenResults
//Desc: Non sequential Rule Generation.
vector<string> DomNominalSet::MTBRuleGenResults(double precisionThresh, vector<vector<int>>groups, int targetClass)
{
	vector<string> toReturn;
	vector<int> allGroupCases;
	vector<DNSRule> allGroupRules;

	//Go over all groups.
	for (int n = 0; n < groups.size(); n++)
	{
		//////////////////////////////
		string toAdd;
		vector<int> casesCovered;
		vector<DNSRule> allGeneratedRules;
		const double COVERAGETHRESHOLD = 1.5;//%

		//Generate all possible rules.
		allGeneratedRules = MTBRuleGeneration(precisionThresh, groups.at(n), COVERAGETHRESHOLD, targetClass, 0);

		//Determine all cases covered by group.
		for (int i = 0; i < allGeneratedRules.size(); i++)
		{
			vector<int> curRuleCases = allGeneratedRules.at(i).getCasesUsed();

			for (int j = 0; j < curRuleCases.size(); j++)
			{
				bool isContained = false;
				for (int k = 0; k < casesCovered.size(); k++)
				{
					if (curRuleCases.at(j) == casesCovered.at(k))
					{
						isContained = true;
						break;
					}
				}

				if (!isContained)
				{
					casesCovered.push_back(curRuleCases.at(j));
				}

			}
		}

		//Add to all group data.
		for (int i = 0; i < allGeneratedRules.size(); i++)
		{
			vector<int> curRuleCases = allGeneratedRules.at(i).getCasesUsed();

			for (int j = 0; j < curRuleCases.size(); j++)
			{
				bool isContained = false;
				for (int k = 0; k < allGroupCases.size(); k++)
				{
					if (curRuleCases.at(j) == allGroupCases.at(k))
					{
						isContained = true;
						break;
					}
				}

				if (!isContained)
				{
					allGroupCases.push_back(curRuleCases.at(j));
				}

			}
		}

		//Determine cases for class 1 and class 2.
		int numCasesClass1 = 0;
		int numCasesClass2 = 0;
		int totalClass1 = 0;
		int totalClass2 = 0;
		for (int i = 0; i < file->getSetAmount(); i++)
		{
			int classCur = file->getClassOfSet(i);
			if (classCur == 1)
			{
				totalClass1++;
			}
			else if (classCur == 2)
			{
				totalClass2++;
			}
		}

		for (int i = 0; i < casesCovered.size(); i++)
		{
			int curClass = file->getClassOfSet(casesCovered.at(i));
			if (curClass == 1)
			{
				numCasesClass1++;
			}
			else if (curClass == 2)
			{
				numCasesClass2++;
			}
		}

		for (int i = 0; i < allGeneratedRules.size(); i++)
		{
			allGroupRules.push_back(allGeneratedRules.at(i));
		}
	
		//Record:
		toAdd = "Group = " + to_string(n + 1) + " , Precision = " + to_string(precisionThresh) + "%, Rules Used = " + to_string(allGeneratedRules.size()) + ", Cases Covered = " + to_string(casesCovered.size()) + ".\n" +
			"Cases Class 1: " + to_string(numCasesClass1) + " Cases Class 2: " + to_string(numCasesClass2) + " Cases Class 1 Total: " + to_string(totalClass1) + "\n";
		toReturn.push_back(toAdd);
	}

	//Determine how many cases of the target class are covered.
	int casesInTargetClass = 0;
	for (int i = 0; i < allGroupCases.size(); i++)
	{
		if (file->getClassOfSet(allGroupCases.at(i)) == targetClass)
		{
			casesInTargetClass++;
		}

	}

	toReturn.push_back(("\nAll Generated rules: " + to_string(allGroupRules.size()) + " All cases covered: " + to_string(allGroupCases.size()) + " Total target class cases: " + to_string(casesInTargetClass) + "\n"));
	return toReturn;
}

//ParetoFrontRUleGenWithOverlap:
//Desc: Generates all possible rules, rejects all rules over overlap perecision, finds Pareto front.
vector<string> DomNominalSet::ParetoFrontRuleGenWithOverlap(double precisionThresh, vector<vector<int>>groups, int targetClass)
{
	vector<string> toReturn;
	vector<int> allGroupCases;
	vector<DNSRule> allGroupRules;
	vector<int> coorainteAppearance;
	double totalCasesInTargetClass = 0;
	double totalCasesInData = file->getSetAmount();

	//Determine total number of cases in target class.
	for(int i = 0; i < file->getSetAmount(); i++)
	{
		if (file->getClassOfSet(i) == targetClass) totalCasesInTargetClass++;
	}

	//Setting up vector to count the number of times a coordinate is used.
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{
		coorainteAppearance.push_back(0);
	}

	//Go over all groups.
	for (int n = 0; n < groups.size(); n++)
	{
		//////////////////////////////
		string toAdd;
		vector<int> casesCovered;
		vector<DNSRule> paretoFrontRules;
		//const double COVERAGETHRESHOLD = 1.5;//%
		const double OVERLAPTHRESHOLD = 50.0;//% was 18

		//Determine number of sets in target class.
		double numTargetClassRemaning = 0;
		for (int i = 0; i < file->getSetAmount(); i++)
		{
			if (file->getClassOfSet(i) == targetClass) numTargetClassRemaning++;
		}

		//Calc coverage threshold:
		double calcCoverageThreshold = (((numTargetClassRemaning) / 10) / totalCasesInData) * 100.0;

		if (calcCoverageThreshold > 1.5  || n != -1) calcCoverageThreshold = 0.5;//TESTING!

		//Iterate over all dimensions.
		for (int m = 1; m <= groups.at(n).size(); m++)
		{
			//Each dimension variables.
			vector<int> dimensionsToUse;
			vector<DNSRule> allGeneratedRules;
			vector<DNSRule> curParetoFront;
			vector<int> curCasesCovered;
			
			//Determine what coordinates we are going to to use.
			for (int l = 0; l < m; l++)
			{
				dimensionsToUse.push_back(groups.at(n).at(l));
			}

			//Generate all possible rules for each subgroup.
			allGeneratedRules = MTBRuleGeneration(precisionThresh, dimensionsToUse, calcCoverageThreshold, targetClass, totalCasesInTargetClass);
			
			//Get all cases covered by all rules.
			vector<int> allCasesCoveredByGeneral;
			for (int i = 0; i < allGeneratedRules.size(); i++)
			{
				DNSRule currentRule = allGeneratedRules.at(i);
				vector<int> casesInCurrentRule = currentRule.getCasesUsed();

				for (int j = 0; j < casesInCurrentRule.size(); j++)
				{
					bool contained = false;
					for (int k = 0; k < allCasesCoveredByGeneral.size(); k++)
					{
						if (casesInCurrentRule.at(j) == allCasesCoveredByGeneral.at(k))
						{
							contained = true;
							break;
						}
					}

					if (!contained)
					{
						allCasesCoveredByGeneral.push_back(casesInCurrentRule.at(j));
					}
				}
			}

			//Calculate overlap for each rule.
			vector<pair<DNSRule, double>> rulesWithOverlap;
			for (int i = 0; i < allGeneratedRules.size(); i++)
			{
				DNSRule currentRule = allGeneratedRules.at(i);
				vector<int> casesInCurrentRule = currentRule.getCasesUsed();
				double overlapCount = 0;
				for (int j = 0; j < casesInCurrentRule.size(); j++)
				{
					for (int k = 0; k < allCasesCoveredByGeneral.size(); k++)
					{
						if (casesInCurrentRule.at(j) == allCasesCoveredByGeneral.at(k))
						{
							overlapCount++;
							break;
						}
					}
				}

				double totalOverlap = (overlapCount / allCasesCoveredByGeneral.size()) * 100.0;
				pair<DNSRule, double> ruleToAdd;
				ruleToAdd.first = currentRule;
				ruleToAdd.second = totalOverlap;
				rulesWithOverlap.push_back(ruleToAdd);

			}
			allGeneratedRules.clear();

			//Reject the rules with incorrect.
			for (int i = 0; i < rulesWithOverlap.size(); i++)
			{
				double overlap = rulesWithOverlap.at(i).second;
				DNSRule rule = rulesWithOverlap.at(i).first;

				if (overlap <= OVERLAPTHRESHOLD)//%
				{
					allGeneratedRules.push_back(rule);
				}
			}

			//Calculate pareto front for each dimension.
			curParetoFront = calculateParetoFront(allGeneratedRules);
			curParetoFront = trueConvex(curParetoFront);//Make sure pareto front is truly convex.
			
			//Determine all cases used.
			for (int i = 0; i < curParetoFront.size(); i++)
			{
				DNSRule currentRule = curParetoFront.at(i);
				vector<int> casesInCurrentRule = currentRule.getCasesUsed();

				for (int j = 0; j < casesInCurrentRule.size(); j++)
				{
					bool contained = false;
					for (int k = 0; k < curCasesCovered.size(); k++)
					{
						if (casesInCurrentRule.at(j) == curCasesCovered.at(k))
						{
							contained = true;
							break;
						}
					}

					if (!contained)
					{
						curCasesCovered.push_back(casesInCurrentRule.at(j));
					}
				}
			}

			//Determine if this pareto front is better.
			if (curCasesCovered.size() > casesCovered.size())
			{
				casesCovered = curCasesCovered;
				paretoFrontRules = curParetoFront;
			}
		}

		//record all final rules.
		for (int i = 0; i < paretoFrontRules.size(); i++)
		{
			allGroupRules.push_back(paretoFrontRules.at(i));
		}

		//Add cases to overall cases.
		for (int i = 0; i < casesCovered.size(); i++)
		{
			bool notContained = true;
			for (int j = 0; j < allGroupCases.size(); j++)
			{
				if (allGroupCases.at(j) == casesCovered.at(i))
				{
					notContained = false;
					break;
				}
			}

			if (notContained)
			{
				allGroupCases.push_back(casesCovered.at(i));
			}
		}

		//Determine cases for class 1 and class 2.
		int numCasesClass1 = 0;
		int numCasesClass2 = 0;
		int totalClass1 = 0;
		int totalClass2 = 0;
		for (int i = 0; i < file->getSetAmount(); i++)
		{
			int classCur = file->getClassOfSet(i);
			if (classCur == 1)
			{
				totalClass1++;
			}
			else if (classCur == 2)
			{
				totalClass2++;
			}
		}

		for (int i = 0; i < casesCovered.size(); i++)
		{
			int curClass = file->getClassOfSet(casesCovered.at(i));
			if (curClass == 1)
			{
				numCasesClass1++;
			}
			else if (curClass == 2)
			{
				numCasesClass2++;
			}
		}

		//Count the number of times each cordinate shows up.
		for (int i = 0; i < paretoFrontRules.size(); i++)
		{
			DNSRule r = paretoFrontRules.at(i);

			vector<int> ruleAttributes = r.getCoordinatesUsed();
			for (int j = 0; j < ruleAttributes.size(); j++)
			{
				//Increment count by one.
				coorainteAppearance.at(ruleAttributes.at(j)) = (coorainteAppearance.at(ruleAttributes.at(j)) + 1);
			}
		}


		//====================================================Record:======================================================//
		toAdd += "Coordinate Appearance = ";
		for (int i = 0; i < coorainteAppearance.size() - 1; i++)
		{
			toAdd += "X" + to_string(i + 1) + ": " + to_string(coorainteAppearance.at(i)) + ", ";
		}
		toAdd += "X" + to_string(coorainteAppearance.size() - 1) + ": " + to_string(coorainteAppearance.at(coorainteAppearance.size() - 1)) + "\n ";
		toAdd += "Group = " + to_string(n + 1) + " , Precision = " + to_string(precisionThresh) + "%, Rules Used = " + to_string(paretoFrontRules.size()) + ", Cases Covered = " + to_string(casesCovered.size()) + ".\n" +
			"Cases Class 1: " + to_string(numCasesClass1) + " Cases Class 2: " + to_string(numCasesClass2) + " Cases Class 1 Total: " + to_string(totalClass1) + "\n";
		toReturn.push_back(toAdd);
		//================================================================================================================//

		//Clear coordinate apperance vector for next group.
		coorainteAppearance.clear();
		for (int i = 0; i < file->getDimensionAmount(); i++)
		{
			coorainteAppearance.push_back(0);
		}

		/*
		vector<int> removedCases;

		//Remove cases used in this group.
		for (int i = 0; i < casesCovered.size(); i++)
		{
			//count how many cases below were deleted and re compute the index.
			int caseIndex = casesCovered.at(i);
			int numBelow = 0;
			for (int j = 0; j < removedCases.size(); j++)
			{
				if (removedCases.at(j) < caseIndex) numBelow++;
			}

			int computedCaseIndex = caseIndex - numBelow;

			file->deleteSet(computedCaseIndex);
			removedCases.push_back(caseIndex);
		}
		*/
	}

	//file->returnToOriginalDataDimensions();

	//Determine how many cases of the target class are covered.
	int casesInTargetClass = 0;
	for (int i = 0; i < allGroupCases.size(); i++)
	{
		if (file->getClassOfSet(allGroupCases.at(i)) == targetClass)
		{
			casesInTargetClass++;
		}
	}

	toReturn.push_back(("\nAll Generated rules: " + to_string(allGroupRules.size()) + " All cases covered: " + 
		to_string(allGroupCases.size()) + " Total target class cases: " + to_string(casesInTargetClass) + "\n"));
	this->casesToRemove = allGroupCases;
	return toReturn;
}

//calculateParetoFront:
//Desc: Calculates the pareto front of the passed rules using a convex hull.
vector<DNSRule> DomNominalSet::calculateParetoFront(vector<DNSRule> generatedRules)
{
	//Local Variables:
	DNSRule topLeftRule; //Smallest Coverage, Highest Precision.
	DNSRule bottomRightRule; //Largest Coverage, Smallest Precision.
	int topLeftIndex = -1;
	int bottomRightIndex = -1;
	vector<DNSRule> paretoFront; //Final rules to return;
	vector<DNSRule> potentialRules; //Rules for next iteration.
	vector<DNSRule> rulesToAddToFront; //Rules generated from recursive cases.
	double highestPrecision = 0.0;//%
	double highestCoverage = 0.0;//%
	double smallestPrecision = 100.0;//%
	double smallestCoverage = 100.0;//%

	//Base Case:
	if (generatedRules.size() < 3)
	{
		//If there are less then three points we have 3 cases,
		//1.) 2 rules, these will be picked as top left and bottom right.
		//2.) 1 rule, this can automatically be added to pareto front. 
		//3.) 0 rules, Nothing to do here.
		return generatedRules;
	}

	//1.) Determine top left and top right rule.
	//Top Left Rule:
	for (int i = 0; i < generatedRules.size(); i++)
	{
		double curRulePrecision = generatedRules.at(i).getPrecision();
		double curRuleCoverage = generatedRules.at(i).getTotalCoverage();

		if (curRulePrecision > highestPrecision && curRuleCoverage < smallestCoverage)
		{
			topLeftRule = generatedRules.at(i);
			topLeftIndex = i;
			highestPrecision = curRulePrecision;
			smallestCoverage = curRuleCoverage;
		}
	}

	//Bottom Right Rule:
	for (int i = 0; i < generatedRules.size(); i++)
	{
		double curRulePrecision = generatedRules.at(i).getPrecision();
		double curRuleCoverage = generatedRules.at(i).getTotalCoverage();

		if (curRulePrecision < smallestPrecision && curRuleCoverage > highestCoverage)
		{
			bottomRightRule = generatedRules.at(i);
			bottomRightIndex = i;
			smallestPrecision = curRulePrecision;
			highestCoverage = curRuleCoverage;
		}
	}

	//2.) Calculate the line between the two rules.
	//(Y = MX + B)
	double slope = -1;
	double b = -1;

	//Y2-Y1/X2-X1. 
	slope = (smallestPrecision - highestPrecision) / (highestCoverage - smallestCoverage);

	//B = Y - MX. Using Top Left point.
	b = highestPrecision - (slope * smallestCoverage);

	//3.) Retrive all rules above the calculated line using residuals.
	for(int i = 0; i < generatedRules.size(); i++)
	{
		if (i == topLeftIndex || i == bottomRightIndex) continue;
		double curRulePrecision = generatedRules.at(i).getPrecision();
		double curRuleCoverage = generatedRules.at(i).getTotalCoverage();

		//Calculate the X position of the current rule on the line.
		//X = Y - B / M
		double xPositionOnLine = (curRulePrecision - b) / slope;

		//If the X position of the rule is greater then or equal to the posiiton on the line,
		//Add it to potential rules.
		if (curRuleCoverage >= xPositionOnLine)
		{
			potentialRules.push_back(generatedRules.at(i));
		}
	}

	//4.) Repeat.
	rulesToAddToFront = calculateParetoFront(potentialRules);
	

	//5.) Check to see if the previously calculated top left and bottom right 
	//should be included in the newly created pareto front.

	if (rulesToAddToFront.size() != 0)
	{
		//If the previously calculated top left is higher then the next top left, keep.
		if (topLeftRule.getPrecision() > rulesToAddToFront.at(0).getPrecision())
		{
			paretoFront.push_back(topLeftRule);
		}

		//If the previously calculated bottom right is further then the next bottom right, keep.
		if (bottomRightRule.getTotalCoverage() > rulesToAddToFront.at(rulesToAddToFront.size() - 1).getTotalCoverage())
		{
			paretoFront.push_back(bottomRightRule);
		}
	}
	else //Base case previously reached.
	{
		paretoFront.push_back(topLeftRule);
		paretoFront.push_back(bottomRightRule);
	}

	//Add up pareto front.
	for (int i = 0; i < rulesToAddToFront.size(); i++)
	{
		paretoFront.push_back(rulesToAddToFront.at(i));
	}

	return paretoFront;
}

//trueConvex:
//Desc: A function to ensure that the pareto front is truly convex.
vector<DNSRule> DomNominalSet::trueConvex(vector<DNSRule> paretoFront)
{

	if (paretoFront.size() <= 2)
	{
		return paretoFront;
	}

	bool removed = true;
	while (removed)
	{
		removed = false;
		auto it = paretoFront.begin();
		for (int i = 0; i < paretoFront.size(); i += 2)
		{
			if (i >= paretoFront.size() - 2) break;
			DNSRule firstRule = paretoFront.at(i);
			DNSRule secondRule = paretoFront.at(i + 2);
			DNSRule middleRule = paretoFront.at(i + 1);

			//Cacluate the equation of the line between the two points.
			double slope = -1;
			double b = -1;

			//Y2-Y1/X2-X1. 
			slope = (secondRule.getPrecision() - firstRule.getPrecision()) / (secondRule.getTotalCoverage() - firstRule.getTotalCoverage());

			//B = Y - MX. Using Top Left point.
			b = firstRule.getPrecision() - (slope * firstRule.getTotalCoverage());

			//Cacluate the posision of X on the line for the middle point.
			//X = Y - B / M
			double xPosForMiddlePoint = (middleRule.getPrecision() - b) / slope;

			//If the x position on line is smaller, keep. Else remove.
			if (xPosForMiddlePoint > middleRule.getTotalCoverage())
			{
				paretoFront.erase((it + 1));
				removed = true;
				break;
			}
			it += 2;
		}
	}

	return paretoFront;
}

//tenFoldCrossValidation:
//Desc: Ten cross fold validation to calculate accuracy. 
vector<string> DomNominalSet::tenFoldCrossValidation(int targetClass)
{
	//Locat data:
	vector<string> toReturn;
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

		//Iterate over test partition and predict for our rules.
		double correctlyPredicted = 0;
		
		for (int caseIndex = 0; caseIndex < testPartition.size(); caseIndex++)
		{
			int curCase = testPartition.at(caseIndex);
			int curCaseClass = file->getClassOfSet(curCase);
			double valAt5 = file->getOriginalData(curCase, 4);
			double valAt9 = file->getOriginalData(curCase, 8);
			double valAt15 = file->getOriginalData(curCase, 14);
			double valAt19 = file->getOriginalData(curCase, 18);
			double valAt20 = file->getOriginalData(curCase, 19);
			double valAt21 = file->getOriginalData(curCase, 20);
			double valAt22 = file->getOriginalData(curCase, 21);

			//Rule definitions:
			if (valAt5 == 3 || valAt5 == 4 || valAt5 == 5 || valAt5 == 6 || valAt5 == 8 || valAt5 == 9)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredicted++;
				}
			}
			else if (valAt9 == 6 || valAt9 == 3)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredicted++;
				}
			}
			else if (valAt19 == 2 && valAt20 == 8 && valAt21 != 2 && valAt22 != 2)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredicted++;
				}
			}
			else if (valAt15 == 3 || valAt15 == 2 || valAt15 == 9)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredicted++;
				}
			}
			else if (valAt19 != 2 && valAt20 != 6 && valAt21 == 5 && valAt22 == 1)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredicted++;
				}
			}
			else if (valAt19 == 6 && valAt20 == 5 && valAt21 != 1 && valAt22 != 6)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredicted++;
				}
			}
			else if (valAt20 == 8 && valAt21 == 2 && valAt22 != 6)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredicted++;
				}
			}
		}

		//Iterate over test partition and perdict for competitiors rules.
		double correctlyPredictedComp = 0;


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
				}
			}
			else if (valAt20 == 5)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredictedComp++;
				}
			}
			else if (valAt8 == 2 && (valAt12 == 3 || valAt12 == 2) && valAt21 == 2)
			{
				if (curCaseClass == targetClass)
				{
					correctlyPredictedComp++;
				}
			}
		}

		//Accuracy calculation:
		double compAccuracy = (correctlyPredictedComp / numTargetClassCases) * 100.0;
		double ourAccuracy = (correctlyPredicted / numTargetClassCases) * 100.0;

		//Add to total to get average.
		totalOurAccuracy += ourAccuracy;
		totalCompAccuracy += compAccuracy;

		//Record test results.
		string toAdd = "Test " + to_string(i + 1) + ":\n";
		toAdd += "Accuracy for our rules = " + to_string(ourAccuracy) + "%\n";
		toAdd += "Accuracy for the competitors rules = " + to_string(compAccuracy) + "%\n\n";
		toReturn.push_back(toAdd);
	}

	double finalAverageOurRules = totalOurAccuracy / partitionNumber;
	double finalAverageCompRules = totalCompAccuracy / partitionNumber;

	string averages = "Our accurcy avrg = " + to_string(finalAverageOurRules) + "% Comp avrg = " + to_string(finalAverageCompRules) + "%\n\n";
	toReturn.push_back(averages);

	return toReturn;
}

//visualizeRules:
//Desc: Draws red boxes around block pairs that rules were created with.
GLvoid DomNominalSet::visualizeRules()
{
	//Variables for drawing:
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.
	const double BOXINCDEC = 6;//Value to make height of poly.

	//Determine what the offset of dimensions are since some may be hidden.
	int dimensionOffset = (this->file->getDimensionAmount() - this->file->getVisibleDimensionCount());
	int numDimensionsDrawn = 0;

	//Iterate over the dimensions.
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
	{
		//If this dimension is visible.
		if (file->isDimensionVisible(i))
		{
			numDimensionsDrawn++;

			//Check if this dimension has any rules.
			for (int j = 0; j < ruleData.size(); j++)
			{
				//Get the first rules data.
				pair<double, pair<double, double>> currentRule = ruleData.at(j);

				double firstCord = currentRule.first;
				double secondCord = firstCord + 1;

				//If this dimension is the current dimension we are looking for, draw rule.
				if (firstCord == i)
				{
					double leftPosition = currentRule.second.first;
					double rightPosition = currentRule.second.second;

					//If this is a single attribute rule,
					if (rightPosition == -1)
					{
						////Set Width:
						//GLdouble width = 3.0;
						//glLineWidth(width);

						////Set Color:
						//glColor4d(0, 0, 1.0, 1);

						////Draw the dominant set rectangle.
						//glBegin(GL_LINE_STRIP);

						//// draw bottom left
						//glVertex2d(
						//	((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))) - 3,
						//	(leftPosition - BOXINCDEC)
						//);

						//// draw top left
						//glVertex2d(
						//	((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))) - 3,
						//	(leftPosition + BOXINCDEC)
						//);

						//// draw top left
						//glVertex2d(
						//	((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))) + 3,
						//	(leftPosition + BOXINCDEC)
						//);

						//// draw bottom left
						//glVertex2d(
						//	((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))) + 3,
						//	(leftPosition - BOXINCDEC)
						//);

						//// draw bottom left
						//glVertex2d(
						//	((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))) - 3,
						//	(leftPosition - BOXINCDEC)
						//);

						//glEnd();
						drawOval(((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))), leftPosition + 20, 20, 40, 40);
					}
					else //Two attribute rule.
					{
						//Set Width:
						GLdouble width = 3.0;
						glLineWidth(width);

						//Set Color:
						glColor4d(0, 0, 1.0, 1);

						//Draw the dominant set rectangle.
						glBegin(GL_LINE_STRIP);

						// draw bottom left
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))),
							(leftPosition - BOXINCDEC)
						);

						// draw top left
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))),
							(leftPosition + BOXINCDEC)
						);

						// draw top right
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn + 1))),
							(rightPosition + BOXINCDEC)
						);

						// draw bottom right
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn + 1))),
							(rightPosition - BOXINCDEC)
						);

						// draw bottom left
						glVertex2d(
							((-worldWidth / 2.0) + ((xAxisIncrement) * (numDimensionsDrawn))),
							(leftPosition - BOXINCDEC)
						);

						glEnd();
					}
				}
			}
		}

	}

	/*

	//Iterate over the rule data:
	for (int i = 0; i < ruleData.size(); i++) // file->getDimensionAmount()
	{
		//Get the first rules data.
		pair<double, pair<double, double>> currentRule = ruleData.at(i);

		double firstCord = currentRule.first;
		double secondCord = firstCord + 1;

		double leftPosition = currentRule.second.first;
		double rightPosition = currentRule.second.second;

		//Set Width:
		GLdouble width = 3.0;
		glLineWidth(width);

		//Set Color:
		glColor4d(0, 0, 1.0, 1);

		//Draw the dominant set rectangle.
		glBegin(GL_LINE_STRIP);

		// draw bottom left
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (firstCord + 1 - dimensionOffset))),
			(leftPosition - BOXINCDEC)
		);

		// draw top left
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (firstCord + 1 - dimensionOffset))),
			(leftPosition + BOXINCDEC)
		);

		// draw top right
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (secondCord + 1 - dimensionOffset))),
			(rightPosition + BOXINCDEC)
		);

		// draw bottom right
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (secondCord + 1 - dimensionOffset))),
			(rightPosition - BOXINCDEC)
		);

		// draw bottom left
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (firstCord + 1 - dimensionOffset))),
			(leftPosition - BOXINCDEC)
		);

		glEnd();

	}

	*/
}

//getRuleData:
//Desc: Getter for Rule Data.
vector<pair<double, pair<double, double>>> DomNominalSet::getRuleData()
{
	return ruleData;
}

//getGeneratedMTBRuleData:
//Desc: Getter for description of rules generated after MTBRG is finished. (Used by OpenGL).
string DomNominalSet::getGeneratedMTBRuleData()
{
	return this->generatedRuleDataMTBRG;
}

//drawOval:
//Desc: Draws an oval at the given position with the given size.
GLvoid DomNominalSet::drawOval(float x_center, float y_center, float w, float h, int n)
{
	double PI = 2* acos(0.0);

	float theta, angle_increment;
	float x, y;
	if (n <= 0)
		n = 1;
	angle_increment = (2*PI) / n;
	glPushMatrix();

	//  center the oval at x_center, y_center
	glTranslatef(x_center, y_center, 0);
	//  draw the oval using line segments
	glBegin(GL_LINE_LOOP);

	for (theta = 0.0f; theta < (2 * PI); theta += angle_increment)
	{
		x = w / 2 * cos(theta);
		y = h / 2 * sin(theta);

		glColor4d(0, 0, 1.0, 1);
		glVertex2f(x, y);
	}
	glEnd();
	glPopMatrix();
}