#ifndef DOMNOMSETSLINE_H
#define DOMNOMSETSLINE_H

#include <vector>
using namespace std;

public class DomNomSetsLinesBetweenCords
{

public:
	
	//Constructors:
	DomNomSetsLinesBetweenCords();

	//Getters and setters:
	vector<double> getLeftCoordinate();
	void setLeftCoordinate(vector<double>);
	vector<double> getRightCoordinate();
	void setRightCoordinate(vector<double>);
	vector<double> getFreqency();
	void setFrequency(vector<double>);
	vector<double> getClassVec();
	void setClassVec(vector<double>);
	vector<int> getcolorIdx();
	void setColorIdx(vector<int> toSet);
	vector<int> getLeftData();
	void setLeftData(vector<int> toSet);
	vector<int> getRightData();
	void setRightData(vector<int> toSet);

private:
	vector<double> leftCoordinate;
	vector<double> rightCoordinate;
	vector<double> frequency;
	vector<double> classVec;
	vector<int> colorIdx;
	vector<int> leftData;
	vector<int> rightData;

};

#endif