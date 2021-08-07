#include "stdafx.h"
#include "DomNominalSet.h"
#include <msclr\marshal_cppstd.h>

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
}

//ReCalculateData:
void DomNominalSet::reCalculateData()
{
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

	//=====================================================


	//Itterate over dimensions.
	for (int i = 0; i < file->getDimensionAmount(); i++)
	{
		//Get the dimension data.
		vector<unordered_map<double, double>*>* curDimensionVec = toReturn->at(i);
		unordered_map<double, double> dimensionCount;

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

	//===Normailize data===

	//Iterate over vector:
	for (int i = 0; i < this->file->getDimensionAmount(); i++)
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

	return toReturn;
}

// getValuePerClassFreq
// Desc: Gets how often classes show up for each frequency.
vector<vector<unordered_map<double, double>*>*>* DomNominalSet::getValuePerClassFreq()
{
	vector<vector<unordered_map<double, double>*>*>* toReturn = new vector<vector<unordered_map<double, double>*>*>();

	//Itterate over number of dimensions.
	for (int j = 0; j < this->file->getDimensionAmount(); j++)
	{
		//Add the vector of classes to the dimension.
		toReturn->push_back(new vector<unordered_map<double, double>*>());

		//Add classes to vector for this dimension.
		for (int k = 0; k < file->getClassAmount() - 2; k++) // -2 because of Default and 'class' Column.
		{
			toReturn->at(j)->push_back(new unordered_map<double, double>());
		}
	}

	//Itterate over all attributes:
	for (int i = 0; i < file->getDimensionAmount(); i++)
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

//drawVisualization:
//Desc: Draws the Dominant Nominal Sets Visualization by first drawing rectangles then lines.
GLvoid DomNominalSet::drawVisualization()
{
	//Get rule data from file (Seperate object used to generate rule data.)
	ruleData = file->getRuleData();

	//Refresh the calculated positions:
	reCalculateData();

	//Check to see if the dimension ammount is greater than 15.
	if (file->getDimensionAmount() >= 15)
	{
		//If the coordinates are being reordered or shifted, only draw blocks.
		//This is to keep the program from lagging too much.
		
		if (file->getReOrderMode() == false && file->getShiftMode() == false)
		{
			drawRectangles(this->sortedByPurityVector, this->classPercPerBlock, this->worldWidth);
			drawLines(this->worldWidth);
			if (ruleData.size() != 0 && file->getDNSRuleVisualizationMode()) visualizeRules();

		}
		else
		{
			drawRectangles(this->sortedByPurityVector, this->classPercPerBlock, this->worldWidth);
		}
	}
	else
	{
		//If less than 15 dimensions:
		drawRectangles(this->sortedByPurityVector, this->classPercPerBlock, this->worldWidth);
		drawLines(this->worldWidth);
		if (ruleData.size() != 0 && file->getDNSRuleVisualizationMode()) visualizeRules();
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

			//(  (freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2) -  ((((domPerc * freq)* HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)) / 2))

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
	//Sort coordinates by their number of purity blocks.
	vector<string> toReturn;

	//Record the number of rules.
	int ruleCount = 1;

	//Record what cases are in this rule.
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
								pair<double, double> values;
								values.first = left;
								values.second = right;

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
								pair<double, double> values;
								values.first = left;
								values.second = right;

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

								//Figure out the position of the gray block.
								double grayBlockposition = 0;

								for (int m = 0; m < middleOther.at(j).size(); m++)
								{
									pair<double, double> curPair = middleOther.at(i + 1).at(m);
									if (curPair.first == right)grayBlockposition = curPair.second;
								}

								//Record rule in rule data.
								pair<double, double> values;
								values.first = left;
								values.second = right;

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

//linguisticDesc
//Desc: Creates the linguistic description for the visualization. Result is a formatted string to be added to the linguistic desc.
string DomNominalSet::linguisticDesc()
{
	//String to add to the descrption:
	string toReturn = "--- Dominant Nominal Sets ---\n";

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

	//Get Rules.
	vector<string> rules = determineRules();

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

//visualizeRules:
//Desc: Draws red boxes around block pairs that rules were created with.
GLvoid DomNominalSet::visualizeRules()
{
	//Variables for drawing:
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.
	const double BOXINCDEC = 15;//Value to make height of poly.

	//Iterate over the dimensions:
	for (int i = 0; i < ruleData.size(); i++) // file->getDimensionAmount()
	{
		//Get the first rules data.
		pair<double, pair<double, double>> currentRule = ruleData.at(i);

		double firstCord = currentRule.first;
		double secondCord = firstCord + 1;

		double leftData = currentRule.second.first;
		double rightData = currentRule.second.second;

		double leftPosition = 0;
		double rightPosition = 0;

		//Check to see if the rule contains more blocks.
		if (i != ruleData.size() - 1)
		{
			//If the rule lines are coming from the same coordinate and have the same value
			if (ruleData.at(i + 1).first == firstCord && ruleData.at(i + 1).second.first == leftData)
			{
				int x = x + 1;
			}
		}

		//Determine the positions.
		vector<pair<double, double>> currentVecLeft = sortedVector.at(firstCord);
		for (int j = 0; j < currentVecLeft.size(); j++)
		{
			if (currentVecLeft.at(j).first == leftData)
			{
				leftPosition = currentVecLeft.at(j).second;
				break;
			}
		}

		vector<pair<double, double>> currentVecRight = sortedVector.at(secondCord);
		for (int j = 0; j < currentVecRight.size(); j++)
		{
			if (currentVecRight.at(j).first == rightData)
			{
				rightPosition = currentVecRight.at(j).second;
				break;
			}
		}

		//Set Width:
		GLdouble width = 3.0;
		glLineWidth(width);

		//Set Color:
		glColor4d(1.0, 0, 0, 1);

		//Draw the dominant set rectangle.
		glBegin(GL_LINE_STRIP);

		// draw bottom left
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (firstCord + 1))),
			(leftPosition - BOXINCDEC)
		);

		// draw top left
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (firstCord + 1))),
			(leftPosition + BOXINCDEC)
		);

		// draw top right
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (secondCord + 1))),
			(rightPosition + BOXINCDEC)
		);

		// draw bottom right
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (secondCord + 1))),
			(rightPosition - BOXINCDEC)
		);

		// draw bottom left
		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (firstCord + 1))),
			(leftPosition - BOXINCDEC)
		);

		glEnd();

	}
}

//getRuleData:
//Desc: Getter for Rule Data.
vector<pair<double, pair<double, double>>> DomNominalSet::getRuleData()
{
	return ruleData;
}