#pragma once
#include <string>
#include <ctime>

namespace CppCLRWinformsProjekt {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	public ref class LoadingScreen : public System::Windows::Forms::Form
	{
	public: LoadingScreen(String^ name, String^ prompt)
	{
		this->name = name;
		this->Prompt = prompt;

		InitializeComponent();

		time_t startTime = time(0);
		tm* startTimePtr = localtime(&startTime);
		startHour = startTimePtr->tm_hour;
		startMin = startTimePtr->tm_min;
		startSec = startTimePtr->tm_sec;
	}

	protected:~LoadingScreen()
	{
		if (components)
		{
			delete components;
		}
	}

	private:System::String^ path;
	private: System::Windows::Forms::Label^ userPrompt;
	private: System::ComponentModel::Container^ components;
	private: System::Windows::Forms::Button^ confirmButton;

	private: String^ name;
	private: String^ Prompt;
	private: int dimensionsToTest = 0;
	private: int curDimension = 0;
	private: int startHour;
	private: int startMin;
	private: int startSec;


#pragma region

		   void InitializeComponent(void)
		   {
			   this->confirmButton = (gcnew System::Windows::Forms::Button());
			   this->userPrompt = (gcnew System::Windows::Forms::Label());
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
			   this->confirmButton->Click += gcnew System::EventHandler(this, &LoadingScreen::onConfirm);
			   //
			   // userPrompt
			   //
			   this->userPrompt->AutoSize = true;
			   this->userPrompt->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				   static_cast<System::Byte>(0)));
			   this->userPrompt->Location = System::Drawing::Point(40, 15);
			   this->userPrompt->Name = L"userPrompt";
			   this->userPrompt->Size = System::Drawing::Size(154, 20);
			   this->userPrompt->TabIndex = 1;
			   this->userPrompt->Text = this->Prompt + "\n\n Progress: calulating " + curDimension + " out of " + dimensionsToTest;
			   // 
			   // Form1
			   // 
			   this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			   this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			   this->ClientSize = System::Drawing::Size(300, 100);
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

	public: void setDimensionsCurrent(int dimensionNum)
	{
		curDimension = dimensionNum;
	}

	public: void setTotalDimensionToTest(int dimensionNum)
	{
		dimensionsToTest = dimensionNum;
		this->userPrompt->Text = this->Prompt + "\nProgress: calulating " + curDimension + " out of " + dimensionsToTest;
	}

	public: void updateTime()
	{
		time_t now = time(0);
		tm* curTimePtr = localtime(&now);

		int curHour = curTimePtr->tm_hour;
		int curMin = curTimePtr->tm_min;
		int curSec = curTimePtr->tm_sec;

		int displayTimeHour = curHour - startHour;
		int displayTimeMin = curMin - startMin;
		int displayTimeSec = curSec - startSec;

		if (displayTimeHour < 0)
		{
			displayTimeHour = startHour - curHour;
		}

		if (displayTimeMin < 0)
		{
			displayTimeMin = startMin - curMin;
		}

		if (displayTimeSec < 0)
		{
			displayTimeSec = startSec - curSec;
		}
		
		this->userPrompt->Text = this->Prompt + "\nProgress: calulating " + curDimension + " out of " + dimensionsToTest + 
			"\nTime Elapsed: " + displayTimeHour + ":" + displayTimeMin + ":" + displayTimeSec;
	}

	};
}
