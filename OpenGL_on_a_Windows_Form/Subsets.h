#pragma once

namespace VisCanvas {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Subsets
	/// </summary>
	public ref class Subsets : public System::Windows::Forms::Form
	{
	public:
		Subsets(COpenGL^ openGL)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			OpenGL = openGL;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Subsets()
		{
			if (components)
			{
				delete components;
			}
		}

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

	private:
		OpenGLForm::COpenGL^ OpenGL;

		System::Windows::Forms::ComboBox^ classSelection;

		System::Windows::Forms::DataGridView^ pointList;
		System::Windows::Forms::DataGridViewCheckBoxColumn^ pointSelected;
		System::Windows::Forms::DataGridViewCheckBoxColumn^ cubeSelected;
		System::Windows::Forms::DataGridViewTextBoxColumn^ pointIndex;
		System::Windows::Forms::DataGridViewTextBoxColumn^ pointOverlaps;
		System::Windows::Forms::DataGridViewTextBoxColumn^ pointCubes;
		System::Windows::Forms::DataGridViewTextBoxColumn^ pointCoordinates;
		
		System::Windows::Forms::Button^ applyButton;
		System::Windows::Forms::Button^ allButton;
		System::Windows::Forms::Button^ allCubesButton;

		bool checkedState;
		bool cubeCheckedState;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(Subsets::typeid));

			this->classSelection = (gcnew System::Windows::Forms::ComboBox());

			this->pointList = (gcnew System::Windows::Forms::DataGridView());
			this->pointSelected = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->pointIndex = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->pointOverlaps = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->pointCubes = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->cubeSelected = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->pointCoordinates = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			
			this->applyButton = (gcnew System::Windows::Forms::Button());
			this->allButton = (gcnew System::Windows::Forms::Button());
			this->allCubesButton = (gcnew System::Windows::Forms::Button());

			checkedState = true;
			cubeCheckedState = true;

			this->SuspendLayout();

