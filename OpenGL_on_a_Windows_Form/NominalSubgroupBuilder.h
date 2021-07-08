#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <stack>
#include "NSG.h"
#include "NSGGroupAssingment.h"
#include "UserInputPopUp.h"
#include <msclr\marshal_cppstd.h>

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class NominalSubGroupBuilder : public System::Windows::Forms::Form
	{
	public: NominalSubGroupBuilder(NSG* g)
	{
		group = g;
		prevS = new stack<NSG*>();
		InitializeComponent();
		buildTable();
	}

	protected:~NominalSubGroupBuilder()
	{
		if (components)
		{
			delete components;
		}
	}

	private:System::String^ path;
	private: System::Windows::Forms::DataGridView^ dataGridView;
	private: System::Windows::Forms::Button^ confirmButton;
	private: System::Windows::Forms::Button^ addSubButton;
	private: System::Windows::Forms::Button^ selectGroupButton;
	private: System::Windows::Forms::Button^ backButton;
	private: System::Windows::Forms::Button^ viewButton;
	private: System::Windows::Forms::RichTextBox^ subCatBox;
	private: System::ComponentModel::Container^ components;
	private: NSG* group;
	private: NSG* prevGroup;
	private: stack<NSG*>* prevS;
	private: vector<string>* finalScheme;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			   this->viewButton = (gcnew System::Windows::Forms::Button());
			   this->addSubButton = (gcnew System::Windows::Forms::Button());
			   this->selectGroupButton = (gcnew System::Windows::Forms::Button());
			   this->backButton = (gcnew System::Windows::Forms::Button());
			   this->subCatBox = (gcnew System::Windows::Forms::RichTextBox());
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
			   // addSubgroupButton
			   // 
			   this->addSubButton->Location = System::Drawing::Point(70, 446);
			   this->addSubButton->Name = L"Create Category";
			   this->addSubButton->Size = System::Drawing::Size(110, 23);
			   this->addSubButton->TabIndex = 1;
			   this->addSubButton->Text = L"Create Category";
			   this->addSubButton->UseVisualStyleBackColor = true;
			   this->addSubButton->Click += gcnew System::EventHandler(this, &NominalSubGroupBuilder::addSubGroup);
			   // 
			   // selectGroupButton
			   // 
			   this->selectGroupButton->Location = System::Drawing::Point(180, 446);
			   this->selectGroupButton->Name = L"Select Category";
			   this->selectGroupButton->Size = System::Drawing::Size(110, 23);
			   this->selectGroupButton->TabIndex = 1;
			   this->selectGroupButton->Text = L"Select Category";
			   this->selectGroupButton->UseVisualStyleBackColor = true;
			   this->selectGroupButton->Click += gcnew System::EventHandler(this, &NominalSubGroupBuilder::selectGroup);
			   // 
			   // backButton
			   // 
			   this->backButton->Location = System::Drawing::Point(10, 446);
			   this->backButton->Name = L"Back";
			   this->backButton->Size = System::Drawing::Size(60, 23);
			   this->backButton->TabIndex = 1;
			   this->backButton->Text = L"Back";
			   this->backButton->UseVisualStyleBackColor = true;
			   this->backButton->Click += gcnew System::EventHandler(this, &NominalSubGroupBuilder::back);
			   // 
			   // viewButton
			   // 
			   this->viewButton->Location = System::Drawing::Point(290, 446);
			   this->viewButton->Name = L"View";
			   this->viewButton->Size = System::Drawing::Size(70, 23);
			   this->viewButton->TabIndex = 1;
			   this->viewButton->Text = L"View";
			   this->viewButton->UseVisualStyleBackColor = true;
			   this->viewButton->Click += gcnew System::EventHandler(this, &NominalSubGroupBuilder::view);
			   //
			   // subCatBox
			   //
			   this->subCatBox->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 13, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->subCatBox->Location = System::Drawing::Point(370, 12);
			   this->subCatBox->Name = L"subCatBox";
			   this->subCatBox->Size = System::Drawing::Size(192, 455);
			   this->subCatBox->TabIndex = 0;
			   this->subCatBox->Text = L"";
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(570, 479);
			   this->Controls->Add(this->dataGridView);
			   this->Controls->Add(this->addSubButton);
			   this->Controls->Add(this->selectGroupButton);
			   this->Controls->Add(this->backButton);
			   this->Controls->Add(this->viewButton);
			   this->Controls->Add(this->subCatBox);
			   this->Name = gcnew String(group->groupName.c_str());
			   this->Text = gcnew String(group->groupName.c_str()) + " (Category)";
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			   this->ResumeLayout(false);

		   }

#pragma endregion

	private: System::Void buildTable();
	private: System::Void addSubGroup(System::Object^ sender, System::EventArgs^ e);
	private: System::Void selectGroup(System::Object^ sender, System::EventArgs^ e);
	private: System::Void view(System::Object^ sender, System::EventArgs^ e);
	private: System::Void back(System::Object^ sender, System::EventArgs^ e);
	public: std::vector<std::string> getSchemeMap();

	};

}