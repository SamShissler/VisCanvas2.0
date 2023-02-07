#pragma once
#ifndef _THREADEDLOADINGFORM
#define _THREADEDLOADINGFORM
#include "stdafx.h"
#include <thread>
#include "loadingForm.h"

//the purpose of the loading form is to indicate to the user that the program is still running during computationally intesive parts 
//of the code

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std;
class threadedLoadingForm
{
	public: void loadLoadingForm(); //calls the thread for the loading form
	private: void threadedLoading(); //the code which initiates the loading form and closes the loading form when ready
	public: void terminateLoadingForm(); //the code which closes the the loading form thread
	private: bool loading = true; //when set to false, the loading form closes
	private: std::thread thread_obj; //gloabl thread_obj
};

#endif