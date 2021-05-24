#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "OrdinalGrouping.h"

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class Scheme : public System::Windows::Forms::Form
	{
	public: Scheme(std::unordered_map<std::string, std::string>* sm, std::vector<std::string>* k, int nu, int nr, int nd, int av)
		{
			numRows = nr;
			numDimensions = nd;
			numUnique = nu;
			schemeMap = sm;
			keys = k;
			attributeValue = av;
			InitializeComponent();
			buildSchemeTable();
		}

	protected:~Scheme()
		{
			if (components)
			{
				delete components;
			}
		}


	private:System::String^ path;
	private: System::Windows::Forms::DataGridView^ dataGridView;
	private: System::Windows::Forms::Button^ saveButton;
	private: System::Windows::Forms::Button^ genValsButton;
	private: System::Windows::Forms::Button^ keepValsButton;
	private: System::ComponentModel::Container^ components;
	private: std::unordered_map<std::string, std::string>* schemeMap;
	private: System::Windows::Forms::DataGridView^ passedData;
	private: int numDimensions;
	private: int numRows;
	private: int numUnique;
	private: int attributeValue;
	private: std::vector<std::string>* keys;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			   this->saveButton = (gcnew System::Windows::Forms::Button());
			   this->genValsButton = (gcnew System::Windows::Forms::Button());
			   this->keepValsButton = (gcnew System::Windows::Forms::Button());
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
			   // saveButton
			   // 
			   this->saveButton->Location = System::Drawing::Point(12, 446);
			   this->saveButton->Name = L"saveButton";
			   this->saveButton->Size = System::Drawing::Size(75, 23);
			   this->saveButton->TabIndex = 1;
			   this->saveButton->Text = L"Save";
			   this->saveButton->UseVisualStyleBackColor = true;
			   this->saveButton->Click += gcnew System::EventHandler(this, &Scheme::updateScheme);
			   // 
			   // useSuggestedValuesButton
			   // 
			   this->genValsButton->Location = System::Drawing::Point(90, 446);
			   this->genValsButton->Name = L"useSuggestedValuesButton";
			   this->genValsButton->Size = System::Drawing::Size(130, 23);
			   this->genValsButton->TabIndex = 1;
			   this->genValsButton->Text = L"Generate Values";
			   this->genValsButton->UseVisualStyleBackColor = true;
			   this->genValsButton->Click += gcnew System::EventHandler(this, &Scheme::orderWindowLaunch);
			   //
			   //KeepValuesButton
			   //
			   this->keepValsButton->Location = System::Drawing::Point(222, 446);
			   this->keepValsButton->Name = L"keepValuesButton";
			   this->keepValsButton->Size = System::Drawing::Size(130, 23);
			   this->keepValsButton->TabIndex = 1;
			   this->keepValsButton->Text = L"Keep Values";
			   this->keepValsButton->UseVisualStyleBackColor = true;
			   this->keepValsButton->Click += gcnew System::EventHandler(this, &Scheme::keepValues);
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(370, 479);
			   this->Controls->Add(this->saveButton);
			   this->Controls->Add(this->genValsButton);
			   this->Controls->Add(this->keepValsButton);
			   this->Controls->Add(this->dataGridView);
			   this->Name = L"Scheme Loader (Attribute: X" + attributeValue + ")";
			   this->Text = L"Scheme Loader (Attribute: X" + attributeValue + ")";
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			   this->ResumeLayout(false);

		   }

#pragma endregion

	private: System::Void buildSchemeTable();
	private: System::Void updateScheme(System::Object^ sender, System::EventArgs^ e);
	private: System::Void orderWindowLaunch(System::Object^ sender, System::EventArgs^ e);
	private: System::Void updateTable();
	private: System::Void keepValues(System::Object^ sender, System::EventArgs^ e);
	public: std::unordered_map<std::string, std::string>* getSchemeMap();
	};
}
