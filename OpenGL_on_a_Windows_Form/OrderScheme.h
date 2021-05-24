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

	public ref class OrderScheme : public System::Windows::Forms::Form
	{
	public: OrderScheme(DataGridView^ pd, int nr, int nd, int av)
	{
		numRows = nr;
		numDimensions = nd;
		passedData = pd;
		attributeValue = av;
		InitializeComponent();
		loadData();
	}

	protected:~OrderScheme()
	{
		if (components)
		{
			delete components;
		}
	}

	private:System::String^ path;
	private: System::Windows::Forms::DataGridView^ dataGridView;
	private: System::Windows::Forms::Button^ continueButton;
	private: System::Windows::Forms::Button^ allOrdinalButton;
	private: System::Windows::Forms::Button^ allNominalButton;
	private: System::ComponentModel::Container^ components;
	private: std::unordered_map<std::string, std::string>* schemeMap;
	private: System::Windows::Forms::DataGridView^ passedData;
	private: System::Windows::Forms::RichTextBox^ infoBox;
	private: int numDimensions;
	private: int numRows;
	private: int numUnique = 0;
	private: int lenNominal = 0;
	private: int attributeValue;
	private: std::vector<std::string>* keys;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			   this->continueButton = (gcnew System::Windows::Forms::Button());
			   this->allOrdinalButton = (gcnew System::Windows::Forms::Button());
			   this->allNominalButton = (gcnew System::Windows::Forms::Button());
			   this->infoBox = (gcnew System::Windows::Forms::RichTextBox());
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
			   // infoBox
			   // 
			   this->infoBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->infoBox->Location = System::Drawing::Point(362, 12);
			   this->infoBox->Name = L"infoBox";
			   this->infoBox->Size = System::Drawing::Size(501, 428);
			   this->infoBox->TabIndex = 0;
			   this->infoBox->Text = L"";
			   // 
			   // continueButton
			   // 
			   this->continueButton->Location = System::Drawing::Point(12, 446);
			   this->continueButton->Name = L"continueButton";
			   this->continueButton->Size = System::Drawing::Size(75, 23);
			   this->continueButton->TabIndex = 1;
			   this->continueButton->Text = L"Continue";
			   this->continueButton->UseVisualStyleBackColor = true;
			   this->continueButton->Click += gcnew System::EventHandler(this, &OrderScheme::generateValues);
			   // 
			   // allOrdinal
			   // 
			   this->allOrdinalButton->Location = System::Drawing::Point(112, 446);
			   this->allOrdinalButton->Name = L"allOrdinal";
			   this->allOrdinalButton->Size = System::Drawing::Size(75, 23);
			   this->allOrdinalButton->TabIndex = 1;
			   this->allOrdinalButton->Text = L"All Ordinal";
			   this->allOrdinalButton->UseVisualStyleBackColor = true;
			   this->allOrdinalButton->Click += gcnew System::EventHandler(this, &OrderScheme::allOrdinalVals);
			   // 
			   // allNominal
			   // 
			   this->allNominalButton->Location = System::Drawing::Point(212, 446);
			   this->allNominalButton->Name = L"allNominal";
			   this->allNominalButton->Size = System::Drawing::Size(75, 23);
			   this->allNominalButton->TabIndex = 1;
			   this->allNominalButton->Text = L"All Nominal";
			   this->allNominalButton->UseVisualStyleBackColor = true;
			   this->allNominalButton->Click += gcnew System::EventHandler(this, &OrderScheme::allNominalVals);
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(870, 479);
			   this->Controls->Add(this->continueButton);
			   
			   //Commented out because the change to indavidual scheme for columns.
			   
			   //this->Controls->Add(this->allOrdinalButton);
			   //this->Controls->Add(this->allNominalButton);
			   this->Controls->Add(this->dataGridView);
			   this->Controls->Add(this->infoBox);
			   this->Name = L"Scheme Loader";
			   this->Text = L"Scheme Loader";
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			   this->ResumeLayout(false);

		   }

#pragma endregion

	private: System::Void loadData();
	private: System::Void buildSchemeTable();
	private: System::Void generateValues(System::Object^ sender, System::EventArgs^ e);
	private: System::Void allNominalVals(System::Object^ sender, System::EventArgs^ e);
	private: System::Void allOrdinalVals(System::Object^ sender, System::EventArgs^ e);
	private: System::Void handleNominalBinary(std::vector<std::string> nominalVector);
	private: System::Void handleNominalHigher(std::vector<std::string> nominalVector);
	private: System::Void handleOrdinal(std::vector<std::string> ordinalVector);
	private: System::Void handleInterval(std::vector<std::string> IntervalVector);\
	private: System::Void handleRatio(std::vector<std::string> ratioVector);
	private: std::string toBinary(int n);
	public: std::unordered_map<std::string, std::string>* getSchemeMap();
	public: int getLenNominal();
	};
}
