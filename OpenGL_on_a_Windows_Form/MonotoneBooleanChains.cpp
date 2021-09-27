
#include "stdafx.h"
#include "MonotoneBooleanChains.h"


//Default constructor:
MonotoneBooleanChains::MonotoneBooleanChains()
{

}

//Constructor:
MonotoneBooleanChains::MonotoneBooleanChains(int numD)
{
	generateChains(numD);//Generate the chains for this dimension and save to MTBChains.
}

//generateChains:
//Desc: generates MTB chains for a specified dimension.
void MonotoneBooleanChains::generateChains(int numD)
{
	MTBChains.clear();//Make sure any other data is deleted.
	
	for (int i = 0; i <= numD - 1; i++)
	{
		vector<string> newChain;

		//If this is the starting chain, start with <0,1>.
		if (i == 0)
		{
			newChain.push_back("0");
			newChain.push_back("1");
			MTBChains.push_back(newChain);
			continue;
		}

		//Vector to hold new chains.
		vector<vector<string>> newHanselChains;

		//Go over current chains and adjust to make new Hansel Chain.

		//Create the min chain.
		vector<vector<string>>vectorEjMin;
		for (int k = 0; k < MTBChains.size(); k++)
		{
			vector<string> curChain = MTBChains.at(k);
			for (int m = 0; m < curChain.size(); m++)
			{
				curChain.at(m) = "0" + curChain.at(m);
			}
			vectorEjMin.push_back(curChain);
		}

		//Create the max chain.
		vector<vector<string>>vectorEjMax;
		for (int k = 0; k < MTBChains.size(); k++)
		{
			vector<string> curChain = MTBChains.at(k);
			for (int m = 0; m < curChain.size(); m++)
			{
				curChain.at(m) = "1" + curChain.at(m);
			}
			vectorEjMax.push_back(curChain);
		}

		//Adjust the min chain by adding the largest element from the last chain.
		//Remove the largest element from the max chain.

		for (int k = 0; k < vectorEjMax.size(); k++)
		{
			vectorEjMin.at(k).push_back(vectorEjMax.at(k).at(vectorEjMax.at(k).size() - 1));
			vectorEjMax.at(k).pop_back();
		}

		//Clear hanselChains and add vectors together.
		newHanselChains.clear();
		vector<vector<string>> unionVector;
		for (int k = 0; k < vectorEjMin.size(); k++)
		{
			if (vectorEjMin.at(k).size() != 0)
			{
				unionVector.push_back(vectorEjMin.at(k));
			}
		}

		for (int k = 0; k < vectorEjMax.size(); k++)
		{
			if (vectorEjMax.at(k).size() != 0)
			{
				unionVector.push_back(vectorEjMax.at(k));
			}
		}

		MTBChains = unionVector;
	}

	sortChains();
	fillResults();
	fillStatus();
}

//sortChains:
//Desc: Sorts chains in order of least links to most.
void MonotoneBooleanChains::sortChains()
{
	vector<pair<int, vector<string>>> chainSizeList;

	//Sort the chains by making pairs of chains and sizes.
	for (int i = 0; i < MTBChains.size(); i++)
	{
		vector<string> currentChain = MTBChains.at(i);
		int currentChainSize = currentChain.size();
		pair<int, vector<string>> toAdd;
		toAdd.first = currentChainSize;
		toAdd.second = currentChain;
		chainSizeList.push_back(toAdd);
	}

	sort(chainSizeList.begin(), chainSizeList.end());

	//Re-assign hansel chains to object:
	MTBChains.clear();
	for (int i = 0; i < chainSizeList.size(); i++)
	{
		MTBChains.push_back(chainSizeList.at(i).second);
	}
}