			//
			// Main Window
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1000, 600);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Controls->Add(this->classSelection);
			this->Controls->Add(this->pointList);
			this->Controls->Add(this->applyButton);
			this->Controls->Add(this->allButton);
			this->Controls->Add(this->allCubesButton);
			this->Name = L"subsets";
			this->ShowIcon = false;
			this->ShowInTaskbar = false;
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
			this->Text = L"Select Subsets";
			this->Load += gcnew System::EventHandler(this, &Subsets::subsetsLoad);
			this->ResumeLayout(false);
			this->PerformLayout();

			//
			// class selection
			//
			this->classSelection->Location = System::Drawing::Point(100, 10);
			this->classSelection->Name = L"classSelection";
			this->classSelection->Size = System::Drawing::Size(800, 50);
			this->classSelection->Text = L"select...";
			this->classSelection->SelectedIndexChanged += gcnew System::EventHandler(this, &Subsets::loadPoints);

			// 
			// stats list
			//
			this->pointList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(5) {
				this->pointSelected,
					this->pointIndex,
					this->cubeSelected,
					this->pointCubes,
					this->pointOverlaps
			});
			this->pointList->Location = System::Drawing::Point(100, 40);
			this->pointList->AllowUserToAddRows = false;
			this->pointList->AllowUserToDeleteRows = false;
			this->pointList->RowHeadersVisible = false;
			this->pointList->ShowEditingIcon = false;
			this->pointList->Name = L"pointList";
			this->pointList->Size = System::Drawing::Size(800, 500);

			// 
			// Checkbox column
			// 
			this->pointSelected->HeaderText = L"Select Samples";
			this->pointSelected->MinimumWidth = 45;
			this->pointSelected->Name = L"pointSelected";

			// 
			// Index column
			// 
			this->pointIndex->HeaderText = L"Sample";
			this->pointIndex->MinimumWidth = 45;
			this->pointIndex->Name = L"pointIndex";

			// 
			// Select cubes column
			// 
			this->cubeSelected->HeaderText = L"Select Cubes";
			this->cubeSelected->MinimumWidth = 45;
			this->cubeSelected->Name = L"pointCoordinates";

			// 
			// Cubes column
			// 
			this->pointCubes->HeaderText = L"Impurities";
			this->pointCubes->MinimumWidth = 45;
			this->pointCubes->Name = L"pointCoordinates";

			// 
			// Overlaps column
			// 
			this->pointOverlaps->HeaderText = L"Occlusions";
			this->pointOverlaps->MinimumWidth = 45;
			this->pointOverlaps->Name = L"pointOverlaps";

			//
			// Apply button
			//
			this->applyButton->BackColor = System::Drawing::SystemColors::ButtonFace;
			this->applyButton->Location = System::Drawing::Point(500, 545);
			this->applyButton->Name = L"applyButton";
			this->applyButton->Size = System::Drawing::Size(400, 40);
			this->applyButton->Text = L"Apply";
			this->applyButton->UseVisualStyleBackColor = false;
			this->applyButton->Click += gcnew System::EventHandler(this, &Subsets::applyClick);

			//
			// All button
			//
			this->allButton->BackColor = System::Drawing::SystemColors::ButtonFace;
			this->allButton->Location = System::Drawing::Point(100, 545);
			this->allButton->Name = L"allButton";
			this->allButton->Size = System::Drawing::Size(200, 40);
			this->allButton->Text = L"Select/Deselect All Samples";
			this->allButton->UseVisualStyleBackColor = false;
			this->allButton->Click += gcnew System::EventHandler(this, &Subsets::allClick);

			//
			// All cubes button
			//
			this->allCubesButton->BackColor = System::Drawing::SystemColors::ButtonFace;
			this->allCubesButton->Location = System::Drawing::Point(300, 545);
			this->allCubesButton->Name = L"allButton";
			this->allCubesButton->Size = System::Drawing::Size(200, 40);
			this->allCubesButton->Text = L"Select/Deselect All Cubes";
			this->allCubesButton->UseVisualStyleBackColor = false;
			this->allCubesButton->Click += gcnew System::EventHandler(this, &Subsets::allCubesClick);
		}

		private: System::Void loadPoints(System::Object^ sender, System::EventArgs^ e)
		{
			this->pointList->Columns->Clear(); // refreshes the list

			cli::array<System::Windows::Forms::DataGridViewColumn^>^ items = gcnew cli::array<System::Windows::Forms::DataGridViewColumn^>(OpenGL->file->getDimensionAmount() + 5);
			items[0] = this->pointSelected;
			items[1] = this->pointIndex;
			items[2] = this->cubeSelected;
			items[3] = this->pointCubes;
			items[4] = this->pointOverlaps;
			for (int i = 5; i < OpenGL->file->getDimensionAmount() + 5; i++) 
			{	
				items[i] = gcnew System::Windows::Forms::DataGridViewTextBoxColumn();
				items[i]->HeaderText = gcnew System::String((*OpenGL->file->getDimensionName(i - 5)).c_str());
			}
			this->pointList->Columns->AddRange(items);
			
			std::vector<int>* setIndexList = OpenGL->file->getSetsInClass(this->classSelection->SelectedIndex);
			for (int i = 0; i < setIndexList->size(); i++)
			{
				System::String^ impurityCount = gcnew System::String((to_string(OpenGL->file->getImpurities(setIndexList->at(i))) + " / " + to_string(OpenGL->file->getOverlaps()->at(setIndexList->at(i)).getSetNumber())).c_str());
				bool isSelected = OpenGL->file->getOverlaps()->at(setIndexList->at(i)).isDisplayed();
				System::String^ temp = gcnew System::String((*OpenGL->getSetName(setIndexList->at(i))).c_str());
				this->pointList->Rows->Add(OpenGL->isVisible(setIndexList->at(i)), temp, isSelected, impurityCount, OpenGL->file->getOverlappingCount(setIndexList->at(i)));
				for (int j = 0; j < OpenGL->file->getDimensionAmount(); j++)
				{
					this->pointList->Rows[i]->Cells[5 + j]->Value = OpenGL->file->getData(setIndexList->at(i), j);
				}
			}
		}

		private: System::Void applyClick(System::Object^ sender, System::EventArgs^ e)
		{
			if (OpenGL->uploadedFile()) 
			{
				int currentIndex = this->classSelection->SelectedIndex;
				std::vector<int>* setIndexList = OpenGL->file->getSetsInClass(currentIndex);
				for (int i = 0; i < setIndexList->size(); i++) 
				{
					if (Convert::ToBoolean(this->pointList->Rows[i]->Cells[0]->Value))
					{
						OpenGL->file->setVisible(setIndexList->at(i), true);
					}
					else
					{
						OpenGL->file->setVisible(setIndexList->at(i), false);
					}
					
					if (Convert::ToBoolean(this->pointList->Rows[i]->Cells[2]->Value))
					{
						OpenGL->file->getOverlaps()->at(setIndexList->at(i)).setDisplayed(true);
					}
					else
					{
						OpenGL->file->getOverlaps()->at(setIndexList->at(i)).setDisplayed(false);
					}
				}
			}		
		}

		private: System::Void allClick(System::Object^ sender, System::EventArgs^ e)
		{
			if (OpenGL->uploadedFile())
			{
				int currentIndex = this->classSelection->SelectedIndex;
				std::vector<int>* setIndexList = OpenGL->file->getSetsInClass(currentIndex);
				checkedState = !checkedState;
				for (int i = 0; i < setIndexList->size(); i++)
				{
					OpenGL->file->setVisible(setIndexList->at(i), checkedState);
					this->pointList->Rows[i]->Cells[0]->Value = checkedState;
				}
			}
		}

		private: System::Void allCubesClick(System::Object^ sender, System::EventArgs^ e)
		{
			if (OpenGL->uploadedFile())
			{
				int currentIndex = this->classSelection->SelectedIndex;
				std::vector<int>* setIndexList = OpenGL->file->getSetsInClass(currentIndex);
				cubeCheckedState = !cubeCheckedState;
				for (int i = 0; i < setIndexList->size(); i++)
				{
					OpenGL->file->getOverlaps()->at(setIndexList->at(i)).setDisplayed(cubeCheckedState);
					this->pointList->Rows[i]->Cells[2]->Value = cubeCheckedState;
				}
			}
		}

		private: System::Void subsetsLoad(System::Object^ sender, System::EventArgs^ e)
		{
			// Setup of the classes
			this->classSelection->Items->Clear();
			cli::array<System::Object^>^ items = gcnew cli::array<System::Object^>(OpenGL->file->getClassAmount());
			for (int i = 0; i < OpenGL->file->getClassAmount(); i++) {
				System::String^ mString = gcnew System::String((*OpenGL->file->getClasses()[i].getName()).c_str());
				items[i] = (Object^)mString;
			}
			this->classSelection->Items->AddRange(items);
		}

	};
}