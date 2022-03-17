#pragma once

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class NormalizationStyle : public System::Windows::Forms::Form
	{
	public: NormalizationStyle()
	{
		InitializeComponent();
	}

	protected:~NormalizationStyle()
	{
		if (components)
		{
			delete components;
		}
	}

	private: System::ComponentModel::Container^ components;
	private: System::Windows::Forms::Button^ MinMaxButton;
	private: System::Windows::Forms::Button^ zScoreButton;
	private: int style;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->MinMaxButton = (gcnew System::Windows::Forms::Button());
			   this->zScoreButton = (gcnew System::Windows::Forms::Button());
			   this->SuspendLayout();

			   // 
			   // MinMax Button
			   // 
			   this->MinMaxButton->Location = System::Drawing::Point(55, 70);
			   this->MinMaxButton->Name = L"Min-Max Normalization";
			   this->MinMaxButton->Size = System::Drawing::Size(200, 40);
			   this->MinMaxButton->TabIndex = 1;
			   this->MinMaxButton->Text = L"Min-Max Normalization";
			   this->MinMaxButton->UseVisualStyleBackColor = true;
			   this->MinMaxButton->Click += gcnew System::EventHandler(this, &NormalizationStyle::MinMax);

			   // 
			   // z-Score Button
			   // 
			   this->zScoreButton->Location = System::Drawing::Point(55, 110);
			   this->zScoreButton->Name = L"z-Score Min-Max Normalization";
			   this->zScoreButton->Size = System::Drawing::Size(200, 40);
			   this->zScoreButton->TabIndex = 1;
			   this->zScoreButton->Text = L"z-Score Min-Max Normalization";
			   this->zScoreButton->UseVisualStyleBackColor = true;
			   this->zScoreButton->Click += gcnew System::EventHandler(this, &NormalizationStyle::zScore);

			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(300, 250);
			   this->Controls->Add(this->MinMaxButton);
			   this->Controls->Add(this->zScoreButton);
			   this->Name = L"Select Normalization Style";
			   this->Text = L"Select Normalization Style";
			   this->ResumeLayout(false);
			   this->MaximizeBox = false;
			   this->MinimizeBox = false;
			   this->CenterToScreen();
		   }

#pragma endregion

	private: System::Void MinMax(System::Object^ sender, System::EventArgs^ e);
	private: System::Void zScore(System::Object^ sender, System::EventArgs^ e);
	public: int getStyle();
	};
}