//fillResults:
//Desc: Fills results vector with initial values.
void MonotoneBooleanChains::fillResults()
{
	for (int i = 0; i < MTBChains.size(); i++)
	{
		vector<string> currentChain = MTBChains.at(i);
		vector<bool> currentChainResults;
		for (int j = 0; j < MTBChains.at(i).size(); j++)
		{
			currentChainResults.push_back(NULL);
		}
		results.push_back(currentChainResults);
	}//End of iteration over hanselChains.
}

//fillStatus:
//Desc: Fills satus with initial values.
void MonotoneBooleanChains::fillStatus()
{
	for (int i = 0; i < MTBChains.size(); i++)
	{
		vector<string> currentChain = MTBChains.at(i);
		vector<bool> currentChainStatus;
		for (int j = 0; j < MTBChains.at(i).size(); j++)
		{
			currentChainStatus.push_back(false);
		}
		chainStatus.push_back(currentChainStatus);
	}//End of iteration over hanselChains.
}

//getNextLink:
//Desc: Gets the next link to check from. This is simmilar to checking next oracle.
string MonotoneBooleanChains::getNextLink()
{
	//If our chains are not resolved.
	if(!isResolved())
	{
		int smallestChainSize = 0;

		//Find the smallest chain size.
		for (int i = 0; i < chainStatus.size(); i++)
		{
			bool resolvedFlag = true;
			vector<bool> currentChainStatus = chainStatus.at(i);
			for (int j = 0; j < currentChainStatus.size(); j++)
			{
				if (currentChainStatus.at(j) == false)
				{
					resolvedFlag = false;
					break;
				}
			}//End of current chain status.

			if (resolvedFlag == false)
			{
				smallestChainSize = currentChainStatus.size();
				break;
			}
		}//End of all chain status.

		//Choose the chain to ask from:
		vector<string> chainToAskFrom;
		int chainIndex = 0;
		for (int i = 0; i < MTBChains.size(); i++)
		{
			if (MTBChains.at(i).size() == smallestChainSize)
			{
				bool resolvedFlag = true;

				for (int j = 0; j < MTBChains.at(i).size(); j++)
				{
					if (chainStatus.at(i).at(j) == false)
					{
						resolvedFlag = false;
						break;
					}
				}

				if (resolvedFlag == false)
				{
					chainToAskFrom = MTBChains.at(i);
					chainIndex = i;
					break;
				}
			}

		}

		//Ask from min value.
		//Ask the smallest question out of the smallest chains.
		for (int i = 0; i < chainToAskFrom.size(); i++)
		{
			if (chainStatus.at(chainIndex).at(i) == false)
			{
				//Record the chain and current index for link.
				currentChainIndex = chainIndex;
				currentLinkIndex = i;
				answeredQuestionVal = stoi(chainToAskFrom.at(i), 0, 2);

				return(chainToAskFrom.at(i));

			}//End of checking current link for status.
		}//End of iterating over current chain.

	}
	else
	{
		return "-1";
	}

}

//isResolved:
//Desc: Checks if the entire chain is resolved.
bool MonotoneBooleanChains::isResolved()
{
	for (int i = 0; i < chainStatus.size(); i++)
	{
		for (int j = 0; j < chainStatus.at(i).size(); j++)
		{
			if (chainStatus.at(i).at(j) == false) return false;
		}
	}
	return true;
}

//giveAnswer
//Desc: records answer for the previous link retrived.
void MonotoneBooleanChains::giveAnswer(bool ans)
{
	results.at(currentChainIndex).at(currentLinkIndex) = ans; //Record answer.
	chainStatus.at(currentChainIndex).at(currentLinkIndex) = true; //Mark as answered.
	testExpand(ans);
}

