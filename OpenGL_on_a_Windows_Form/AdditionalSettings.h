#pragma once

#include "OpenGL.h"

namespace VisCanvas {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for Options
	/// </summary>
	public ref class AdditionalSettings : public System::Windows::Forms::Form
	{
	public:
		AdditionalSettings(OpenGLForm::COpenGL^ openGL)
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
		~AdditionalSettings()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TabControl^  tabControl1;
	protected: 
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::CheckBox^  checkBox1;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
	private: System::Windows::Forms::Button^ applyButton;
	private: System::Windows::Forms::Button^ applyButton2;
	private: System::Windows::Forms::RadioButton^  radioButton2;
	private: System::Windows::Forms::RadioButton^  radioButton1;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::CheckBox^  checkBox4;
	private: System::Windows::Forms::CheckBox^  checkBox3;
	private: System::Windows::Forms::CheckBox^  checkBox2;
			 OpenGLForm::COpenGL^ OpenGL;
#pragma region Windows Form Designer generated code
			 /// <summary>
			 /// Required method for Designer support - do not modify
			 /// the contents of this method with the code editor.
			 /// </summary>
			 void InitializeComponent(void)
			 {
				 System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(AdditionalSettings::typeid));
				 this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
				 this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
				 this->applyButton = (gcnew System::Windows::Forms::Button());
				 this->applyButton2 = (gcnew System::Windows::Forms::Button());
				 this->label1 = (gcnew System::Windows::Forms::Label());
				 this->textBox1 = (gcnew System::Windows::Forms::TextBox());
				 this->button1 = (gcnew System::Windows::Forms::Button());
				 this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
				 this->checkBox4 = (gcnew System::Windows::Forms::CheckBox());
				 this->checkBox3 = (gcnew System::Windows::Forms::CheckBox());
				 this->checkBox2 = (gcnew System::Windows::Forms::CheckBox());
				 this->label3 = (gcnew System::Windows::Forms::Label());
				 this->radioButton2 = (gcnew System::Windows::Forms::RadioButton());
				 this->radioButton1 = (gcnew System::Windows::Forms::RadioButton());
				 this->label2 = (gcnew System::Windows::Forms::Label());
				 this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
				 this->tabControl1->SuspendLayout();
				 this->tabPage1->SuspendLayout();
				 this->tabPage2->SuspendLayout();
				 this->SuspendLayout();
				 // 
				 // tabControl1
				 // 
				 this->tabControl1->Controls->Add(this->tabPage1);
				 this->tabControl1->Controls->Add(this->tabPage2);
				 this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
				 this->tabControl1->Location = System::Drawing::Point(0, 0);
				 this->tabControl1->Name = L"tabControl1";
				 this->tabControl1->SelectedIndex = 0;
				 this->tabControl1->Size = System::Drawing::Size(412, 229);
				 this->tabControl1->TabIndex = 0;
				 // 
				 // tabPage1
				 // 
				 this->tabPage1->BackColor = System::Drawing::Color::LightGray;
				 this->tabPage1->Controls->Add(this->applyButton);
				 this->tabPage1->Controls->Add(this->label1);
				 this->tabPage1->Controls->Add(this->textBox1);
				 this->tabPage1->Controls->Add(this->button1);
				 this->tabPage1->Location = System::Drawing::Point(4, 22);
				 this->tabPage1->Name = L"tabPage1";
				 this->tabPage1->Padding = System::Windows::Forms::Padding(3);
				 this->tabPage1->Size = System::Drawing::Size(404, 203);
				 this->tabPage1->TabIndex = 0;
				 this->tabPage1->Text = L"Hypercube";
				 // 
				 // applyButton
				 // 
				 this->applyButton->AutoSize = false;
				 this->applyButton->Location = System::Drawing::Point(340, 174);
				 this->applyButton->Name = L"nButtonControl13";
				 this->applyButton->Size = System::Drawing::Size(56, 23);
				 this->applyButton->TabIndex = 47;
				 this->applyButton->Text = L"Apply";
				 this->applyButton->Click += gcnew System::EventHandler(this, &AdditionalSettings::applyButton_Click);
				 // 
				 // applyButton2
				 // 
				 this->applyButton2->AutoSize = false;
				 this->applyButton2->Location = System::Drawing::Point(340, 174);
				 this->applyButton2->Name = L"nButtonControl13";
				 this->applyButton2->Size = System::Drawing::Size(56, 23);
				 this->applyButton2->TabIndex = 47;
				 this->applyButton2->Text = L"Apply";
				 this->applyButton2->Click += gcnew System::EventHandler(this, &AdditionalSettings::AdditionalSettings_Apply);
				 
				 // 
				 // label1
				 // 
				 this->label1->AutoSize = true;
				 this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold));
				 this->label1->Location = System::Drawing::Point(218, 73);
				 this->label1->Name = L"label1";
				 this->label1->Size = System::Drawing::Size(86, 17);
				 this->label1->TabIndex = 2;
				 this->label1->Text = L"Threshold:";
				 // 
				 // textBox1
				 // 
				 this->textBox1->Location = System::Drawing::Point(221, 93);
				 this->textBox1->Name = L"textBox1";
				 this->textBox1->Size = System::Drawing::Size(100, 20);
				 this->textBox1->TabIndex = 1;
				 this->textBox1->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
				 this->textBox1->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &AdditionalSettings::radiusKeyPress);
				 // 
				 // button1
				 // 
				 this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9, System::Drawing::FontStyle::Bold));
				 this->button1->Location = System::Drawing::Point(76, 64);
				 this->button1->Name = L"button1";
				 this->button1->Size = System::Drawing::Size(65, 61);
				 this->button1->TabIndex = 0;
				 this->button1->Text = L"Use Mean";
				 this->button1->UseVisualStyleBackColor = true;
				 this->button1->Click += gcnew System::EventHandler(this, &AdditionalSettings::button1_Click);
				 // 
				 // tabPage2
				 // 
				 this->tabPage2->BackColor = System::Drawing::Color::LightGray;
				 this->tabPage2->Controls->Add(this->checkBox4);
				 this->tabPage2->Controls->Add(this->checkBox3);
				 this->tabPage2->Controls->Add(this->checkBox2);
				 this->tabPage2->Controls->Add(this->label3);
				 this->tabPage2->Controls->Add(this->radioButton2);
				 this->tabPage2->Controls->Add(this->radioButton1);
				 this->tabPage2->Controls->Add(this->label2);
				 this->tabPage2->Controls->Add(this->checkBox1);
				 this->tabPage2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F));
				 this->tabPage2->Location = System::Drawing::Point(4, 22);
				 this->tabPage2->Name = L"tabPage2";
				 this->tabPage2->Padding = System::Windows::Forms::Padding(3);
				 this->tabPage2->Size = System::Drawing::Size(404, 203);
				 this->tabPage2->TabIndex = 1;
				 this->tabPage2->Text = L"Options";
				 // 
				 // checkBox4
				 // 
				 this->checkBox4->AutoSize = true;
				 this->checkBox4->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F));
				 this->checkBox4->Location = System::Drawing::Point(11, 131);
				 this->checkBox4->Name = L"checkBox4";
				 this->checkBox4->Size = System::Drawing::Size(87, 19);
				 this->checkBox4->TabIndex = 54;
				 this->checkBox4->Text = L"Case Names";
				 this->checkBox4->UseVisualStyleBackColor = true;
				 // 
				 // checkBox3
				 // 
				 this->checkBox3->AutoSize = true;
				 this->checkBox3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F));
				 this->checkBox3->Location = System::Drawing::Point(11, 103);
				 this->checkBox3->Name = L"checkBox3";
				 this->checkBox3->Size = System::Drawing::Size(99, 19);
				 this->checkBox3->TabIndex = 53;
				 this->checkBox3->Text = L"Class Names";
				 this->checkBox3->UseVisualStyleBackColor = true;
				 // 
				 // checkBox2
				 // 
				 this->checkBox2->AutoSize = true;
				 this->checkBox2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.5F));
				 this->checkBox2->Location = System::Drawing::Point(11, 75);
				 this->checkBox2->Name = L"checkBox2";
				 this->checkBox2->Size = System::Drawing::Size(129, 19);
				 this->checkBox2->TabIndex = 52;
				 this->checkBox2->Text = L"Coordinate Names";
				 this->checkBox2->UseVisualStyleBackColor = true;
				 // 
				 // label3
				 // 
				 this->label3->AutoSize = true;
				 this->label3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					 static_cast<System::Byte>(0)));
				 this->label3->Location = System::Drawing::Point(8, 25);
				 this->label3->Name = L"label3";
				 this->label3->Size = System::Drawing::Size(124, 34);
				 this->label3->TabIndex = 51;
				 this->label3->Text = L"CSV File Format\r\nContains:";
				 this->label3->Click += gcnew System::EventHandler(this, &AdditionalSettings::label3_Click);
				 // 
				 // radioButton2
				 // 
				 this->radioButton2->AutoSize = true;
				 this->radioButton2->Checked = true;
				 this->radioButton2->Location = System::Drawing::Point(256, 75);
				 this->radioButton2->Name = L"radioButton2";
				 this->radioButton2->Size = System::Drawing::Size(64, 19);
				 this->radioButton2->TabIndex = 49;
				 this->radioButton2->TabStop = true;
				 this->radioButton2->Text = L"Bottom";
				 this->radioButton2->UseVisualStyleBackColor = true;
				 // 
				 // radioButton1
				 // 
				 this->radioButton1->AutoSize = true;
				 this->radioButton1->Location = System::Drawing::Point(256, 51);
				 this->radioButton1->Name = L"radioButton1";
				 this->radioButton1->Size = System::Drawing::Size(46, 19);
				 this->radioButton1->TabIndex = 48;
				 this->radioButton1->Text = L"Top";
				 this->radioButton1->UseVisualStyleBackColor = true;
				
				 // label2
				 // 
				 this->label2->AutoSize = true;
				 this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					 static_cast<System::Byte>(0)));
				 this->label2->Location = System::Drawing::Point(215, 105);
				 this->label2->Name = L"label2";
				 this->label2->Size = System::Drawing::Size(148, 17);
				 this->label2->TabIndex = 2;
				 this->label2->Text = L"Selected Case Color:";
				  
				 // checkBox1
				 // 
				 this->checkBox1->AutoSize = true;
				 this->checkBox1->Checked = true;
				 this->checkBox1->CheckState = System::Windows::Forms::CheckState::Checked;
				 this->checkBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
					 static_cast<System::Byte>(0)));
				 this->checkBox1->Location = System::Drawing::Point(182, 24);
				 this->checkBox1->Name = L"checkBox1";
				 this->checkBox1->Size = System::Drawing::Size(214, 21);
				 this->checkBox1->TabIndex = 0;
				 this->checkBox1->Text = L"Display Coordinate Names";
				 this->checkBox1->UseVisualStyleBackColor = true;
				 this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &AdditionalSettings::checkBox1_CheckedChanged);
				 // 
				 // AdditionalSettings
				 // 
				 this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
				 this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
				 this->ClientSize = System::Drawing::Size(412, 229);
				 this->Controls->Add(this->tabControl1);
				 this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedToolWindow;
				 this->MaximizeBox = false;
				 this->MinimizeBox = false;
				 this->Name = L"AdditionalSettings";
				 this->ShowIcon = false;
				 this->StartPosition = System::Windows::Forms::FormStartPosition::CenterParent;
				 this->Text = L"Options";
				 this->Load += gcnew System::EventHandler(this, &AdditionalSettings::AdditionalSettings_Load);
				 this->tabControl1->ResumeLayout(false);
				 this->tabPage1->ResumeLayout(false);
				 this->tabPage1->PerformLayout();
				 this->tabPage2->ResumeLayout(false);
				 this->tabPage2->PerformLayout();
				 this->ResumeLayout(false);

			 }
