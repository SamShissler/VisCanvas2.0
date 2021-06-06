#pragma once

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class NominalColorPicker : public System::Windows::Forms::Form
	{
	public: NominalColorPicker()
	{
		InitializeComponent();
	}

	protected:~NominalColorPicker()
	{
		if (components)
		{
			delete components;
		}
	}

	private: System::ComponentModel::Container^ components;
	private: System::Windows::Forms::Button^ redButton;
	private: System::Windows::Forms::Button^ greenButton;
	private: System::Windows::Forms::Button^ blueButton;
	private: System::Windows::Forms::Button^ allButton;
	private: System::Windows::Forms::Button^ grayButton;
	private: int result;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->redButton = (gcnew System::Windows::Forms::Button());
			   this->greenButton = (gcnew System::Windows::Forms::Button());
			   this->blueButton = (gcnew System::Windows::Forms::Button());
			   this->allButton = (gcnew System::Windows::Forms::Button());
			   this->grayButton = (gcnew System::Windows::Forms::Button());
			   this->SuspendLayout();

			   // 
			   // redButton
			   // 
			   this->redButton->Location = System::Drawing::Point(95, 70);
			   this->redButton->Name = L"Red Color Scheme";
			   this->redButton->Size = System::Drawing::Size(120, 23);
			   this->redButton->TabIndex = 1;
			   this->redButton->Text = L"Red Color Scheme";
			   this->redButton->UseVisualStyleBackColor = true;
			   this->redButton->Click += gcnew System::EventHandler(this, &NominalColorPicker::redColor);

			   // 
			   // greenButton
			   // 
			   this->greenButton->Location = System::Drawing::Point(95, 100);
			   this->greenButton->Name = L"Green Color Scheme";
			   this->greenButton->Size = System::Drawing::Size(120, 23);
			   this->greenButton->TabIndex = 1;
			   this->greenButton->Text = L"Green Color Scheme";
			   this->greenButton->UseVisualStyleBackColor = true;
			   this->greenButton->Click += gcnew System::EventHandler(this, &NominalColorPicker::greenColor);

			   // 
			   // blueButton
			   // 
			   this->blueButton->Location = System::Drawing::Point(95, 130);
			   this->blueButton->Name = L"Blue Color Scheme";
			   this->blueButton->Size = System::Drawing::Size(120, 23);
			   this->blueButton->TabIndex = 1;
			   this->blueButton->Text = L"Blue Color Scheme";
			   this->blueButton->UseVisualStyleBackColor = true;
			   this->blueButton->Click += gcnew System::EventHandler(this, &NominalColorPicker::blueColor);

			   // 
			   // allButton
			   // 
			   this->allButton->Location = System::Drawing::Point(95, 160);
			   this->allButton->Name = L"All Colors Scheme";
			   this->allButton->Size = System::Drawing::Size(120, 23);
			   this->allButton->TabIndex = 1;
			   this->allButton->Text = L"All Colors Scheme";
			   this->allButton->UseVisualStyleBackColor = true;
			   this->allButton->Click += gcnew System::EventHandler(this, &NominalColorPicker::allColor);

			   //
			   // grayButton
			   // 
			   this->grayButton->Location = System::Drawing::Point(95, 190);
			   this->grayButton->Name = L"Gray Scale Scheme";
			   this->grayButton->Size = System::Drawing::Size(120, 23);
			   this->grayButton->TabIndex = 1;
			   this->grayButton->Text = L"Gray Scale Scheme";
			   this->grayButton->UseVisualStyleBackColor = true;
			   this->grayButton->Click += gcnew System::EventHandler(this, &NominalColorPicker::grayColor);

			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(300, 300);
			   this->Controls->Add(this->redButton);
			   this->Controls->Add(this->greenButton);
			   this->Controls->Add(this->blueButton);
			   this->Controls->Add(this->allButton);
			   this->Controls->Add(this->grayButton);
			   this->Name = L"Nominal Sets Color Picker";
			   this->Text = L"Nominal Sets Color Picker";
			   this->ResumeLayout(false);

		   }

#pragma endregion

	private: System::Void redColor(System::Object^ sender, System::EventArgs^ e);
	private: System::Void greenColor(System::Object^ sender, System::EventArgs^ e);
	private: System::Void blueColor(System::Object^ sender, System::EventArgs^ e);
	private: System::Void allColor(System::Object^ sender, System::EventArgs^ e);
	private: System::Void grayColor(System::Object^ sender, System::EventArgs^ e);
	public: int getResult();
	};
}