//expand
//Desc: Expands to other links in other chains.
void MonotoneBooleanChains::expand(bool ans)
{
	//Expand results.
	if (ans == false) //Expand below.
	{
		//Go over chains.
		for (int j = 0; j < MTBChains.size(); j++)
		{
			vector<string> curChain = MTBChains.at(j);

			//If the bottom value in the chain is less then answered 
			//value, expand until it value less then is not found.
			int firstVal = stoi(curChain.at(0), 0, 2);

			if (firstVal < answeredQuestionVal)
			{
				//Go up list.
				for (int k = 0; k <= curChain.size() - 1; k++)
				{
					//If value in chain is less than answered value.
					int curVal = stoi(curChain.at(k), 0, 2);
					if (curVal < answeredQuestionVal)
					{
						chainStatus.at(j).at(k) = true;
						results.at(j).at(k) = ans;
					}
					else break;//If higher we are done with this chain.
				}
			}
		}
	}
	else//ans == true;
	{
		//Go over chains.
		for (int j = 0; j < MTBChains.size(); j++)
		{
			vector<string> curChain = MTBChains.at(j);

			//If the top value in the chain is higher then answered 
			//value, expand until it value higher then is not found.
			int firstVal = stoi(curChain.at(curChain.size() - 1), 0, 2);

			if (firstVal > answeredQuestionVal)
			{
				//Go down list.
				for (int k = MTBChains.at(j).size() - 1; k >= 0; k--)
				{
					//If value in chain is greater than answered value.
					int curVal = stoi(curChain.at(k), 0, 2);
					if (curVal > answeredQuestionVal)
					{
						chainStatus.at(j).at(k) = true;
						results.at(j).at(k) = ans;
					}
					else break;//If smaller we are done with this chain.
				}
			}
		}
	}
}

void MonotoneBooleanChains::testExpand(bool ans)
{
	//Expand results.
	if (ans == false) //Expand below.
	{
		//Go over chains.
		for (int j = 0; j < MTBChains.size(); j++)
		{
			vector<string> curChain = MTBChains.at(j);

			//If the bottom value in the chain is less then answered 
			//value, expand until it value less then is not found.
			int firstVal = stoi(curChain.at(0), 0, 2);

			if (firstVal < answeredQuestionVal)
			{
				//Go up list.
				for (int k = 0; k <= curChain.size() - 1; k++)
				{
					//If value in chain is less than answered value.
					int curVal = stoi(curChain.at(k), 0, 2);
					if (curVal < answeredQuestionVal)
					{
						chainStatus.at(j).at(k) = true;
						results.at(j).at(k) = ans;
					}
					else break;//If higher we are done with this chain.
				}
			}
		}
	}
	else//ans == true;
	{
		//How we expand:
		//1.) Find the right most 1 in the answered chain. Ex. 000110(1)000
		//2.) Mark as true and answered all values that are the past the 1. Ex. 0001101111.

		string answeredLink = MTBChains.at(currentChainIndex).at(currentLinkIndex);

		//1.) Find right most 1.
		int rightmostOnePosition = -1;
		for (int j = 0; j < answeredLink.size(); j++)
		{
			if (answeredLink[j] == '1')
			{
				rightmostOnePosition = j;
			}
		}

		//2.) Mark all other values as checked,
		//Go over chains.
		for (int j = 0; j < MTBChains.size(); j++)
		{
			vector<string> curChain = MTBChains.at(j);

			//Go down list.
			for (int k = MTBChains.at(j).size() - 1; k >= 0; k--)
			{
				string linkToCheck = MTBChains.at(j).at(k);

				//Check if chains match:
				bool correctBits = true;
				for (int m = 0; m <= rightmostOnePosition; m++)
				{
					if (linkToCheck.at(m) != answeredLink.at(m)) correctBits = false;
				}

				if (correctBits)
				{
					chainStatus.at(j).at(k) = true;
					results.at(j).at(k) = ans;
				}

			}
		}
	}

}