#pragma endregion
	
	private: System::Void radiusKeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {

				 if (!Char::IsControl(e->KeyChar) && !Char::IsDigit(e->KeyChar) && (e->KeyChar != '.') && (e->KeyChar != '-')) {
					 e->Handled = true;
				 }

				 // only allow one decimal point
				 if ((e->KeyChar == '.') && safe_cast<TextBox^>(sender)->Text->IndexOf('.') > -1) {
					 e->Handled = true;
				 }

				 // only allow for one negative sign and only at the front of the numer
				 if ((e->KeyChar == '-') && safe_cast<TextBox^>(sender)->SelectionStart != 0) {
					 e->Handled = true;
				 } 

				 if ((e->KeyChar == '-') && safe_cast<TextBox^>(sender)->SelectionStart == 0 && safe_cast<TextBox^>(sender)->Text->IndexOf('-') == 0) {
					 e->Handled = true;
				 }

				 if (( (Char::IsDigit(e->KeyChar)) || (e->KeyChar == '.') )  && safe_cast<TextBox^>(sender)->SelectionStart == 0 && safe_cast<TextBox^>(sender)->Text->IndexOf('-') == 0) {
					 e->Handled = true;
				 }
			 }
	private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {

				 if (!(this->checkBox1->Checked))
				 {
					 this->radioButton1->Enabled = false;
					 this->radioButton2->Enabled = false;
				 } else
				 {
					 this->radioButton1->Enabled = true;
					 this->radioButton2->Enabled = true;
				 }

			 }
	
	private:

		/* Converts from System::String^ to std::string */
		std::string *toStandardString(System::String^ str)
		{
			const char* location = (const char*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi( str ).ToPointer();
			std::string *temp = new std::string(location);
			/* Clean up the const char* location memory */
			System::Runtime::InteropServices::Marshal::FreeHGlobal(IntPtr((void*)location));
			return temp;
		}	
	private:
		System::Void applyRadius(System::Void)
		{
			if (OpenGL->uploadedFile())
			{
				std::string *str = toStandardString(this->textBox1->Text);
				double newRadius = std::stod(*str);
				OpenGL->file->setRadius(newRadius);
				delete str;
			}
		}

	private: System::Void applyButton_Click(System::Object^ sender, System::EventArgs^ e) {
				this->applyRadius();
			}
	private: System::Void AdditionalSettings_Load(System::Object^  sender, System::EventArgs^  e) {

				 this->checkBox2->Checked = OpenGL->file->isReadDimensionNames();
				 this->checkBox3->Checked = OpenGL->file->isReadClassNames();
				 this->checkBox4->Checked = OpenGL->file->isReadSetNames();

				 if (OpenGL->uploadedFile())
				 {
					 this->checkBox1->Checked = OpenGL->textOnEnabled();
					 this->radioButton1->Checked = OpenGL->textOnTop();
					 this->radioButton2->Checked = OpenGL->textOnBottom();

					 std::string str = std::to_string(OpenGL->file->getRadius());

					 this->textBox1->Text = gcnew System::String(str.c_str());
				 }

			 }
	private: System::Void AdditionalSettings_Apply(System::Object^ sender, System::EventArgs^ e) {

		DataInterface * x = OpenGL->file;
		x->setReadDimensionNames(this->checkBox2->Checked);
		x->setReadClassNames(this->checkBox3->Checked);
		x->setReadSetNames(this->checkBox4->Checked);

		if (OpenGL->uploadedFile())
		{
			OpenGL->setTextOnEnabled(this->checkBox1->Checked);
			OpenGL->setTextOnTop(this->radioButton1->Checked);
			OpenGL->setTextOnBottom(this->radioButton2->Checked);

			std::string str = std::to_string(OpenGL->file->getRadius());

			this->textBox1->Text = gcnew System::String(str.c_str());
		}

	}
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
				 bool useMeanState = OpenGL->file->isUseMeanForClusters();
				 if (useMeanState)
				 {
					 this->button1->BackColor = System::Drawing::SystemColors::ButtonHighlight;
				 }
				 else
				 {
					 this->button1->BackColor = System::Drawing::SystemColors::ButtonFace;
				 }
				 OpenGL->file->setUseMeanForClusters(!useMeanState);
			 }
	private: System::Void label3_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
	};
}
