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

	public ref class ClassScheme : public System::Windows::Forms::Form
	{
	public: ClassScheme(String^ s, DataGridView^ pd, int nr, int nd)
	{
		numRows = nr;
		numDimensions = nd;
		passedData = pd;
		path = s;
		InitializeComponent();
		loadData();
	}

	protected:~ClassScheme()
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
	private: int numDimensions;
	private: int numRows;
	private: int numUnique = 0;
	private: std::vector<std::string>* keys;

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
			   // saveButton
			   // 
			   this->saveButton->Location = System::Drawing::Point(12, 446);
			   this->saveButton->Name = L"saveButton";
			   this->saveButton->Size = System::Drawing::Size(75, 23);
			   this->saveButton->TabIndex = 1;
			   this->saveButton->Text = L"Save";
			   this->saveButton->UseVisualStyleBackColor = true;
			   this->saveButton->Click += gcnew System::EventHandler(this, &ClassScheme::updateScheme);
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(370, 479);
			   this->Controls->Add(this->saveButton);
			   this->Controls->Add(this->dataGridView);
			   this->Name = L"Class Scheme Loader";
			   this->Text = L"Class Scheme Loader";
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			   this->ResumeLayout(false);

		   }

#pragma endregion

	private: System::Void loadData();
	private: System::Void buildSchemeTable();
	private: System::Void updateScheme(System::Object^ sender, System::EventArgs^ e);
	public: std::unordered_map<std::string, std::string>* getSchemeMap();
	};
}
