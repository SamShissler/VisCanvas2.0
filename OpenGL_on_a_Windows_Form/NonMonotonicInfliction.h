#pragma once
#include <string>

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class NonMonotonicInfliciton : public System::Windows::Forms::Form
	{
	public: NonMonotonicInfliciton()
	{
		InitializeComponent();

		this->infoBox->Text = "There are two options when using an inflecton point.\n\n"
			+ "The first is to split the attributes into two coordinates. "
			+ "For example, if an attribute is <= the inflection point, "
			+ "it will be in the first coordinate. If an attribute is > "
			+ "the inflection point, then it will go in the second "
			+ "coordinate. If an attribute is not in a coordinate, it is "
			+ "stored as 'empty'.\n\n" 
			+"The second is to merge the values together around the "
			+ "inflection point. The new attribut values are stored in the "
			+ "followin way, If A < IP then 2IP, If A > IP then 2A - IP, "
			+ "If A = IP then IP.";

		this->infoBox->ReadOnly = true;

	}

	protected:~NonMonotonicInfliciton()
	{
		if (components)
		{
			delete components;
		}
	}


	private:System::String^ path;
	private: System::Windows::Forms::Button^ MergeButton;
	private: System::Windows::Forms::Button^ SplitButton;
	private: System::Windows::Forms::Label^ userPrompt;
	private: System::Windows::Forms::TextBox^ inputFeild;
	private: System::ComponentModel::Container^ components;
	private: System::Windows::Forms::RichTextBox^ infoBox;

	private: bool merge = false;
	private: double inflictionPoint = -1;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->MergeButton = (gcnew System::Windows::Forms::Button());
			   this->SplitButton = (gcnew System::Windows::Forms::Button());
			   this->userPrompt = (gcnew System::Windows::Forms::Label());
			   this->inputFeild = (gcnew System::Windows::Forms::TextBox());
			   this->infoBox = (gcnew System::Windows::Forms::RichTextBox());
			   this->SuspendLayout();
			   // 
			   // MergeButton
			   // 
			   this->MergeButton->Location = System::Drawing::Point(55, 140);
			   this->MergeButton->Name = L"MergeButton";
			   this->MergeButton->Size = System::Drawing::Size(75, 23);
			   this->MergeButton->TabIndex = 1;
			   this->MergeButton->Text = L"Merge";
			   this->MergeButton->UseVisualStyleBackColor = true;
			   this->MergeButton->Click += gcnew System::EventHandler(this, &NonMonotonicInfliciton::onMerge);
			   // 
			   // SplitButton
			   // 
			   this->SplitButton->Location = System::Drawing::Point(130, 140);
			   this->SplitButton->Name = L"SplitButton";
			   this->SplitButton->Size = System::Drawing::Size(75, 23);
			   this->SplitButton->TabIndex = 1;
			   this->SplitButton->Text = L"Split";
			   this->SplitButton->UseVisualStyleBackColor = true;
			   this->SplitButton->Click += gcnew System::EventHandler(this, &NonMonotonicInfliciton::onSplit);
			   //
			   // inputFeild
			   //
			   this->inputFeild->Location = System::Drawing::Point(80, 110);
			   this->inputFeild->Name = L"inputFeild";
			   this->inputFeild->Size = System::Drawing::Size(100, 20);
			   this->inputFeild->TabIndex = 2;
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
			   this->userPrompt->Text = "Please enter the Inflection point.";
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
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(620, 250);
			   this->Controls->Add(this->MergeButton);
			   this->Controls->Add(this->SplitButton);
			   this->Controls->Add(this->inputFeild);
			   this->Controls->Add(this->userPrompt);
			   this->Controls->Add(this->infoBox);
			   this->Name = "InflectionPoint";
			   this->Text = "InflectionPoint";
			   this->ResumeLayout(false);

		   }

#pragma endregion
	private: System::Void onMerge(System::Object^ sender, System::EventArgs^ e)
	{
		merge = true;
		string infl = msclr::interop::marshal_as<std::string>(this->inputFeild->Text);
		this->inflictionPoint = stod(infl);
		this->Close();
	}

	private: System::Void onSplit(System::Object^ sender, System::EventArgs^ e)
	{
		std::string s = "Split";
		string infl = msclr::interop::marshal_as<std::string>(this->inputFeild->Text);
		this->inflictionPoint = stod(infl);
		this->Close();
	}

	public: bool getKindBool()
	{
		return(this->merge);
	}

	public: double getInflictionPoint()
	{
		return(this->inflictionPoint);
	}

	};
}