//smartExpand:
//Desc: expands downwards to every value if the link has two 1's.
void MonotoneBooleanChains::smartExpand(bool ans)
{
	//First, we need to check what kind of link we are getting the answer for.
	//There are three types of links.
	//1.) a link with more than 2 1's.
	//2.) a link with less then 2 1's.
	//3.) a link with exactly 2 1's.
	int kindOfLink = -1;

	//Go over current link to expand to. If it has the correct number of 1s, 
	// then dont expand over.
	int numOf1s = 0;
	for (int l = 0; l < MTBChains.at(currentChainIndex).at(currentLinkIndex).size(); l++)
	{
		if (MTBChains.at(currentChainIndex).at(currentLinkIndex).at(l) == '1') numOf1s++;
	}

	if (numOf1s > 2)
	{
		kindOfLink = 1;
	}
	else if (numOf1s < 2)
	{
		kindOfLink = 2;
	}
	else
	{
		kindOfLink = 3;
	}

	//If the kind of link is either 1 or 2, we will be marking these as false. However, we do not want 
	//to mark all potential link 3 as false. This way we are able to check these paairs.

	//Expand results.
	if (ans == false) //Expand below.
	{
		//Go over chains.
		for (int j = 0; j < MTBChains.size(); j++)
		{
			vector<string> curChain = MTBChains.at(j);

			//If the bottom value in the chain is less then answered 
			//value, expand until it value less then is not found.
			int firstVal = stoi(curChain.at(0), 0, 2);

			if (firstVal < answeredQuestionVal)
			{
				//Go up list.
				for (int k = 0; k <= curChain.size() - 1; k++)
				{
					//If value in chain is less than answered value.
					int curVal = stoi(curChain.at(k), 0, 2);
					if (curVal < answeredQuestionVal)
					{
						//Go over current link to expand to. If it has the correct number of 1s, 
						// then dont expand over.
						int numOf1s = 0;
						for (int l = 0; l < curChain.at(k).size(); l++)
						{
							if (curChain.at(k).at(l) == '1') numOf1s++;
						}
						
						if (kindOfLink == 3)//If the current link is a pair, we can mark everything below it as false.
						{
							chainStatus.at(j).at(k) = true;
							results.at(j).at(k) = ans;
						}
						else if (kindOfLink != 3 && numOf1s != 2)//We are marking as false since link cant be evalueated. the next link is not a pair. mark as false.
						{
							chainStatus.at(j).at(k) = true;
							results.at(j).at(k) = ans;
						}
						else if (kindOfLink != 3 && numOf1s == 2) //Non evaluable link checking to a pair.
						{
							//Do nothing...
						}
					}
					else break;//If higher we are done with this chain.
				}
			}
		}
	}
	else //Expand above.
	{
		//Go over chains.
		for (int j = 0; j < MTBChains.size(); j++)
		{
			vector<string> curChain = MTBChains.at(j);

			//If the top value in the chain is higher then answered 
			//value, expand until it value higher then is not found.
			int firstVal = stoi(curChain.at(curChain.size() - 1), 0, 2);

			if (firstVal > answeredQuestionVal)
			{
				//Go down list.
				for (int k = MTBChains.at(j).size() - 1; k >= 0; k--)
				{
					//If value in chain is greater than answered value.
					int curVal = stoi(curChain.at(k), 0, 2);
					if (curVal > answeredQuestionVal)
					{
						//Go over current link to expand to. If it has the correct number of 1s, 
						// then dont expand over.
						int numOf1s = 0;
						for (int l = 0; l < curChain.at(k).size(); l++)
						{
							if (curChain.at(k).at(l) == '1') numOf1s++;
						}

						//We are marking a group above as done.
						if (numOf1s != 2)
						{
							chainStatus.at(j).at(k) = true;
							results.at(j).at(k) = ans;
						}
						else
						{
							//This is a pair that is set as true. Must be evaluated.
						}
					}
					else break;//If smaller we are done with this chain.
				}
			}
		}
	}
}

//markAsFalse
//Desc: Marks current link as evaluated if we cannot determine rules for
// the size. EX. 011010 (there are 3 1's)
void MonotoneBooleanChains::markAsEvaluated()
{
	chainStatus.at(currentChainIndex).at(currentLinkIndex) = true;
	results.at(currentChainIndex).at(currentLinkIndex) = true;
}