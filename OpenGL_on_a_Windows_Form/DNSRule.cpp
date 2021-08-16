#include "stdafx.h"
#include "DNSRule.h"

//Default constructor.
DNSRule::DNSRule()
{
	
}

//=====Methods=====//

void DNSRule::addAttribute(double attribute)
{
	attributesUsed.push_back(attribute);
}

void DNSRule::addCoordinate(int cord)
{
	coordinatesUsed.push_back(cord);
}

double DNSRule::getMostRecentAttribute()
{
	return this->attributesUsed.at(attributesUsed.size() - 1);
}

//=====Getters and Setters=====//

void DNSRule::setAttributesUsed(vector<double> toSet)
{
	this->attributesUsed = toSet;
}

vector<double> DNSRule::getAttributesUsed()
{
	return attributesUsed;
}

void DNSRule::setCoordinatesUsed(vector<int> toSet)
{
	this->coordinatesUsed = toSet;
}

vector<int> DNSRule::getCoordinatesUsed()
{
	return coordinatesUsed;
}

void DNSRule::setCorrectCases(int toSet)
{
	this->correctCases = toSet;
}

int DNSRule::getCorrectCases()
{
	return correctCases;
}

void DNSRule::setIncorrectCases(int toSet)
{
	this->incorrectCases = toSet;
}

int DNSRule::getIncorrectCases()
{
	return incorrectCases;
}

void DNSRule::setTotalCases(int toSet)
{
	this->totalCases = toSet;
}

int DNSRule::getTotalCases()
{
	return totalCases;
}

void DNSRule::setRuleClass(int toSet)
{
	this->ruleClass = toSet;
}

int DNSRule::getRuleClass()
{
	return ruleClass;
}

void DNSRule::setHasChanged(bool toSet)
{
	this->hasChanged = toSet;
}

bool DNSRule::getHasChanged()
{
	return hasChanged;
}