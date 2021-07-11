#include "stdafx.h"
#include "DomNominalSet.h"
#include <msclr\marshal_cppstd.h>

DomNominalSet::DomNominalSet()
{
	file = nullptr;
}

//Constructor:
DomNominalSet::DomNominalSet(DataInterface *file, int worldHeight)
{
	this->file = file;
	this->worldHeight = worldHeight;
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

GLvoid DomNominalSet::drawVisualization(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth)
{
	drawRectangles(sortedByPurityVector, classPercPerBlock, worldWidth);
	drawLines(worldWidth);
}

GLvoid DomNominalSet::drawRectangles(vector<vector<pair<double, double>>> sortedByPurityVector, vector<vector<unordered_map<double, double>*>*>* classPercPerBlock, double worldWidth)
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
		double blockOffsetVertical = (80 + (file->getDimensionShift(i) * (this->worldHeight * 0.5) )); //Account for shifting.
		double prevHeight = (80 + (file->getDimensionShift(i) * (this->worldHeight * 0.5) ));

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

				glColor4d(0, 0, 0, GRAY_VAL);

				glLineWidth(.5);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
					((freq* HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
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
					((freq* HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
				);

				glVertex2d(
					((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
					(prevHeight - 3)
				);

				glEnd();

				if (j == curVec.size() - 1)
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
			((80 + (file->getDimensionShift(i) * (this->worldHeight * 0.5)))) //Accounts for shift.
		);

		glVertex2d(
			((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
			((80 + (file->getDimensionShift(i) * (this->worldHeight * 0.5)))) //Accounts for shift.
		);

		glEnd();

		dimensionCount++;
	}

	//Record vectors for line drawing.
	this->middleOther = middleOther;
	this->domClass = domClass;
	this->sortedVector = sortedByPurityVector;
}


//drawLines
//Desc: Draws line sections between coordinates in the OPENGL window.
GLvoid DomNominalSet::drawLines(double worldWidth)
{
	int dimensionCount = 0; // Variable for the dimension index.
	int colorChoice = file->getNominalColor();
	glLineWidth(3.0); //Seting line width.
	double xAxisIncrement = worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.0

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



		// draw the data between these two dimensions, scaling width to count
		for (int k = 0; k < frequency.size(); k++)
		{
			//Keep track of how many small lines are shown.
			numSmallLines += frequency.size();

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

			glColor4d((*color)[0], (*color)[1], (*color)[2], alpha);
			double width = (frequency[k] / (file->getSetAmount(classVal) / 15));
			glLineWidth(width);
			double leftData = leftCoordinate[k];
			double rightData = rightCoordinate[k];

			// draw a line using both points
			glBegin(GL_LINE_STRIP);
			glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
				(leftData));
			glVertex2d((-worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 2)),
				(rightData));
			glEnd();
		}

		dimensionCount++;

	}

	//Add the number of lines set transparent to the dominant nominal sets.
	file->setDNSLinesTransparent(numOfLinesSetTransparent);
	file->setDNSNumSmallLines(numSmallLines);

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


//determineRules
//Desc: Iterates over data and determines logical classification rules to add to
// the linguistic description.
vector<string> DomNominalSet::determineRules()
{
	//Sort coordinates by their number of purity blocks.
	vector<string> toReturn;
	vector<vector<unordered_map<double, double>*>*>* valueFreqPerClass = getValuePerClassFreq();
	vector<vector<unordered_map<double, double>*>*>* classPercPerBlock = getClassPercPerBlock(valueFreqPerClass);

	//Iterate over the dimensions.
	for (int i = 0; i < file->getDimensionAmount() - 1; i++)
	{
		//Make sure we don't go out of bounds.
		if (!(file->getDataDimensions()->at(i)->isVisible())) continue;
		if (file->getVisibleDimensionCount() < 2) break;

		int j = i + 1;
		while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
		if (j >= file->getDimensionAmount()) continue;

		//Itterate over sets.
		for (int k = 0; k < this->file->getSetAmount(); k++)
		{
			double left = file->getData(k, i);
			double right = file->getData(k, j);
			double classOfCur = file->getClassOfSet(k) - 1;

			//Get the purity percentage of each left and right blocks.
			double purityLeftBlock = classPercPerBlock->at(i)->at(classOfCur)->at(left);
			double purityRightBlock = classPercPerBlock->at(j)->at(classOfCur)->at(right);

			//Get Cord names:
			string firstCord = *(file->getDimensionName(i));
			string secondCord = *(file->getDimensionName(j));

			//Going from dominantly other class to dominantly current class, line is current class.
			if (purityLeftBlock <= 0.15 && purityRightBlock >= 0.85)
			{
				boolean exist = false;
				int otherClass = 1;
				if (file->getClassOfSet(k) == 1) otherClass = 2;

				//Make sure the string has not been added.
				string solutionOne = "If A is in " + firstCord + " in block dominantly class " + to_string(otherClass) + " and A is in " + secondCord +
					" in block dominantly class " + to_string(file->getClassOfSet(k)) + " then A is " + to_string(file->getClassOfSet(k)) + ".\n";

				string solutionTwo = "If A is in " + firstCord + " in block dominantly class " + to_string(otherClass) + " and A is not in " + secondCord +
					" in block dominantly class " + to_string(file->getClassOfSet(k)) + " then A is " + to_string(otherClass) + ".\n";

				for (int m = 0; m < toReturn.size(); m++)
				{
					if (toReturn.at(m) == solutionOne || toReturn.at(m) == solutionTwo)
					{
						exist = true;
						break;
					}
				}

				if (!exist)
				{
					toReturn.push_back(solutionOne);
					toReturn.push_back(solutionTwo);
				}
			}

			//Going from dominantly current class to dominantly other class, line is other class.
			else if (purityLeftBlock >= 0.85 && purityRightBlock <= 0.15)
			{
				boolean exist = false;
				int otherClass = 1;
				if (file->getClassOfSet(k) == 1) otherClass = 2;

				//Make sure the string has not been added.
				string solutionOne = "If A is in " + firstCord + " in block dominantly class " + to_string(file->getClassOfSet(k)) + " and A is in " + secondCord +
					" in block dominantly class " + to_string(otherClass) + " then A is " + to_string(otherClass) + ".\n";

				string solutionTwo ="If A is in " + firstCord + " in block dominantly class " + to_string(file->getClassOfSet(k)) + " and A is not in " + secondCord +
					" in block dominantly class " + to_string(otherClass) + " then A is " + to_string(file->getClassOfSet(k)) +".\n";

				for (int m = 0; m < toReturn.size(); m++)
				{
					if (toReturn.at(m) == solutionOne || toReturn.at(m) == solutionTwo)
					{
						exist = true;
						break;
					}
				}

				if (!exist)
				{
					toReturn.push_back(solutionOne);
					toReturn.push_back(solutionTwo);
				}

			}

		}
	
	}

	return toReturn;

}

//drawGrayRectangles
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

					//Top Right Point:
					glEnd();
				}

			}


			//Calculate the top third position.

			//Block Height:
			double currentBlockHeight = (freq * HEIGHT_OF_ALL_BLOCKS);

			//Get a third of the block height:
			double thirdBlockHeight = currentBlockHeight / 3;

			//Calculate a sixth block height:
			double sixthBlockHeight = currentBlockHeight / 6;

			//Calcualte the first color position:
			double firstColorPosition = sixthBlockHeight + blockOffsetVertical;

			//Calculate the second color position:
			double secondColorPosition = (thirdBlockHeight * 2)+ sixthBlockHeight + blockOffsetVertical;

			//Mutate vector to draw lines in next step.
			curVec[j].second = firstColorPosition;
			sortedByPurityVector[i] = curVec;
			middleOther.at(i).push_back({ key, secondColorPosition });

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
