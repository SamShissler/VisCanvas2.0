#include <vector>

using namespace std;

public class DNSRule
{
public:
	
	//Consturctors:
	DNSRule();

	//Methods:
	void addAttribute(double);
	void addCoordinate(int);
	double getMostRecentAttribute();

	//Getters and Setters:
	void setAttributesUsed(vector<double>);
	vector<double> getAttributesUsed();
	void setCoordinatesUsed(vector<int>);
	vector<int> getCoordinatesUsed();
	void setCorrectCases(int);
	int getCorrectCases();
	void setIncorrectCases(int);
	int getIncorrectCases();
	void setTotalCases(int);
	int getTotalCases();
	void setRuleClass(int);
	int getRuleClass();
	void setHasChanged(bool);
	bool getHasChanged();

private:

	vector<double> attributesUsed; //list of attributes used.
	vector<int> coordinatesUsed;
	int correctCases; //Total correct cases in the rule.
	int incorrectCases; //Total incorrect cases in the rule.
	int totalCases; //Total cases in the rule.
	int ruleClass; //the class the rule predicts.
	bool hasChanged = false; //records if a rule has been changed between coordinates.

};//End of class.