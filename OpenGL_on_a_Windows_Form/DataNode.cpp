#include "stdafx.h"
#include "DataNode.h"

// create a node with data of 0.0
DataNode::DataNode() {
	currentData = 0.0;
	originalData = 0.0;
}

// create a node with data of the passed value(data)
DataNode::DataNode(double data) {
	currentData = data;
	originalData = data;
}

// delete the object
DataNode::~DataNode() {
}

// gets the data held by the node
double DataNode::getData() const {
	return currentData;
}

// gets the data that the node was originally set to hold
double DataNode::getOriginalData() const {
	return originalData;
}


// sets the data the node holds
void DataNode::setData(double newData) {
	currentData = newData;
	originalData = newData;
}

// sets the data the node holds
void DataNode::setDataCurrent(double newData) {
	currentData = newData;
}

// sets the data to the original
void DataNode::resetData() {
	currentData = originalData;
}

// multiplies the data held by the node, by the passed value(multiplier)
void DataNode::multiplyData(double multiplier) {
	currentData *= multiplier;
}

// divides the data held by the node, by the passed value(divisor)
void DataNode::divideData(double divisor) {
	currentData /= divisor;
}

// add the passed value(addend) to the data held by the node
void DataNode::addToData(double addend) {
	currentData += addend;
}

// returns pointer of passed value.
DataNode* DataNode::copyOfDataNode()
{
	DataNode* toReturn = new DataNode();
	toReturn->setData(this->originalData);
	toReturn->setDataCurrent(this->getData());
	return(toReturn);
}