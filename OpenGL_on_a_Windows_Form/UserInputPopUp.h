#pragma once
#include <string>

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class UserInputPopUp : public System::Windows::Forms::Form
	{
	public: UserInputPopUp(String^ name, String^ prompt)
	{
		this->name = name;
		this->Prompt = prompt;
		InitializeComponent();
	}

	protected:~UserInputPopUp()
	{
		if (components)
		{
			delete components;
		}
	}


	private:System::String^ path;
	private: System::Windows::Forms::Button^ confirmButton;
	private: System::Windows::Forms::Label^ userPrompt;
	private: System::Windows::Forms::TextBox^ inputFeild;
	private: System::ComponentModel::Container^ components;
	
	private: std::string* result;
	private: String^ name;
	private: String^ Prompt;

#pragma region

		   void InitializeComponent(void)
		   {
			   this->confirmButton = (gcnew System::Windows::Forms::Button());
			   this->userPrompt = (gcnew System::Windows::Forms::Label());
			   this->inputFeild = (gcnew System::Windows::Forms::TextBox());
			   this->SuspendLayout();
			   // 
			   // confirmButton
			   // 
			   this->confirmButton->Location = System::Drawing::Point(115, 70);
			   this->confirmButton->Name = L"confirmButton";
			   this->confirmButton->Size = System::Drawing::Size(75, 23);
			   this->confirmButton->TabIndex = 1;
			   this->confirmButton->Text = L"Confirm";
			   this->confirmButton->UseVisualStyleBackColor = true;
			   this->confirmButton->Click += gcnew System::EventHandler(this, &UserInputPopUp::onConfirm);
			   //
			   // inputFeild
			   //
			   this->inputFeild->Location = System::Drawing::Point(100, 40);
			   this->inputFeild->Name = L"inputFeild";
			   this->inputFeild->Size = System::Drawing::Size(100, 20);
			   this->inputFeild->TabIndex = 2;
			   //
			   // userPrompt
			   //
			   this->userPrompt->AutoSize = true;
			   this->userPrompt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 7, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->userPrompt->Location = System::Drawing::Point(5, 5);
			   this->userPrompt->Name = L"userPrompt";
			   this->userPrompt->Size = System::Drawing::Size(154, 20);
			   this->userPrompt->TabIndex = 1;
			   this->userPrompt->Text = this->Prompt;
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(300, 100);
			   this->Controls->Add(this->confirmButton);
			   this->Controls->Add(this->inputFeild);
			   this->Controls->Add(this->userPrompt);
			   this->Name = this->name;
			   this->Text = this->name;
			   this->ResumeLayout(false);

		   }

#pragma endregion
	private: System::Void onConfirm(System::Object^ sender, System::EventArgs^ e)
	{
		this->Close();
	}
	public: String^ getResult()
	{
		return(this->inputFeild->Text);
	}

	};
}
