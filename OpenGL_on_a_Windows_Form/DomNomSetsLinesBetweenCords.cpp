#include "stdafx.h"
#include "DomNomSetsLinesBetweenCords.h"

//Default constructor.
DomNomSetsLinesBetweenCords::DomNomSetsLinesBetweenCords()
{

}

//=====Getters and Setters=====//
vector<double> DomNomSetsLinesBetweenCords::getLeftCoordinate()
{
	return leftCoordinate;
}

void DomNomSetsLinesBetweenCords::setLeftCoordinate(vector<double> toSet)
{
	leftCoordinate = toSet;
}

vector<double> DomNomSetsLinesBetweenCords::getRightCoordinate()
{
	return rightCoordinate;
}

void DomNomSetsLinesBetweenCords::setRightCoordinate(vector<double> toSet)
{
	rightCoordinate = toSet;
}

vector<double> DomNomSetsLinesBetweenCords::getFreqency()
{
	return frequency;
}

void DomNomSetsLinesBetweenCords::setFrequency(vector<double> toSet)
{
	frequency = toSet;
}

vector<double> DomNomSetsLinesBetweenCords::getClassVec()
{
	return classVec;
}

void DomNomSetsLinesBetweenCords::setClassVec(vector<double> toSet)
{
	classVec = toSet;
}

vector<int> DomNomSetsLinesBetweenCords::getcolorIdx()
{
	return colorIdx;
}

void DomNomSetsLinesBetweenCords::setColorIdx(vector<int> toSet)
{
	colorIdx = toSet;
}

vector<int> DomNomSetsLinesBetweenCords::getLeftData()
{
	return leftData;
}
void DomNomSetsLinesBetweenCords::setLeftData(vector<int> toSet)
{
	leftData = toSet;
}

vector<int> DomNomSetsLinesBetweenCords::getRightData()
{
	return rightData;
}

void DomNomSetsLinesBetweenCords::setRightData(vector<int> toSet)
{
	rightData = toSet;
}