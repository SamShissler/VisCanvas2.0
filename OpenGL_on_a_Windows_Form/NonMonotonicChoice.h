#pragma once
#include <string>
#include <vector>
#include <string>
#include <msclr\marshal_cppstd.h>
#include "NonMonotonicInfliction.h"

using namespace std;

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class NonMonotonicChoice : public System::Windows::Forms::Form
	{
	public: NonMonotonicChoice(vector<double> *passedAttributes, int coordinateNum)
	{
		this->coordinateNumber = coordinateNum;
		this->coordinateAttributes = passedAttributes;
		this->MonotonizedAttributes = new vector<pair<double, double>>();
		InitializeComponent();

		this->infoBox->Text = "There are two options when converting a non-monotonic coordinate.\n\n"
			+ "The first is to flip all of the attributes in the coordinate. Flipping a coordinate is commonly used when attribute values are normalized from 0 to 1. "
			+ "For example, if the original attribute A = 5 is encoded to 0.45, then it would be flipped to -0.45. \n\n"
			+ "The second is to use an infliction point and either split the data into two coordinates using that point or merge the data together. \n"
			+ "Flipping the attributes is used when a coordinate grows in the opposite direction of the target. "
			+ "Infliction point is used when the data has a specific scale and a desired start. ";

		this->infoBox->ReadOnly = true;
	}

	protected:~NonMonotonicChoice()
	{
		if (components)
		{
			delete components;
		}
	}


	private:System::String^ path;
	private: System::Windows::Forms::Button^ flipButton;
	private: System::Windows::Forms::Button^ inflectionButton;
	private: System::Windows::Forms::Label^ userPrompt;
	private: System::ComponentModel::Container^ components;
	private: System::Windows::Forms::RichTextBox^ infoBox;

	private: vector<pair<double, double>>* MonotonizedAttributes;
	private: vector<double>* coordinateAttributes;
	private: int coordinateNumber;

	private: bool infliction = false;
	private: bool inflictionKind = false;

	vector<pair<double, double>>* leftCordSplit;
	vector<pair<double, double>>* rightCordSplit;



#pragma region

		   void InitializeComponent(void)
		   {
			   this->flipButton = (gcnew System::Windows::Forms::Button());
			   this->inflectionButton = (gcnew System::Windows::Forms::Button());
			   this->userPrompt = (gcnew System::Windows::Forms::Label());
			   this->infoBox = (gcnew System::Windows::Forms::RichTextBox());
			   this->SuspendLayout();
			   // 
			   // confirmButton
			   // 
			   this->flipButton->Location = System::Drawing::Point(35, 140);
			   this->flipButton->Name = L"flipButton";
			   this->flipButton->Size = System::Drawing::Size(75, 23);
			   this->flipButton->TabIndex = 1;
			   string textStr = "Flip";
			   this->flipButton->Text = gcnew String(textStr.c_str());
			   this->flipButton->UseVisualStyleBackColor = true;
			   this->flipButton->Click += gcnew System::EventHandler(this, &NonMonotonicChoice::onFlip);
			   // 
			   // inflectionButton
			   // 
			   this->inflectionButton->Location = System::Drawing::Point(110, 140);
			   this->inflectionButton->Name = L"inflectionButton";
			   this->inflectionButton->Size = System::Drawing::Size(100, 23);
			   this->inflectionButton->TabIndex = 1;
			   this->inflectionButton->Text = L"Use Inflection";
			   this->inflectionButton->UseVisualStyleBackColor = true;
			   this->inflectionButton->Click += gcnew System::EventHandler(this, &NonMonotonicChoice::onInflection);
			   // 
			   // infoBox
			   // 
			   this->infoBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->infoBox->Location = System::Drawing::Point(235, 12);
			   this->infoBox->Name = L"infoBox";
			   this->infoBox->Size = System::Drawing::Size(375, 228);
			   this->infoBox->TabIndex = 0;
			   this->infoBox->Text = L"";
			   //
			   // userPrompt
			   //
			   this->userPrompt->AutoSize = true;
			   this->userPrompt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->userPrompt->Location = System::Drawing::Point(25, 90);
			   this->userPrompt->Name = L"userPrompt";
			   this->userPrompt->Size = System::Drawing::Size(154, 90);
			   this->userPrompt->TabIndex = 1;
			   string prompt = "How would you like to monotonize\ncoordinate X" + to_string(this->coordinateNumber) + "?";
			   this->userPrompt->Text = gcnew String (prompt.c_str());
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(620, 250);
			   this->Controls->Add(this->flipButton);
			   this->Controls->Add(this->inflectionButton);
			   this->Controls->Add(this->userPrompt);
			   this->Name = "Monotonization Choice";
			   this->Text = "Monotonization Choice";
			   this->Controls->Add(this->infoBox);
			   this->ResumeLayout(false);

		   }

#pragma endregion
	private: System::Void onInflection(System::Object^ sender, System::EventArgs^ e)
	{
		//Ask for infliction point and what kind of addition they want to do.
		CppCLRWinformsProjekt::NonMonotonicInfliciton^ InflictionWindow = gcnew CppCLRWinformsProjekt::NonMonotonicInfliciton();
		InflictionWindow->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedDialog;
		InflictionWindow->ShowDialog();

		bool kindOfInfliction = InflictionWindow->getKindBool();
		double inflPoint = InflictionWindow->getInflictionPoint();

		//Determine how the user wants to deal with the infliction point.
		if (kindOfInfliction == false) //SPLIT
		{
			vector<pair<double, double>>* leftCord = new vector<pair<double, double>>();
			vector<pair<double, double>>* rightCord = new vector<pair<double, double>>();

			//Iterate over attributes.
			for (int i = 0; i < coordinateAttributes->size(); i++)
			{
				double curAttribute = coordinateAttributes->at(i);
				
				pair<double, double> newPair;
				newPair.first = curAttribute;
				newPair.second = curAttribute;

				pair<double, double> emptyPair;
				emptyPair.first = curAttribute;
				emptyPair.second = -DBL_MAX;

				//If less or equal, add to left cord and right nothing.
				if (curAttribute <= inflPoint)
				{
					leftCord->push_back(newPair);
					rightCord->push_back(emptyPair);
				}
				else
				{
					leftCord->push_back(emptyPair);
					rightCord->push_back(newPair);
				}
			}

			leftCordSplit = leftCord;
			rightCordSplit = rightCord;

			//Mark that a split was used.
			inflictionKind = false;
		}
		else if (kindOfInfliction == true) // MERGE
		{

			sort(this->coordinateAttributes->begin(), this->coordinateAttributes->end());

			//Mark that a merge was used.
			inflictionKind = true;
		}

		//Mark that infliction was used.
		this->infliction = true;
		this->Close();
	}
	private: System::Void onFlip(System::Object^ sender, System::EventArgs^ e)
	{
		//flip all attributes:
		vector<pair<double, double>> *pairsOfResults = new vector<pair<double,double>>();
		for (int i = 0; i < coordinateAttributes->size(); i++)
		{
			pair<double, double> curPair;
			curPair.first = coordinateAttributes->at(i);
			curPair.second = (coordinateAttributes->at(i)) * -1;
			pairsOfResults->push_back(curPair);
		}
		this->MonotonizedAttributes = pairsOfResults;

		this->Close();
	}
	public: vector<pair<double,double>>* getResult()
	{
		return(this->MonotonizedAttributes);
	}

	public: vector<pair<double, double>>* getSplitLeftCord()
	{
		return (this->leftCordSplit);
	}

	public: vector<pair<double, double>>* getSplitRightCord()
	{
		return (this->rightCordSplit);
	}

	public: bool getiInflictionBool()
	{
		return this->infliction;
	}

	public: bool getiInflictionKind()
	{
		return this->inflictionKind;
	}

	};
}
