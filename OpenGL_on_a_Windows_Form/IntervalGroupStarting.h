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

	public ref class IntervalGroupStarting : public System::Windows::Forms::Form
	{
	public: IntervalGroupStarting()
	{
		numGroups = 1;
		InitializeComponent();
		buildTable();
	}

	protected:~IntervalGroupStarting()
	{
		if (components)
		{
			delete components;
		}
	}


	private:System::String^ path;
	private: System::Windows::Forms::DataGridView^ dataGridView;
	private: System::Windows::Forms::Button^ saveButton;
	private: System::ComponentModel::Container^ components;
	private: std::unordered_map<std::string, std::string>* schemeMap;
	private: System::Windows::Forms::DataGridView^ passedData;
	private: int numGroups;
	private: std::vector<std::string>* startingPoints;
	private: std::vector<std::string>* IntervalToFirst;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			   this->saveButton = (gcnew System::Windows::Forms::Button());
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
			   this->saveButton->Location = System::Drawing::Point(12, 446);
			   this->saveButton->Name = L"saveButton";
			   this->saveButton->Size = System::Drawing::Size(75, 23);
			   this->saveButton->TabIndex = 1;
			   this->saveButton->Text = L"Save";
			   this->saveButton->UseVisualStyleBackColor = true;
			   this->saveButton->Click += gcnew System::EventHandler(this, &IntervalGroupStarting::handleClose);
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(370, 479);
			   this->Controls->Add(this->saveButton);
			   this->Controls->Add(this->dataGridView);
			   this->Name = L"Interval Group Starting";
			   this->Text = L"Interval Group Starting";
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			   this->ResumeLayout(false);

		   }

#pragma endregion

	private: System::Void buildTable();
	private: System::Void handleClose(System::Object^ sender, System::EventArgs^ e);
	public: std::vector<std::string>* getIntervalToFirst();
	public: std::vector<std::string>* getStartingPoints();
	};
}
