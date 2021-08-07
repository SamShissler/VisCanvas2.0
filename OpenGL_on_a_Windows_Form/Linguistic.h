#pragma once

#include "OpenGL.h"
#include <cctype>
#include <string>
#include <msclr\marshal_cppstd.h>
#include <map>

namespace VisCanvas {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace OpenGLForm;

	/// <summary>
	/// Summary for Settings
	/// </summary>
	public ref class Linguistic : public System::Windows::Forms::Form
	{
	public:
		Linguistic(COpenGL^ openGL)
		{
			OpenGL = openGL;

			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
			

			this->SetStyle(::ControlStyles::SupportsTransparentBackColor | ::ControlStyles::UserPaint | ::ControlStyles::AllPaintingInWmPaint |
				::ControlStyles::OptimizedDoubleBuffer,
				true);
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Linguistic()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::ComponentModel::IContainer^ components;
	
	private: System::Windows::Forms::RichTextBox^ description;

	protected:

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		OpenGLForm::COpenGL^ OpenGL;
		System::String^ lingDescrip;

	private: System::Windows::Forms::Timer^ drawingTimer;
	
#pragma region Windows Form Designer generated code
		   /// <summary>
		   /// Required method for Designer support - do not modify
		   /// the contents of this method with the code editor.
		   /// </summary>
		   void InitializeComponent(void)
		   {
			   this->drawingTimer = (gcnew System::Windows::Forms::Timer());
			   this->drawingTimer->Enabled = true;
			   this->drawingTimer->Interval = 10;
			   this->drawingTimer->Tick += gcnew System::EventHandler(this, &Linguistic::timer_tick);

			   this->description = (gcnew System::Windows::Forms::RichTextBox());
			   this->description->Size = System::Drawing::Size(760, 340);
			   this->description->Text = gcnew String(OpenGL->file->getLinguisticDescription().c_str());
			   this->description->Location = System::Drawing::Point(10, 10);
			   this->description->Font = gcnew System::Drawing::Font(L"Eras Demi ITC", 8);

			   this->Controls->Add(this->description);
		   }
#pragma endregion
	
	private: System::Void timer_tick(System::Object^ sender, System::EventArgs^ e) {
		UNREFERENCED_PARAMETER(sender);
		UNREFERENCED_PARAMETER(e);
		OpenGL->Render();
		OpenGL->SwapOpenGLBuffers();
	}

	};
}