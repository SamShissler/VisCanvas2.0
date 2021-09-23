#include <vector>
#include <string>
#include "stdafx.h"

using namespace std;

class MonotoneBooleanChains
{
public:
	//Constructors:
	MonotoneBooleanChains();
	MonotoneBooleanChains(int numD);
	void generateChains(int numD);
	string getNextLink();
	void giveAnswer(bool ans);
	void markAsEvaluated();

private:

	//Chain sorting / resolving.
	void sortChains();
	void fillResults();
	void fillStatus();
	void expand(bool ans);
	void smartExpand(bool ans);
	void testExpand(bool ans);
	bool isResolved();

	//Private Member values.
	vector<vector<string>> MTBChains; //Vector of vectors holding chains.
	vector<vector<bool>> results; //Vector of vectors holding the answers to each link.
	vector<vector<bool>> chainStatus; //Vector of vector holding status of links being answered.
	int currentChainIndex = -1; //Value to record what chain was last got from.
	int currentLinkIndex = -1; //Value to record what link was last retrived.
	int answeredQuestionVal = -1;

};
