#include "stdafx.h"
#include "NormalizationStyle.h"

// /*
System::Void CppCLRWinformsProjekt::NormalizationStyle::MinMax(System::Object^ sender, System::EventArgs^ e)
{
	style = 0;
	this->Close();
}

System::Void CppCLRWinformsProjekt::NormalizationStyle::zScore(System::Object^ sender, System::EventArgs^ e)
{
	style = 1;
	this->Close();
}

int CppCLRWinformsProjekt::NormalizationStyle::getStyle()
{
	return style;
}