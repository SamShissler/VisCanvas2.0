#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include "SetCluster.h"
#include "OpenGL.h"

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace std;

	public ref class EmptySpotHBPicker : public System::Windows::Forms::Form
	{
	public: EmptySpotHBPicker(/*System::Object^ sender,*/ vector<SetCluster> hbs, vector<vector<double>> es, string* eshbs)
	{
		blocks = new vector<SetCluster>();
		for (int i = 0; i < hbs.size(); i++) {
			blocks->push_back(hbs[i]);
		}
		emptys = new vector<vector<double>>();
		emptyhbs = new vector<int>();
		vector<double>* temp = new vector<double>();
		for (int i = 0; i < es.size(); i++) {
			temp->clear();
			for (int j = 0; j < es[0].size(); j++) {
				temp->push_back(es[i][j]);
			}
			emptys->push_back(*temp);
			emptyhbs->push_back(-1);
		}
		esHBs = eshbs;
		attributeAddOffset = 0;
		InitializeComponent();
	}

	protected: ~EmptySpotHBPicker()
	{
		if (components)
		{
			delete components;
		}
	}

	private: System::Windows::Forms::TableLayoutPanel^ tableLayoutPanel1;
	private: System::Windows::Forms::ToolStripContainer^ toolStripContainer1;
	private: System::Windows::Forms::ToolStripContainer^ toolStripContainer2;
	private: System::Windows::Forms::DataGridView^ dataGridView;
	private: System::Windows::Forms::ToolStrip^ toolStrip1;
	private: System::Windows::Forms::ToolStripDropDownButton^ FileDropDown;
	private: System::Windows::Forms::ToolStripMenuItem^ openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ saveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ loadSchemeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ editClassValuesToolStripMenuItem;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Button^ visualizeButton;
	private: System::Windows::Forms::Button^ submitButton;
	private: System::Windows::Forms::CheckBox^ useRecommendedCheckBox;
	private: System::Windows::Forms::TextBox^ columnInputFeild;
	private: System::ComponentModel::Container^ components;
	private: std::string** sortingTable;
	private: std::unordered_map<std::string, std::string>* schemeMap;
	private: std::unordered_map<std::string, std::string>* classMap;
	private: int numDimensions;
	private: int numRows;
	private: int attributeAddOffset;
		   //std::vector<SetCluster> blocks;
	private: vector<SetCluster>* blocks;
		   // Need to pass list of empty spots into this class as well
		   //private: std::string* emptys;
	private: vector<vector<double>>* emptys;
		   //List of the hyper blocks that correspond with each empty spot
	private: vector<int>* emptyhbs;
		   //private: vector<vector<int>>* esHBs;
	private: string* esHBs;
		   //Need to pass hyperblocks into this class in a managed class?
		   //Get rid of file selection and ordering buttons
		   //Display hyperblocks
	private: String^ path;

#pragma region 
		   void InitializeComponent(void)
		   {
			   this->toolStripContainer1 = (gcnew System::Windows::Forms::ToolStripContainer());
			   this->toolStripContainer2 = (gcnew System::Windows::Forms::ToolStripContainer());
			   this->visualizeButton = (gcnew System::Windows::Forms::Button());
			   this->submitButton = (gcnew System::Windows::Forms::Button());
			   this->useRecommendedCheckBox = (gcnew System::Windows::Forms::CheckBox());
			   //this->columnInputFeild = (gcnew System::Windows::Forms::TextBox());
			   this->label1 = (gcnew System::Windows::Forms::Label());
			   this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			   //this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			   this->toolStripContainer1->ContentPanel->SuspendLayout();
			   //this->toolStripContainer1->TopToolStripPanel->SuspendLayout();
			   this->toolStripContainer1->SuspendLayout();
			   this->toolStripContainer2->ContentPanel->SuspendLayout();
			   this->toolStripContainer2->SuspendLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->BeginInit();
			   //this->toolStrip1->SuspendLayout();
			   this->SuspendLayout();
			   // 
			   // toolStripContainer1.ContentPanel
			   // 
			   this->toolStripContainer1->Dock = DockStyle::Fill;
			   //this->toolStripContainer1->ContentPanel->Controls->Add(this->columnInputFeild);
			   //this->toolStripContainer1->ContentPanel->Controls->Add(this->label1);
			   this->toolStripContainer1->ContentPanel->Controls->Add(this->dataGridView);
			   this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(1065, 640);
			   this->toolStripContainer1->Location = System::Drawing::Point(-3, 0);
			   this->toolStripContainer1->Name = L"toolStripContainer1";
			   this->toolStripContainer1->Size = System::Drawing::Size(1065, 665);
			   this->toolStripContainer1->TabIndex = 0;
			   this->toolStripContainer1->Text = L"toolStripContainer1";
			   //
			   // toolStripContainer2.ContentPanel
			   //
			   this->toolStripContainer2->Dock = DockStyle::Bottom;
			   this->toolStripContainer2->ContentPanel->Controls->Add(this->visualizeButton);
			   this->toolStripContainer2->ContentPanel->Controls->Add(this->submitButton);
			   this->toolStripContainer2->ContentPanel->Controls->Add(this->useRecommendedCheckBox);
			   this->toolStripContainer2->ContentPanel->Controls->Add(this->label1);
			   this->toolStripContainer2->Location = System::Drawing::Point(0, 0);
			   this->toolStripContainer2->Size = System::Drawing::Size(1065, 33);
			   this->toolStripContainer2->Name = L"toolStripContainer2";
			   //this->toolStripContainer2->TabIndex = 1;
			   this->toolStripContainer2->Text = L"toolStripContainer2";

			   // 
			   // toolStripContainer1.TopToolStripPanel
			   // 
			   //this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->toolStrip1);
			   // 
			   // visualizeButton
			   // 
			   this->visualizeButton->Anchor = AnchorStyles::Top | AnchorStyles::Right;
			   this->visualizeButton->Location = System::Drawing::Point(-20,5);
			   this->visualizeButton->Name = L"visualizeButton";
			   this->visualizeButton->Size = System::Drawing::Size(75, 23);
			   this->visualizeButton->TabIndex = 2;
			   this->visualizeButton->Text = L"Visualize";
			   this->visualizeButton->UseVisualStyleBackColor = true;
			   this->visualizeButton->Click += gcnew System::EventHandler(this, &EmptySpotHBPicker::visualizeButton_Click);
			   // 
			   // submitButton
			   // 
			   this->submitButton->Anchor = AnchorStyles::Top | AnchorStyles::Right;
			   this->submitButton->Location = System::Drawing::Point(60, 5);
			   this->submitButton->Name = L"submitButton";
			   this->submitButton->Size = System::Drawing::Size(75, 23);
			   this->submitButton->TabIndex = 3;
			   this->submitButton->Text = L"Submit";
			   this->submitButton->UseVisualStyleBackColor = true;
			   this->submitButton->Click += gcnew System::EventHandler(this, &EmptySpotHBPicker::submitButton_Click);
			   //
			   // useRecommended
			   //
			   //this->useRecommendedCheckBox.Appearance = Appearance.Button;
			   //this->useRecommendedCheckBox.AutoCheck = true;
			   this->useRecommendedCheckBox->Anchor = AnchorStyles::Top | AnchorStyles::Right;
			   this->useRecommendedCheckBox->Location = System::Drawing::Point(-40, 5);
			   this->useRecommendedCheckBox->Checked = true;
			   this->useRecommendedCheckBox->Size = System::Drawing::Size(75, 23);
			   this->useRecommendedCheckBox->TabIndex = 1;
			   this->useRecommendedCheckBox->Name = L"useRecommendedCheckBox";
			   this->useRecommendedCheckBox->Text = L"Use Recommended";
			   // 
			   // label1
			   // 
			   this->label1->Anchor = AnchorStyles::Top | AnchorStyles::Right;
			   this->label1->AutoSize = true;
			   this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->label1->Location = System::Drawing::Point(-160, 7);
			   this->label1->Name = L"label1";
			   this->label1->Size = System::Drawing::Size(154, 20);
			   this->label1->TabIndex = 1;
			   this->label1->Text = L"Use Recommended"; //  0 – value for attr.in ES case does not belong to HB, 1 - belongs to HB, ? -belonging is unknown because of lack of data)";
			   // 
			   // dataGridView
			   // 
			   this->dataGridView->Dock = DockStyle::Fill;
			   //this->dataGridView->Anchor = AnchorStyles::Bottom;
			   this->dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			   //this->dataGridView->Location = System::Drawing::Point(5, 5);
			   this->dataGridView->Name = L"dataGridView";
			   //this->dataGridView->Size = System::Drawing::Size(1036, 640);
			   this->dataGridView->TabIndex = 0;
			   this->dataGridView->SelectionMode = DataGridViewSelectionMode::FullRowSelect;
			   drawTables();
			   // 
			   // EmptySpotHBPicker
			   // 
			   this->ClientSize = System::Drawing::Size(1060, 665);
			   this->Controls->Add(this->toolStripContainer1);
			   this->Controls->Add(this->toolStripContainer2);
			   this->Location = System::Drawing::Point(1000, 1000);
			   this->Name = L"EmptySpotHBPicker";
			   this->Text = L"Empty Spot (ES) - Hyper Block Compatability Selector";
			   this->toolStripContainer1->ContentPanel->ResumeLayout(false);
			   this->toolStripContainer1->ContentPanel->PerformLayout();
			   //this->toolStripContainer1->TopToolStripPanel->ResumeLayout(false);
			   //this->toolStripContainer1->TopToolStripPanel->PerformLayout();
			   this->toolStripContainer1->ResumeLayout(false);
			   this->toolStripContainer1->PerformLayout();
			   this->toolStripContainer2->ContentPanel->ResumeLayout(false);
			   this->toolStripContainer2->ContentPanel->PerformLayout();
			   //this->toolStripContainer2->TopToolStripPanel->ResumeLayout(false);
			   //this->toolStripContainer2->TopToolStripPanel->PerformLayout();
			   this->toolStripContainer2->ResumeLayout(false);
			   this->toolStripContainer2->PerformLayout();
			   (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			   //this->toolStrip1->ResumeLayout(false);
			   //this->toolStrip1->PerformLayout();
			   this->ResumeLayout(false);

		   }
#pragma endregion

	private:
		System::Void drawTables();
	private: System::Void submitButton_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void submit(System::String^ column);
	private: System::Void visualize(System::String^ column);
	private: System::Void visualizeButton_Click(System::Object^ sender, System::EventArgs^ e);
	};
}
