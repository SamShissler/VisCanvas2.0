#include "stdafx.h"
#include "threadedLoadingForm.h"
#include <chrono>
#include <thread>
#include "loadingForm.h"

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds
using namespace std;


void threadedLoadingForm::loadLoadingFormFunc(bool* loading)
{
	VisCanvas::loadingForm^ form = gcnew VisCanvas::loadingForm();
	form->Show();
	form->Update();
	int n = 0;
	while (*loading)
	{
		string formText;
		if (n == 0)
			form->updateLabel("loading");
		else if (n == 1)
			form->updateLabel("loading.");
		else if (n == 2)
			form->updateLabel("loading..");
		else if (n == 3)
		{
			form->updateLabel("loading...");
			n = -1;//this way n = 0 upon n++ below
		}
		n++;

		form->Update();
		sleep_until(system_clock::now() + milliseconds(500));//half a second between every update
	}
	form->Close();
}
