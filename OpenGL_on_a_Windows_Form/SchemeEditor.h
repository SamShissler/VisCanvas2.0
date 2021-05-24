#pragma once
#include <string>
#include <unordered_map>

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class SchemeEditor : public System::Windows::Forms::Form
	{
	public: SchemeEditor(void)
		{
			attributeAddOffset = 0;
			InitializeComponent();
		}

	protected: ~SchemeEditor()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TableLayoutPanel^ tableLayoutPanel1;
	private: System::Windows::Forms::ToolStripContainer^ toolStripContainer1;
	private: System::Windows::Forms::DataGridView^ dataGridView;
	private: System::Windows::Forms::ToolStrip^ toolStrip1;
	private: System::Windows::Forms::ToolStripDropDownButton^ FileDropDown;
	private: System::Windows::Forms::ToolStripMenuItem^ openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ saveToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ loadSchemeToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^ editClassValuesToolStripMenuItem;
	private: System::Windows::Forms::Label^ label1;
	private: System::Windows::Forms::Button^ descendingOrderButton;
	private: System::Windows::Forms::Button^ ascendingOrderButton;
	private: System::Windows::Forms::TextBox^ columnInputFeild;
	private: System::ComponentModel::Container^ components;
	private: std::string** sortingTable;
	private: std::unordered_map<std::string, std::string>* schemeMap;
	private: std::unordered_map<std::string, std::string>* classMap;
	private: int numDimensions;
	private: int numRows;
	private: int attributeAddOffset;
	private: System::String^ path;

#pragma region 
		void InitializeComponent(void)
		{
			this->toolStripContainer1 = (gcnew System::Windows::Forms::ToolStripContainer());
			this->descendingOrderButton = (gcnew System::Windows::Forms::Button());
			this->ascendingOrderButton = (gcnew System::Windows::Forms::Button());
			this->columnInputFeild = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->dataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->toolStrip1 = (gcnew System::Windows::Forms::ToolStrip());
			this->FileDropDown = (gcnew System::Windows::Forms::ToolStripDropDownButton());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->saveToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadSchemeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->editClassValuesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripContainer1->ContentPanel->SuspendLayout();
			this->toolStripContainer1->TopToolStripPanel->SuspendLayout();
			this->toolStripContainer1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->BeginInit();
			this->toolStrip1->SuspendLayout();
			this->SuspendLayout();
			// 
			// toolStripContainer1
			// 
			// 
			// toolStripContainer1.ContentPanel
			// 
			this->toolStripContainer1->ContentPanel->Controls->Add(this->descendingOrderButton);
			this->toolStripContainer1->ContentPanel->Controls->Add(this->ascendingOrderButton);
			this->toolStripContainer1->ContentPanel->Controls->Add(this->columnInputFeild);
			this->toolStripContainer1->ContentPanel->Controls->Add(this->label1);
			this->toolStripContainer1->ContentPanel->Controls->Add(this->dataGridView);
			this->toolStripContainer1->ContentPanel->Size = System::Drawing::Size(1065, 640);
			this->toolStripContainer1->Location = System::Drawing::Point(-3, 0);
			this->toolStripContainer1->Name = L"toolStripContainer1";
			this->toolStripContainer1->Size = System::Drawing::Size(1065, 665);
			this->toolStripContainer1->TabIndex = 0;
			this->toolStripContainer1->Text = L"toolStripContainer1";
			// 
			// toolStripContainer1.TopToolStripPanel
			// 
			this->toolStripContainer1->TopToolStripPanel->Controls->Add(this->toolStrip1);
			// 
			// descendingOrderButton
			// 
			this->descendingOrderButton->Location = System::Drawing::Point(363, 613);
			this->descendingOrderButton->Name = L"descendingOrderButton";
			this->descendingOrderButton->Size = System::Drawing::Size(75, 23);
			this->descendingOrderButton->TabIndex = 4;
			this->descendingOrderButton->Text = L"Descending";
			this->descendingOrderButton->UseVisualStyleBackColor = true;
			this->descendingOrderButton->Click += gcnew System::EventHandler(this, &SchemeEditor::descendingOrderButton_Click);
			// 
			// ascendingOrderButton
			// 
			this->ascendingOrderButton->Location = System::Drawing::Point(282, 613);
			this->ascendingOrderButton->Name = L"ascendingOrderButton";
			this->ascendingOrderButton->Size = System::Drawing::Size(75, 23);
			this->ascendingOrderButton->TabIndex = 3;
			this->ascendingOrderButton->Text = L"Ascending";
			this->ascendingOrderButton->UseVisualStyleBackColor = true;
			this->ascendingOrderButton->Click += gcnew System::EventHandler(this, &SchemeEditor::ascendingOrderButton_Click);
			// 
			// columnInputFeild
			// 
			this->columnInputFeild->Location = System::Drawing::Point(176, 615);
			this->columnInputFeild->Name = L"columnInputFeild";
			this->columnInputFeild->Size = System::Drawing::Size(100, 20);
			this->columnInputFeild->TabIndex = 2;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(16, 613);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(154, 20);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Column to sort by:";
			// 
			// dataGridView
			// 
			this->dataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView->Location = System::Drawing::Point(15, 3);
			this->dataGridView->Name = L"dataGridView";
			this->dataGridView->Size = System::Drawing::Size(1036, 604);
			this->dataGridView->TabIndex = 0;
			// 
			// toolStrip1
			// 
			this->toolStrip1->Dock = System::Windows::Forms::DockStyle::None;
			this->toolStrip1->GripStyle = System::Windows::Forms::ToolStripGripStyle::Hidden;
			this->toolStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) { this->FileDropDown });
			this->toolStrip1->Location = System::Drawing::Point(3, 0);
			this->toolStrip1->Name = L"toolStrip1";
			this->toolStrip1->RenderMode = System::Windows::Forms::ToolStripRenderMode::Professional;
			this->toolStrip1->Size = System::Drawing::Size(41, 25);
			this->toolStrip1->TabIndex = 0;
			// 
			// FileDropDown
			// 
			this->FileDropDown->DisplayStyle = System::Windows::Forms::ToolStripItemDisplayStyle::Text;
			this->FileDropDown->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {
				this->openToolStripMenuItem, this->saveToolStripMenuItem, this->loadSchemeToolStripMenuItem, this->editClassValuesToolStripMenuItem
			});
			this->FileDropDown->ImageTransparentColor = System::Drawing::Color::Magenta;
			this->FileDropDown->Name = L"FileDropDown";
			this->FileDropDown->Size = System::Drawing::Size(38, 22);
			this->FileDropDown->Text = L"File";
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(145, 22);
			this->openToolStripMenuItem->Text = L"Open";
			this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &SchemeEditor::openToolStripMenuItem_Click);
			// 
			// saveToolStripMenuItem
			// 
			this->saveToolStripMenuItem->Name = L"saveToolStripMenuItem";
			this->saveToolStripMenuItem->Size = System::Drawing::Size(145, 22);
			this->saveToolStripMenuItem->Text = L"Save";
			this->saveToolStripMenuItem->Click += gcnew System::EventHandler(this, &SchemeEditor::saveToolStripMenuItem_Click);
			// 
			// loadSchemeToolStripMenuItem
			// 
			this->loadSchemeToolStripMenuItem->Name = L"loadSchemeToolStripMenuItem";
			this->loadSchemeToolStripMenuItem->Size = System::Drawing::Size(145, 22);
			this->loadSchemeToolStripMenuItem->Text = L"Load Scheme";
			this->loadSchemeToolStripMenuItem->Click += gcnew System::EventHandler(this, &SchemeEditor::loadSchemeToolStripMenuItem_Click);
			// 
			// editClassValuesToolStripMenuItem
			// 
			this->editClassValuesToolStripMenuItem->Name = L"editClassValuesToolStripMenuItem";
			this->editClassValuesToolStripMenuItem->Size = System::Drawing::Size(145, 22);
			this->editClassValuesToolStripMenuItem->Text = L"Edit Class Values";
			this->editClassValuesToolStripMenuItem->Click += gcnew System::EventHandler(this, &SchemeEditor::editClassValuesToolStripMenuItem_Click);
			// 
			// SchemeEditor
			// 
			this->ClientSize = System::Drawing::Size(1060, 665);
			this->Controls->Add(this->toolStripContainer1);
			this->Location = System::Drawing::Point(1000, 1000);
			this->Name = L"SchemeEditor";
			this->Text = L"Data Editor";
			this->toolStripContainer1->ContentPanel->ResumeLayout(false);
			this->toolStripContainer1->ContentPanel->PerformLayout();
			this->toolStripContainer1->TopToolStripPanel->ResumeLayout(false);
			this->toolStripContainer1->TopToolStripPanel->PerformLayout();
			this->toolStripContainer1->ResumeLayout(false);
			this->toolStripContainer1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView))->EndInit();
			this->toolStrip1->ResumeLayout(false);
			this->toolStrip1->PerformLayout();
			this->ResumeLayout(false);

		}
#pragma endregion

	private: System::Void openToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void loadSchemeToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void applyScheme(int attributeVal);
	private: System::Void saveToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void ascendingOrderButton_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void sortAscending(System::String^ column);
	private: System::Void sortDesending(System::String^ column);
	private: System::Void descendingOrderButton_Click(System::Object^ sender, System::EventArgs^ e);
	private: System::Void editClassValuesToolStripMenuItem_Click(System::Object^ sender, System::EventArgs^ e);
};
}
