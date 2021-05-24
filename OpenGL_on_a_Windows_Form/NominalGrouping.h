///
// Name: Eli McCoy
// Date: 3/24/21
// File: Scheme.h
// Desc: Header file for scheme kvpair input window.
///

#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class NominalGrouping: public System::Windows::Forms::Form
	{
	public: NominalGrouping(DataGridView^ pd, std::vector<std::string>* idv)
	{
		passedData = pd;
		ids = idv;
		InitializeComponent();
		buildSchemeTable();
	}

	protected:~NominalGrouping()
	{
		if (components)
		{
			delete components;
		}
	}


	private:System::String^ path;
	private: System::Windows::Forms::DataGridView^ dataGridView;
	private: System::Windows::Forms::Button^ confirmButton;
	private: System::Windows::Forms::Button^ genValsButton;
	private: System::ComponentModel::Container^ components;
	private: std::unordered_map<std::string, std::string>* schemeMap;
	private: System::Windows::Forms::DataGridView^ passedData;
	private: std::vector<std::string>* keys;
	private: int numDimensions;
	private: int numRows;
	private: int numUnique = 0;
	private: std::vector<std::string>* ids;
	private: std::vector<std::string>* toAdd;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			   this->confirmButton = (gcnew System::Windows::Forms::Button());
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->BeginInit();
			   this->SuspendLayout();
			   // 
			   // dataGridView
			   // 
			   this->dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			   this->dataGridView->Location = System::Drawing::Point(12, 12);
			   this->dataGridView->Name = L"dataGridView";
			   this->dataGridView->Size = System::Drawing::Size(346, 428);
			   this->dataGridView->TabIndex = 0;
			   this->dataGridView->AllowUserToAddRows = false;
			   // 
			   // confirmButton
			   // 
			   this->confirmButton->Location = System::Drawing::Point(12, 446);
			   this->confirmButton->Name = L"confirmButton";
			   this->confirmButton->Size = System::Drawing::Size(75, 23);
			   this->confirmButton->TabIndex = 1;
			   this->confirmButton->Text = L"Confirm";
			   this->confirmButton->UseVisualStyleBackColor = true;
			   this->confirmButton->Click += gcnew System::EventHandler(this, &NominalGrouping::updateScheme);
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(370, 479);
			   this->Controls->Add(this->confirmButton);
			   this->Controls->Add(this->genValsButton);
			   this->Controls->Add(this->dataGridView);
			   this->Name = L"Nominal Grouping";
			   this->Text = L"Nominal Grouping";
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			   this->ResumeLayout(false);

		   }

#pragma endregion

	private: System::Void buildSchemeTable();
	private: System::Void updateScheme(System::Object^ sender, System::EventArgs^ e);
	public: std::vector<std::string>* getNominalVector();
	public: std::string toBinary(int n);
	};
}
