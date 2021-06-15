#include "stdafx.h"
#include "NominalColorPicker.h"

System::Void CppCLRWinformsProjekt::NominalColorPicker::redColor(System::Object^ sender, System::EventArgs^ e)
{
	result = 1;
	this->Close();
}

System::Void CppCLRWinformsProjekt::NominalColorPicker::blueColor(System::Object^ sender, System::EventArgs^ e)
{
	result = 3;
	this->Close();
}

System::Void CppCLRWinformsProjekt::NominalColorPicker::greenColor(System::Object^ sender, System::EventArgs^ e)
{
	result = 2;
	this->Close();
}

System::Void CppCLRWinformsProjekt::NominalColorPicker::allColor(System::Object^ sender, System::EventArgs^ e)
{
	result = 5;
	this->Close();
}

System::Void CppCLRWinformsProjekt::NominalColorPicker::grayColor(System::Object^ sender, System::EventArgs^ e)
{
	result = 4;
	this->Close();
}

int CppCLRWinformsProjekt::NominalColorPicker::getResult()
{
	return result;
}