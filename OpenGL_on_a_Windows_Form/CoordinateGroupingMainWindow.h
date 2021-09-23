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

	public ref class CoordinateGroupingMainWindow : public System::Windows::Forms::Form
	{
	public: CoordinateGroupingMainWindow(vector<int> *avaDim)
	{
		InitializeComponent();
		avalibleCoordinates = avaDim;
		setUpData();
	}

	protected:~CoordinateGroupingMainWindow()
	{
		if (components)
		{
			delete components;
		}
	}

	private: System::Windows::Forms::DataGridView^ dataGridView;
	private: System::Windows::Forms::Button^ confirmButton;
	private: System::Windows::Forms::Button^ createGroupButton;
	private: System::Windows::Forms::Button^ generalizeButton;
	private: System::ComponentModel::Container^ components;

	private: int numOfDimensions;
	private: vector<vector<int>> *coordianteGroups;
	private: vector<int> *avalibleCoordinates;
	private: vector<string>* groupNames;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			   this->confirmButton = (gcnew System::Windows::Forms::Button());
			   this->generalizeButton = (gcnew System::Windows::Forms::Button());
			   this->createGroupButton = (gcnew System::Windows::Forms::Button());
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
			   this->confirmButton->Click += gcnew System::EventHandler(this, &CoordinateGroupingMainWindow::generateRules);
			   // 
			   // createGroupButton
			   // 
			   this->createGroupButton->Location = System::Drawing::Point(97, 446);
			   this->createGroupButton->Name = L"createGroupButton";
			   this->createGroupButton->Size = System::Drawing::Size(100, 23);
			   this->createGroupButton->TabIndex = 1;
			   this->createGroupButton->Text = L"Create Group";
			   this->createGroupButton->UseVisualStyleBackColor = true;
			   this->createGroupButton->Click += gcnew System::EventHandler(this, &CoordinateGroupingMainWindow::createGroup);
			   // 
			   // generalizeButton
			   // 
			   this->generalizeButton->Location = System::Drawing::Point(206, 446);
			   this->generalizeButton->Name = L"generalizeButton";
			   this->generalizeButton->Size = System::Drawing::Size(75, 23);
			   this->generalizeButton->TabIndex = 1;
			   this->generalizeButton->Text = L"Generalize";
			   this->generalizeButton->UseVisualStyleBackColor = true;
			   this->generalizeButton->Click += gcnew System::EventHandler(this, &CoordinateGroupingMainWindow::generalizeGroups);
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(370, 479);
			   this->Controls->Add(this->generalizeButton);
			   this->Controls->Add(this->createGroupButton);
			   this->Controls->Add(this->confirmButton);
			   this->Controls->Add(this->dataGridView);
			   this->Name = L"Coordinate Grouping";
			   this->Text = L"Coordinate Grouping";
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			   this->ResumeLayout(false);

		   }

#pragma endregion

	private: System::Void generateRules(System::Object^ sender, System::EventArgs^ e);
	private: System::Void createGroup(System::Object^ sender, System::EventArgs^ e);
	private: System::Void generalizeGroups(System::Object^ sender, System::EventArgs^ e);
	public: vector<vector<int>>* getCoordinateGroups();
	private: void setUpData();
	};
}