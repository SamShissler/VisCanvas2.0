#pragma once

/*
Authors: Shane Vance, Nico Espitia, Brennan Barni, Daniel Ortyn
Last Update: 2018/06/03
Purpose: CS 481 Project
*/

#include "stdafx.h"
#include "DomNominalSet.h"
#include <atlstr.h>
#include <vector>
#include <unordered_map>
#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <utility>

using namespace System::Windows::Forms;

namespace OpenGLForm
{

	public ref class COpenGL : public System::Windows::Forms::NativeWindow
	{

	public:
		/**
		 * This sets up the OpenGL viewport that is used in conjuction with the DataInterface class
		 * @param parentForm
		 * @param iWidth
		 * @param iHeight
		 */
		COpenGL(System::Windows::Forms::Form^ parentForm, GLsizei iWidth, GLsizei iHeight)
		{
			CreateParams^ cp = gcnew CreateParams;

			//Set the position on the form
			cp->X = 0;
			cp->Y = 22; // accounts for the height of the status strip
			cp->Width = iWidth;
			cp->Height = iHeight;

			// specify the form as the parent.
			cp->Parent = parentForm->Handle;

			// create as a child of the specified parent and make OpenGL compliant (no clipping)
			cp->Style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			// create the actual window
			this->CreateHandle(cp);

			// set the world height and width for later
			this->worldWidth = iWidth;
			this->worldHeight = iHeight;

			// Create the data interface
			this->file = new DataInterface();

			this->uploadFile = false;

			// disable the toggle buttons by default
			this->drawingDragged = false;
			this->shiftHorizontal = false;
			this->shiftVertical = false;

			this->textEnabled = true;
			this->textTop = false;
			this->textBottom = true;
			this->invertDimensionToggled = false;
			this->hypercubeToggle = true;
			this->nominalSetsToggle = true;

			this->zoom = 0;

			this->clickedDimension = -1;

			this->font_list_base_2d = 2000;


			m_hDC = GetDC((HWND)this->Handle.ToPointer());

			// check if everything went A-Okay when creating the Child Handle
			if (m_hDC)
			{
				this->MySetPixelFormat(m_hDC);
				this->Reshape(iWidth, iHeight);
				this->Init();
			}
		}


		// SET THE FILE PATH THAT WILL BE USED TO GRAPH THE DATA
		bool SetFilePath(std::string filepath)
		{

			this->zoom = 0;
			this->tempXWorldMouseDifference = 0;
			this->tempYWorldMouseDifference = 0;

			try
			{
				// read the file
				if (!(this->file->readFile(&filepath)))
				{
					throw std::exception();
				}
				else {
					this->uploadFile = true;
					return false;
				}
			}
			catch (...) {
				// display an error message
				DialogResult result = MessageBox::Show("WARNING: VisCanvas is unable to open the file. Click 'Retry' to try again.", "Trouble Opening File", MessageBoxButtons::RetryCancel, MessageBoxIcon::Warning);
				this->uploadFile = false;
				if (result == DialogResult::Retry)
				{
					return true; // reopen the file dialog
				}
				if (result == DialogResult::Cancel)
				{
					return false; // don't proceed to the file dialog
				}
			}

		}

		// BUTTON METHODS GO HERE //

		/**
		 * Controls incrementing a selected set
		 * @return
		 */
		System::Void incrementSelectedSet(System::Void)
		{
			this->file->incrementSelectedSetIndex();
		}

		System::Void incrementSelectedSetIndexCluster(System::Void)
		{
			this->file->incrementSelectedSetIndexCluster();
		}

		/**
		 * Controls decrementing a selected set
		 * @return
		 */
		System::Void decrementSelectedSet(System::Void)
		{
			this->file->decrementSelectedSetIndex();
		}

		/**
		 * Controls doing ascending sort of dimensions
		 * @return
		 */
		System::Void ascendingSort(System::Void)
		{
			int selectedSetIndex = this->file->getSelectedSetIndex();
			this->file->sortAscending(selectedSetIndex);
		}

		/**
		 * Controls doing descending sort of dimensions
		 * @return
		 */
		System::Void descendingSort(System::Void)
		{
			int selectedSetIndex = this->file->getSelectedSetIndex();
			this->file->sortDescending(selectedSetIndex);
		}

		/**
		 * Controls reverting the sort of dimensions
		 * @return
		 */
		System::Void originalSort(System::Void)
		{
			int selectedSetIndex = this->file->getSelectedSetIndex();
			this->file->sortOriginal();
		}

		/**
		 * Controls toggling the mean shift (normalization) of dimensions
		 * @return
		 */
		System::Void mean(System::Void)
		{
			int selectedSetIndex = this->file->getSelectedSetIndex();
			double meanOfSet = this->file->getMean(selectedSetIndex);
			this->file->level(selectedSetIndex, meanOfSet);

		}

		System::Void straightenSpace(System::Void)
		{
			/*int clusterIdx = file->getSelectedClusterIndex();
			SetCluster selected = file->getClusters().at(clusterIdx);

			vector<int> * sets = selected.getSets();
			int currMin = 0;

			for (int i = 0; i < sets->size(); i++)
			{
				int minCount = 0;
				for (int j = 0; j < file->getDimensionAmount(); j++)
				{
					if (file->getData(sets->at(i),j) <= file->getData(currMin, j));
					{
						minCount++;
					}
				}
				if (minCount >= getDimensionAmount() / 2)
				{
					currMin = sets->at(i);
				}
			}*/

			// sort dimensions


			// file->setSelectedSetIndex(currMin);

			int selectedSetIndex = this->file->getSelectedSetIndex();

			this->file->sortAscending(selectedSetIndex);

			double meanOfSet = this->file->getMean(selectedSetIndex);
			this->file->level(selectedSetIndex, meanOfSet);
		}

		/**
		 * Controls toggling the median shift (normalization) of dimensions
		 * @return
		 */
		System::Void median(System::Void)
		{
			int selectedSetIndex = this->file->getSelectedSetIndex();
			double medianOfSet = this->file->getMedian(selectedSetIndex);
			this->file->level(selectedSetIndex, medianOfSet);
		}

		/**
		 * Controls toggling reverting the shift (normalization) of dimensions
		 * to its orginal state prior to sorting
		 * @return
		 */
		System::Void originalData(System::Void)
		{
			this->file->zeroShifts();
		}

		/**
		 * This control toggling whether the user wants the hypercube clusters visible or not
		 * @return
		 */
		System::Void toggleClusters(System::Void)
		{
			this->hypercubeToggle = !(this->file->togglePaintClusters());
			//this->hypercubeToggle = (!this->hypercubeToggle) ? true : false;
		}

		/**
		* This control toggling whether the user wants the hypercube clusters to use original class colors or not
		*/
		System::Void toggleClassColors(System::Void)
		{
			this->file->togglePaintClassColors();
		}

		/**
		* Gets all clusters
		*/
		std::vector<SetCluster> getClusters(System::Void)
		{
			return this->file->getClusters();
		}

		System::Void autoCluster(System::Void)
		{
			//this->file->autoCluster();
			this->file->highlightOverlap(0.0);
		}

		/**
		* Gets whether the cluster at index is displayed or not
		*/
		bool getDisplayed(int index)
		{
			return this->file->getDisplayed(index);
		}

		double getWorldWidth()
		{
			return this->worldWidth;
		}

		double getWorldHeight()
		{
			return this->worldHeight;
		}

		/**
		 * This creates a hypercube analysis pertaining to a particular cluster of data
		 * @return
		 */
		System::Void hypercube(System::Void)
		{
			if (this->file->isPaintClusters())
			{
				int prev = file->getSelectedClusterIndex();
				this->file->subHypercube(file->getSelectedSetIndex(), file->getSelectedClusterIndex(),/*file->getHypercubeThreshold()*0.01*/file->getRadius());
				this->hypercubeToggle = false;
				//this->file->xorClusters(prev, file->getClusters().size() - 1);
			}
			else
			{
				int selectedSetIndex = this->file->getSelectedSetIndex();
				this->file->hypercube(selectedSetIndex, /*file->getHypercubeThreshold()*0.01*/file->getRadius());
				this->hypercubeToggle = false;
			}


		}

		// Set the toggle for manual sort
		System::Void setManualSortToggle(bool isToggled)
		{
			this->shiftHorizontal = isToggled;
			if (isToggled) {
				this->zoom = 0;
				this->tempXWorldMouseDifference = 0;
				this->tempYWorldMouseDifference = 0;
			}
		}

		// Set the toggle for DNS hide manual.
		System::Void setHideManualToggle(bool isToggled)
		{
			this->hideManual = isToggled;
			if (isToggled) {
				this->zoom = 0;
				this->tempXWorldMouseDifference = 0;
				this->tempYWorldMouseDifference = 0;
			}
		}

		/**
		 * Set the toggle state for manual invert
		 * @param isToggled
		 * @return
		 */
		System::Void setManualInvertToggle(bool isToggled)
		{
			this->invertDimensionToggled = isToggled;
			if (isToggled) {
				this->zoom = 0;
				this->tempXWorldMouseDifference = 0;
				this->tempYWorldMouseDifference = 0;
			}
		}

		/**
		 * Get the toggle for manual invert
		 * @return Get whether or not it is toggled
		 */
		bool getManualInvertToggle(System::Void)
		{
			return this->invertDimensionToggled;
		}

		bool getHideManualToggle(System::Void)
		{
			return this->hideManual;
		}

		/**
		 * Get the toggle for manual sort
		 * @return Get whether or not it is toggled
		 */
		bool getManualSortToggle(System::Void)
		{
			return this->shiftHorizontal;
		}

		/**
		 * Set the toggle for manual shift
		 * @param isToggled
		 * @return
		 */
		System::Void setManualShiftToggle(bool isToggled)
		{
			this->shiftVertical = isToggled;
			if (isToggled) {
				this->zoom = 0;
				this->tempXWorldMouseDifference = 0;
				this->tempYWorldMouseDifference = 0;
			}
		}

		/**
		 * Get the toggle for manual shift
		 * @return Gets whether or not it is toggled
		 */
		bool getManualShiftToggle(System::Void)
		{
			return this->shiftVertical;
		}

		/**
		 * This allows for sort dimensions
		 * @param filePath
		 * @return
		 */
		System::Void save(std::string* filePath)
		{
			this->file->saveToFile(filePath);
		}

		/**
		 * Adds a class
		 * @return
		 */
		System::Void addClass(System::Void)
		{
			this->file->addClass();
		}

		/**
		 * Delete a class
		 * @param selectedIndex
		 * @return
		 */
		System::Void removeClass(int selectedIndex)
		{
			this->file->deleteClass(selectedIndex);
		}

		/**
		 * Sets the class name
		 * @param classIndex
		 * @param newName
		 * @return
		 */
		System::Void setClassName(int classIndex, std::string* newName)
		{
			this->file->setClassName(classIndex, newName);
		}

		/**
		 * Sets the color for the class
		 * @param classIndex
		 * @param newColor
		 * @return
		 */
		System::Void setClassColor(int classIndex, std::vector<double>* newColor)
		{
			this->file->setClassColor(classIndex, newColor);
		}

		/**
		 * Gets the color for the class
		 * @param index
		 * @return Returns a list of colors of RGBA
		 */
		std::vector<double>* getClassColor(int index)
		{
			return this->file->getClassColor(index);
		}

		/**
		 * Give the set a name
		 * @param setIndex
		 * @param newName
		 * @return
		 */
		System::Void setSetName(int setIndex, std::string& newName)
		{
			this->file->setSetName(setIndex, newName);
		}

		/**
		 * Assign a set to a class
		 * @param setIndex
		 * @param newClassIndex
		 * @return
		 */
		System::Void setSetClass(int setIndex, int newClassIndex)
		{
			this->file->setSetClass(setIndex, newClassIndex);
		}

		/**
		 * Gets the class of a set
		 * @param setIndex
		 * @return The index of the desired class for the given set index
		 */
		int getClassOfSet(int setIndex)
		{
			return this->file->getClassOfSet(setIndex);
		}

		/**
		 * Sets the color of a cluster
		 * @param clusterIndex
		 * @param newColor
		 * @return
		 */
		System::Void setClusterColor(int clusterIndex, std::vector<double>* newColor)
		{
			this->file->setClusterColor(clusterIndex, newColor);
		}

		/**
		 * Gets the amount of glusters
		 * @return The amount of hypercube clusters
		 */
		int getClusterAmount(System::Void)
		{
			return this->file->getClusterAmount();
		}

		/**
		 * Deletes a cluster
		 * @param classIndex
		 * @return
		 */
		System::Void deleteCluster(int classIndex)
		{
			this->file->deleteCluster(classIndex);
		}

		/**
		 * Edits the set data at the pass set index and dimension
		 * @param setIndex
		 * @param indexOfDimension
		 * @param newDataValue
		 * @return The raw data for the given parameters
		 */
		double setData(int setIndex, int indexOfDimension, double newDataValue)
		{
			return this->file->setData(setIndex, indexOfDimension, newDataValue);
		}

		/**
		 * Get the dimension amount for settings window
		 * @return The amount of dimensions
		 */
		int getDimensionAmount(System::Void)
		{
			return this->file->getDimensionAmount();
		}

		/**
		 * Sets the dimension name for the settings window
		 * @param dimensionIndex
		 * @param newName
		 * @return
		 */
		System::Void setDimensionName(int dimensionIndex, string* newName) {
			this->file->setDimensionName(dimensionIndex, newName);
		}

		/**
		 * Get the dimension name for settings window
		 * @param dimensionIndex
		 * @return The dimension name
		 */
		std::string* getDimensionName(int dimensionIndex)
		{
			std::string* str = this->file->getDimensionName(dimensionIndex);
			return str;
		}


		/**
		 * sets the bounds to be used for artificial calibration at the passed index(dimensionIndex)
		 * @param dimensionIndex
		 * @param newMaximum
		 * @param newMinimum
		 * @return
		 */
		System::Void setCalibrationBounds(int dimensionIndex, double newMaximum, double newMinimum)
		{
			this->file->setCalibrationBounds(dimensionIndex, newMaximum, newMinimum);
		}

		/**
		 * sets the maximum bound
		 * @param dimensionIndex
		 * @return The artificial maximum bound
		 */
		double getArtificialMaximum(int dimensionIndex)
		{
			return this->file->getArtificialMaximum(dimensionIndex);
		}

		/**
		 * sets the minimum bound
		 * @param dimensionIndex
		 * @return The artificial minimum bound
		 */
		double getArtificialMinimum(int dimensionIndex)
		{
			return this->file->getArtificialMinimum(dimensionIndex);
		}

		/**
		 * get whether callibrated data is set
		 * @param dimensionIndex
		 * @return Whether the data has been artificially callibrated
		 */
		bool isArtificiallyCalibrated(int dimensionIndex) {
			return this->file->isArtificiallyCalibrated(dimensionIndex);
		}

		/**
		 * clears the artificial calibration
		 * @param dimensionIndex
		 * @return
		 */
		System::Void clearArtificialCalibration(int dimensionIndex)
		{
			this->file->clearArtificialCalibration(dimensionIndex);
		}

		/**
		 * ets the original data of the sets and will display in the settings dialog
		 * dimension tab
		 * @param setIndex
		 * @param dimensionIndex
		 * @return The original data
		 */
		double getOriginalData(int setIndex, int dimensionIndex)
		{
			return this->file->getOriginalData(setIndex, dimensionIndex);
		}

		/**
		 * Get the set amount for settings window
		 * @return The amount of sets
		 */
		int getSetAmount(System::Void)
		{
			return this->file->getSetAmount();
		}

		/**
		 * Get the set name for settings window
		 * @param setIndex
		 * @return The set name
		 */
		std::string* getSetName(int setIndex)
		{
			std::string* str = this->file->getSetName(setIndex);
			return str;
		}

		/**
		 * Get the class amount for settings window
		 * @return The class amount
		 */
		int getClassAmount(System::Void)
		{
			return this->file->getClassAmount();
		}

		/**
		 * Get the class name for settings window
		 * @param classIndex
		 * @return The class name
		 */
		std::string* getClassName(int classIndex)
		{
			std::string* str = this->file->getClassName(classIndex);
			return str;
		}

		/**
		 * Checks if we uploaded a file
		 * @return If a file has been opened
		 */
		bool uploadedFile(System::Void)
		{
			return this->uploadFile;
		}

		/**
		 * Sets whether the class has had any changes
		 * @param applied
		 * @return
		 */
		System::Void setAppliedClassChanges(bool applied)
		{
			this->applied = applied;
		}

		System::Void setDisplayed(string name, bool displayed)
		{
			this->file->setDisplayed(name, displayed);
		}

		/**
		 * Checks if we applied any changes
		 * @return If the applied class changed have been made
		 */
		bool appliedClassChanges(System::Void)
		{
			return this->applied;
		}

		/**
		 * Gets the set name for a particular class
		 * @param classIndex
		 * @param setIndex
		 * @return The set name for the class
		 */
		std::string* getSetOfClass(int classIndex, int setIndex)
		{
			std::string* str = this->file->getSetOfClass(classIndex, setIndex);
			return str;
		}

		/**
		 * Sets the background color
		 * @param r Red [0-255]
		 * @param g Green [0-255]
		 * @param b Blue [0-255]
		 * @return
		 */
		System::Void Background(int r, int g, int b)
		{
			GLfloat red = ((GLfloat)r) / 255.0f;
			GLfloat green = ((GLfloat)g) / 255.0f;
			GLfloat blue = ((GLfloat)b) / 255.0f;
			glClearColor(red, green, blue, 0.0f);
		}

		/**
		 * Set the RGB values for the color gradient
		 * @param red
		 * @param green
		 * @param blue
		 * @return
		 */
		System::Void SetRGB(GLdouble red, GLdouble green, GLdouble blue)
		{
			this->R = red;
			this->G = green;
			this->B = blue;
		}

		/**
		 * Checks if text enabled for the graph
		 * @return If the text is enabled
		 */
		bool textOnEnabled(System::Void)
		{
			return this->textEnabled;
		}

		/**
		 * text top
		 * @return Whether the text is on top
		 */
		bool textOnTop(System::Void)
		{
			return this->textTop;
		}

		/**
		 * both text enabled
		 * @return Whether the text is on the bottom
		 */
		bool textOnBottom(System::Void)
		{
			return this->textBottom;
		}

		/**
		 * Enable the text for display whether on or off
		 * @param enable
		 * @return
		 */
		System::Void setTextOnEnabled(bool enable)
		{
			this->textEnabled = enable;
		}

		/**
		 * Sets the text on top
		 * @param enable
		 * @return
		 */
		System::Void setTextOnTop(bool enable)
		{
			this->textTop = enable;
		}

		/**
		 * Sets the text on bottom
		 * @param enable
		 * @return
		 */
		System::Void setTextOnBottom(bool enable)
		{
			this->textBottom = enable;
		}

		/**
		 * Sets the zoom variable [0-200%]
		 * @param i
		 * @return
		 */
		System::Void zoomSet(int i)
		{
			this->zoom = i * 2;
		}

		/**
		 * Checks if a set is visible
		 * @param setIndex
		 * @return Whether or not the set is visible
		 */
		bool isVisible(int setIndex)
		{
			return this->file->isVisible(setIndex);
		}

		/**
		 * Resets the pan and zoom to its default state
		 * @return Whether or not the pan or zoom worked
		 */
		bool resetZoomPan(System::Void)
		{
			this->zoom = 0;
			this->tempXWorldMouseDifference = 0;
			this->tempYWorldMouseDifference = 0;
			return true;
		}

		/**
		 * THIS ALLOWS FOR THE CHILD WINDOW TO BE RESIZED ACCORDINGLY
		 * @param x
		 * @param y
		 * @param width
		 * @param height
		 * @param uFlags
		 * @return
		 */
		System::Void Resize(int x, int y, int width, int height, UINT uFlags)
		{
			this->worldHeight = height;
			this->worldWidth = width;
			SetWindowPos((HWND)this->Handle.ToPointer(), NULL, x, y, width, height, uFlags);
			Reshape((GLsizei)width, (GLsizei)height);
		}

		/**
		 * This renders the OpenGL display for VisCanvas
		 * @return
		 */
		System::Void Render(System::Void)
		{
			this->Display();
		}

		/**
		 * This will swap the buffers for smooth drawing of the OpenGL scene
		 * @return
		 */
		System::Void SwapOpenGLBuffers(System::Void)
		{
			SwapBuffers(m_hDC);
		}

		/**
		 * Allows access to the DataInterface class
		 */
		DataInterface* file; // Create the DataInterface for reading the file

		//Testing Dom Nom Sets.
		void setDomNomSetVisualization(DataInterface* file, double worldHeight, double worldWidth);
		void generateRulesDNS();
		void DNScalculateLinePosition();
		void visualizeDomNomVisualization();
		

	private:

		//Visualization Objects:
		DomNominalSet* domNomVisualization;

		double worldWidth;  // Set the world width
		double worldHeight; // Set the world height

		GLdouble R; // Red
		GLdouble G; // Green
		GLdouble B; // Blue

		int clickedDimension; // Holds the clicked dimension

		double mouseX; // Holds mouse X coord
		double mouseY; // Holds mouse Y coord

		double worldMouseX; // Holds mouse X coord when mapped to world
		double worldMouseY; // Holds mouse Y coord when mapped to world
		double worldMouseYOnClick; // Holds the world mouse Y when clicked
		double worldMouseXOnClick;

		double tempXWorldMouseDifference;
		double tempYWorldMouseDifference;


		double shiftAmount;

		bool hypercubeToggle;
		bool nominalSetsToggle;
		bool uploadFile; // Checks to see if the file has been uploaded
		bool applied; // checks if changes to the class have been applied
		bool drawingDragged; // Is made true via mouselistener when dragging the mouse
		bool invertDimensionToggled;

		bool shiftHorizontal;
		bool shiftVertical;
		bool hideManual;

		int font_list_base_2d; // set the start of the display lists for the 2d font
		const char* font; // sets the font
		int size; // sets the size of the font

		// text for dimensions
		bool textEnabled;
		bool textTop;
		bool textBottom;

		// zoom variabls
		int zoom;

		HDC m_hDC;
		HGLRC m_hglrc;

	protected:
		~COpenGL(System::Void)
		{
			this->DestroyHandle();
		}

		/* DO NOT REMOVE OR MODIFY THIS */
		/* THIS IS USED TO DRAW OPENGL  */
		/* IN WINDOWS FORMS             */
		GLint MySetPixelFormat(HDC hdc)
		{
			PIXELFORMATDESCRIPTOR pfd = {
				sizeof(PIXELFORMATDESCRIPTOR),  //  size of this pfd  
				1,                     // version number  
				PFD_DRAW_TO_WINDOW |   // support window  
				PFD_SUPPORT_OPENGL |   // support OpenGL  
				PFD_DOUBLEBUFFER,      // double buffered  
				PFD_TYPE_RGBA,         // RGBA type  
				24,                    // 24-bit color depth  
				0, 0, 0, 0, 0, 0,      // color bits ignored  
				0,                     // no alpha buffer  
				0,                     // shift bit ignored  
				0,                     // no accumulation buffer  
				0, 0, 0, 0,            // accum bits ignored  
				32,                    // 32-bit z-buffer      
				0,                     // no stencil buffer  
				0,                     // no auxiliary buffer  
				PFD_MAIN_PLANE,        // main layer  
				0,                     // reserved  
				0, 0, 0                // layer masks ignored  
			};


			GLint  iPixelFormat;

			// get the device context's best, available pixel format match 
			if ((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
			{
				MessageBox::Show("ChoosePixelFormat Failed");
				return 0;
			}

			// make that match the device context's current pixel format 
			if (SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE)
			{
				MessageBox::Show("SetPixelFormat Failed");
				return 0;
			}

			if ((m_hglrc = wglCreateContext(m_hDC)) == NULL)
			{
				MessageBox::Show("wglCreateContext Failed");
				return 0;
			}

			if ((wglMakeCurrent(m_hDC, m_hglrc)) == NULL)
			{
				MessageBox::Show("wglMakeCurrent Failed");
				return 0;
			}


			this->glEnableText("Callibri", 17);
			this->glTextBegin();

			return 1;
		}

		// Call this to enable the text such as its font and size
		System::Void glEnableText(const char* font, int size)
		{
			if (size < 0)
			{
				return;
			}
			this->font = font;
			this->size = size;
		}

		// call this to create the handle of the text to be drawn
		System::Void glTextBegin(System::Void)
		{
			HFONT font = CreateFont(this->size, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, 0, 0, 0, 0, (LPCTSTR)this->font); // can only use true type fonts 

			// make the system font the device context's selected font  
			SelectObject(m_hDC, font);

			// create the bitmap display lists  
			// we're making images of glyphs 0 thru 254  
			// the display list numbering starts at 2000, an arbitrary choice  
			wglUseFontBitmaps(m_hDC, 0, 255, this->font_list_base_2d);
		}

		System::Void glTextColor2d(double red, double green, double blue, double alpha)
		{
			glColor4d(red, green, blue, alpha);
		}


		System::Void glText2d(double x, double y, const char* text)
		{
			glRasterPos2d(x, y);
			int length = (int)std::strlen(text);
			glListBase(this->font_list_base_2d);
			glCallLists(length, GL_UNSIGNED_BYTE, text);
		}


		double getWorldMouseX() {

			// this will get the mouse position relative to the child window
			LPPOINT pts = new POINT;
			GetCursorPos(pts);

			::ScreenToClient((HWND)this->Handle.ToPointer(), pts); // gets the mouse coordinate relative to the OpenGL view

			double xWorld = pts->x - ((this->worldWidth) / 2.0); //::Control::MousePosition.X;

			//MessageBox::Show(""+xWorld);

			// divide by panel width to get the porportion of the window
			xWorld /= this->worldWidth;
			// multiply by the world width to parse the porportion of the window into the world
			xWorld *= this->worldWidth;
			delete pts;
			return xWorld;
		} // Converts raw mouse X coordinates to world coordinates

		double getWorldMouseY() {

			// this will get the mouse position relative to the child window
			LPPOINT pts = new POINT;
			GetCursorPos(pts);

			::ScreenToClient((HWND)this->Handle.ToPointer(), pts); // Gets the mouse position relative to the OpenGL view

			double yWorld = pts->y - (this->worldHeight); //::Control::MousePosition.Y;
			// divide by panel height to get the proportion of the window
			yWorld /= this->worldHeight;
			// multiply by the world height to parse the proportion of the window into the world
			yWorld *= this->worldHeight;
			delete pts;
			return yWorld;
		} // Converts raw mouse Y coordinates to world coordinates


		// this method takes the passed mouse click coordinates and finds the dimension clicked on
		int findClickedDimension(double xMouseWorldPosition, double yMouseWorldPosition) {

			//double xAxisIncrement = (this->worldWidth) / (this->file->getDimensionAmount() + 1); // +1 instead of +2

			int numDimensionsHidden = 0;//Keep track of dimensions hidden for offset.
			for (int i = 0; i < file->getDimensionAmount(); i++)
			{
				if (!this->file->isDimensionVisible(i))
				{
					numDimensionsHidden++;
				}
			}

			double xAxisIncrement = (this->worldWidth) / (this->file->getDimensionAmount() + 1 - numDimensionsHidden); // +1 instead of +2

			numDimensionsHidden = 0;

			for (int i = 0; i < file->getDimensionAmount(); i++)
			{
				//As long as the dimension is not hidden:
				if (this->file->isDimensionVisible(i))
				{
					double shiftAmount = this->file->getDimensionShift(i);
					double dimensionX = ((-this->worldWidth) / 2.0) + ((xAxisIncrement) * (i + 1 - numDimensionsHidden));

					double dimensionYMax = (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.75);
					double dimensionYMin = (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.1);

					// creates the bound for the parallel lines
					if (xMouseWorldPosition - (dimensionX) >= -15 && xMouseWorldPosition - (dimensionX) <= 15) {
						return i;
					}
				}
				else numDimensionsHidden++;
				
			}
			return -1;
		}

		GLvoid drawDraggedDimension(double x, int dimensionIndex)
		{

			double xAxisIncrement = this->worldWidth / (this->file->getDimensionAmount() + 1);
			double shiftAmount = this->file->getDimensionShift(dimensionIndex);

			// this will draw the current selected dimension line at half alpha
			glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
			glBegin(GL_LINE_STRIP);
			glVertex2d(x, shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.75);
			glVertex2d(x, shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.1);
			glEnd();

			glLineWidth(3.0);
			glPointSize(3.0);

			// this will draw the current selected dimension text at half alpha
			if (this->textEnabled) {
				glTextColor2d(0.0, 0.0, 0.0, 0.5);
				std::string name = (*this->file->getDimensionName(dimensionIndex));
				std::string temp = "1-";
				temp = temp.append(name);
				if (this->textBottom) {
					if (this->file->isDimensionInverted(dimensionIndex)) {
						glText2d(x - ((temp.length() * 10.0) / 2.0), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.05), temp.c_str());
					}
					else {
						glText2d(x - ((name.length() * 10.0) / 2.0), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.05), name.c_str());
					}
				}
				else {
					if (this->file->isDimensionInverted(dimensionIndex)) {
						glText2d(x - ((temp.length() * 10.0) / 2.0), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.78), temp.c_str());
					}
					else {
						glText2d(x - ((name.length() * 10.0) / 2.0), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.78), name.c_str());
					}
				}
				glEnd();
			}

			for (int j = 0; j < file->getSetAmount(); j++)
			{
				std::vector<double>* colorOfCurrent = this->file->getSetColor(j);

				double colorAlpha = (*colorOfCurrent)[3];
				colorAlpha *= 0.5;
				double currentData = this->file->getData(j, dimensionIndex);
				glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], colorAlpha);
				glBegin(GL_POINTS); // draws points
				glVertex2d(x, currentData * (worldHeight * 0.5) + (0.175 * worldHeight));
				glEnd(); // ends drawing line
			}
		}


		GLvoid Display(GLvoid) {

			//Set the lines that will mark the x values of the window
			glLineWidth(2.0);
			glClear(GL_COLOR_BUFFER_BIT);

			// Enables the transparency using alpha
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_BLEND);

			// This is an example of how to make text using OpenGL
			// Made by Shane Vance

			/*
			glEnableText("Calibri", 50);
			glTextBegin();
			glTextColor2d(0.0, 0.0, 1.0, 1.0);
			glText2d(-(this->worldWidth / 2.0), this->worldHeight / 2.0, "HELLO THIS IS A TEST TO SEE HOW MANY CHARACTERS I CAN PUT ON THE SCREEN!");
			glFlush();
			*/

			if (this->uploadedFile()) {
				int dimensionCount = 0;
				double xAxisIncrement = this->worldWidth / (this->file->getVisibleDimensionCount() + 1); // +1 instead of +2
				glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
				for (int i = 0; i < this->file->getDimensionAmount(); i++)
				{
					if (file->getDataDimensions()->at(i)->isVisible())
					{
						double shiftAmount = this->file->getDimensionShift(i);
						glBegin(GL_LINE_STRIP);
						// was (xAxisIncrement) * (i + 1)
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.75));
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.1));
						glEnd();

						dimensionCount++;
					}

				}
				glFlush();

				dimensionCount = 0;

				glColor4d(125.0 / 255.0, 125.0 / 255.0, 125.0 / 255.0, 155.0 / 255.0);
				for (auto entry : file->getAboveOne())
				{
					dimensionCount = 0;
					glBegin(GL_LINE_STRIP);
					for (int i = 0; i < this->file->getDimensionAmount(); i++)
					{
						if (file->getDataDimensions()->at(i)->isVisible())
						{
							double shiftAmount = this->file->getDimensionShift(i);
							// was (xAxisIncrement) * (i + 1)
							glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), ((entry.second + shiftAmount) * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
							dimensionCount++;
						}
					}
					glEnd();

				}

				dimensionCount = 0;

				// this will display the text for the dimension
				if (this->textEnabled) {
					glTextColor2d(0.0, 0.0, 0.0, 1.0);
					for (int i = 0; i < this->file->getDimensionAmount(); i++) {

						if (i == 0 && !(file->getAboveOne().empty()))
						{
							double shiftAmount = this->file->getDimensionShift(i);
							this->textBottom = false;
							for (auto entry : file->getAboveOne())
							{
								glText2d(((-this->worldWidth - (entry.first.length() * 10.0)) / 1.9) + ((xAxisIncrement) * (dimensionCount + 1)), ((entry.second + shiftAmount) * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight), entry.first.c_str());
							}
						}

						if (file->getDataDimensions()->at(i)->isVisible())
						{
							// display dimension text
							double shiftAmount = this->file->getDimensionShift(i);
							std::string name = (*this->file->getDimensionName(i));
							std::string temp = "1-";
							temp = temp.append(name);
							if (this->textBottom) {
								if (this->file->isDimensionInverted(i)) {
									glText2d(((-this->worldWidth - (temp.length() * 10.0)) / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.05), temp.c_str());
								}
								else {
									glText2d(((-this->worldWidth - (name.length() * 10.0)) / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.05), name.c_str());
								}
							}
							else {
								if (this->file->isDimensionInverted(i)) {
									glText2d(((-this->worldWidth - (temp.length() * 10.0)) / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.78), temp.c_str());
								}
								else {
									glText2d(((-this->worldWidth - (name.length() * 10.0)) / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (shiftAmount * (this->worldHeight * 0.5) + this->worldHeight * 0.78), name.c_str());
								}
							}
							glEnd();

							dimensionCount++;
						}
					}
					glFlush();
				}

				drawData();
				glFlush();

				if (this->drawingDragged && shiftHorizontal) {

					if (this->clickedDimension != -1) {
						this->drawDraggedDimension(this->worldMouseX, this->clickedDimension);
						glFlush();
					}

				}
			}
		}

		// Graphs the data to the world
		GLvoid drawData(GLvoid)
		{
			int dimensionCount = 0;
			glLineWidth(3.0);
			double xAxisIncrement = this->worldWidth / (this->file->getVisibleDimensionCount() + 1);

			if (file->getNominalSetsMode())
			{
				drawNominalSetData(); // Dominant Nominal Sets.
			}
			else if (file->getDomNominalSetsMode())
			{
				drawNominalSetCorrelation();
			}
			else if (file->getOverlapMode())
			{
				vector<SetCluster>* overlaps = this->file->getOverlaps();
				for (int j = 0; j < overlaps->size(); j++)
				{
					SetCluster* currCube = &overlaps->at(j);
					std::vector<double>* colorOfCurrent = currCube->getColor();
					glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], /*0.3*/(*colorOfCurrent)[3]);
					glColor4d(192.0, 192.0, 192.0, 0.01);

					// draw cubes
					for (int k = 0; k < currCube->getSetNumber(); k++)
					{
						if (currCube->isDisplayed())
						{
							int currentIndex = currCube->getSets()->at(k);

							glBegin(GL_LINE_STRIP);
							for (int i = 0; i < this->file->getDimensionAmount(); i++)
							{
								if (this->file->isPaintClassColors())
								{
									std::vector<double>* colorOfCurrent = this->file->getSetColor(currentIndex);
									glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);
								}

								if (this->file->getDataDimensions()->at(i)->isVisible())
								{
									double currentData = this->file->getData(currentIndex, i);
									// Adds empty spot boxes to hypercube if applicable
									if (TRUE){//currentData <= 0.0) {
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 20.0, (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) + 20);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 20.0, (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) + 20);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 20.0, (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) - 20);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 20.0, (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) - 20);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 20.0, (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) + 20);
									}
									else {
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
									}
									dimensionCount++;
								}
							}
							glEnd();
							dimensionCount = 0;
						}
					}
				} // end hyper block loop
				dimensionCount = 0;
			}
			else if (file->getQuadMode())
			{
				drawQuadData();
			}
			else if (file->getFrequencyMode())
			{
				drawFrequencyData();
			}
			else if (file->getHistogramMode())
			{
				drawHistogramData();
			}
			else
			{
				dimensionCount = 0;

				if (this->file->isPaintClusters()) {

					for (int j = 0; j < this->file->getClusterAmount(); j++)
					{
						std::vector<double>* colorOfCurrent = this->file->getClusterColor(j);
						glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], /*0.3*/(*colorOfCurrent)[3]);
						glColor4d(192.0, 192.0, 192.0, 0.01);

						// draw cubes
						for (int k = 0; k < this->file->getClusterSets(j)->size(); k++)
						{
							if (this->file->getDisplayed(j))
							{
								int currentIndex = this->file->getClusterSets(j)->at(k);

								glBegin(GL_LINE_STRIP);
								for (int i = 0; i < this->file->getDimensionAmount(); i++)
								{
									if (this->file->isPaintClassColors())
									{
										std::vector<double>* colorOfCurrent = this->file->getSetColor(currentIndex);
										glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);
									}

									if (this->file->getDataDimensions()->at(i)->isVisible())
									{
										double currentData = this->file->getData(currentIndex, i);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
										dimensionCount++;
									}
								}
								glEnd();
								dimensionCount = 0;
							}
						}
					} // end hyper block loop
					dimensionCount = 0;

					// re-draw selected cluster border on top

					if (this->file->drawBorders() && this->file->getDisplayed(this->file->getSelectedClusterIndex()))
					{
						std::vector<double>* colorOfCurrent = this->file->getClusterColor(this->file->getDisplayed(this->file->getSelectedClusterIndex()));
						glColor4d(192.0, 192.0, 192.0, 1.0);

						glBegin(GL_QUAD_STRIP);
						for (int i = 0; i < this->file->getDimensionAmount(); i++)
						{
							if (this->file->getDataDimensions()->at(i)->isVisible())
							{
								double currentData = this->file->getClusters().at(file->getSelectedClusterIndex()).getMaximum(i);
								if(currentData >= 0)
									glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
								
								currentData = this->file->getClusters().at(file->getSelectedClusterIndex()).getMinimum(i);
								if (currentData < 0) {
									currentData = this->file->getClusters().at(file->getSelectedClusterIndex()).getMinimumPositive(i);
								}
								if(currentData >= 0)
									glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

								dimensionCount++;
							}
						}
						glEnd();
						dimensionCount = 0;
					}


					if (this->file->getDisplayed(this->file->getSelectedClusterIndex()))
					{

						// re-draw selected cluster on top

						std::vector<int> list = *this->file->getClusters().at(this->file->getSelectedClusterIndex()).getSets();
						for (int i = 0; i < list.size(); i++) {
							int currentIndex = list[i];

							std::vector<double>* colorOfCurrent = this->file->getSetColor(currentIndex);
							glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);

							glBegin(GL_LINE_STRIP);
							for (int j = 0; j < this->file->getDimensionAmount(); j++)
							{
								if (this->file->getDataDimensions()->at(j)->isVisible())
								{
									double currentData = this->file->getData(currentIndex, j);
									glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

									dimensionCount++;
								}
							}
							glEnd();
							dimensionCount = 0;
						}

						// redraw selected class on top

						for (int i = 0; i < list.size(); i++) {
							int currentIndex = list[i];

							if (this->file->getSelectedClusterClassIndex() == this->file->getClassOfSet(currentIndex))
							{
								std::vector<double>* colorOfCurrent = this->file->getSetColor(currentIndex);
								glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);


								glBegin(GL_LINE_STRIP);
								for (int j = 0; j < this->file->getDimensionAmount(); j++)
								{
									if (this->file->getDataDimensions()->at(j)->isVisible())
									{
										double currentData = this->file->getData(currentIndex, j);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

										dimensionCount++;
									}
								}
								glEnd();
								dimensionCount = 0;
							}
						} // end if displayed

						// draw min, center, and max lines on top
						if (this->file->getDisplayed(this->file->getSelectedClusterIndex()))
						{
							std::vector<double>* colorOfCurrent = this->file->getClusterColor(this->file->getSelectedClusterIndex());
							glColor4d(0.0, 0.0, 0.0, 1.0);

							if (this->file->drawMaxLine())
							{
								glBegin(GL_LINE_STRIP);
								for (int i = 0; i < this->file->getDimensionAmount(); i++)
								{
									if (this->file->getDataDimensions()->at(i)->isVisible())
									{
										double currentData = this->file->getClusters().at(file->getSelectedClusterIndex()).getMaximum(i);
										// Adds empty spot boxes to hypercube if applicable
										if (currentData < 0.0) {}
										else {
											glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
										}
										dimensionCount++;
									}
								}
								glEnd();
							} // end draw max line 

							if (this->file->drawCenterLine())
							{
								glBegin(GL_LINE_STRIP);
								for (int i = 0; i < this->file->getDimensionAmount(); i++)
								{
									if (this->file->getDataDimensions()->at(i)->isVisible())
									{
										double currentData = this->file->getClusters().at(file->getSelectedClusterIndex()).getVirtualCenter(file->getDimensionAmount()).at(i);
										if(currentData < 0.0){}
										else {
											glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
										}
										dimensionCount++;
									}
								}
								glEnd();
								dimensionCount = 0;
							} // end draw center line

							if (this->file->drawMinLine())
							{
								glBegin(GL_LINE_STRIP);
								for (int i = 0; i < this->file->getDimensionAmount(); i++)
								{
									if (this->file->getDataDimensions()->at(i)->isVisible())
									{
										double currentData = this->file->getClusters().at(file->getSelectedClusterIndex()).getMinimum(i);
										if (currentData < 0.0) {
											currentData = this->file->getClusters().at(file->getSelectedClusterIndex()).getMinimumPositive(i);
											if(currentData > 0)
												glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
										}
										else {
											glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
										}
										dimensionCount++;
									}
								}
								glEnd();
								dimensionCount = 0;
							} // end draw min line

							dimensionCount = 0;
							for (int i = 0; i < this->file->getDimensionAmount(); i++)
							{
								if (this->file->getDataDimensions()->at(i)->isVisible())
								{
									std::vector<double> currentData = this->file->getClusters().at(file->getSelectedClusterIndex()).getEmptySpots(i);
									for (int i = 0; i < currentData.size(); i++) {
										glColor4d(192.0, 192.0, 192.0, 1.0);
										glBegin(GL_POLYGON);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) + 15);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) + 15);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) - 15);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) - 15);
										glEnd();
										glColor4d(0.0, 0.0, 0.0, 1.0);
										glBegin(GL_LINE_STRIP);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) + 15);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) + 15);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) - 15);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) - 15);
										glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10.0, (currentData[i] * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight) + 15);
										glEnd();
									}
									dimensionCount++;
								}
							}
							dimensionCount = 0;
						}
					}


				}
				else {

					// draw classes

					double red = 255.0;
					double blue = 0.0;
					double green = 0.0;
					int state = 0;

					double step = (255.0 * 6.0) / file->getSetAmount();

					for (int j = 0; j < this->file->getSetAmount(); j++)
					{

						std::vector<double>* colorOfCurrent = this->file->getSetColor(j);
						glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);

						//glColor4d(red / 255.0, green / 255.0, blue / 255.0, 1.0);

						glBegin(GL_LINE_STRIP); // begins drawing lines
						for (int i = 0; i < this->file->getDimensionAmount(); i++)
						{
							if (this->file->getDataDimensions()->at(i)->isVisible())
							{
								double currentData = this->file->getData(j, i);
								glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

								dimensionCount++;
							}
						}
						glEnd(); // ends drawing line
						dimensionCount = 0;


					}
				}

				// re-draw selected class on top of other classes
				if (this->hypercubeToggle) {
					std::vector<int>* list = this->file->getSetsInClass(this->file->getSelectedClassIndex());
					for (int i = 0; i < list->size(); i++) {
						int currentIndex = (*list)[i];
						// set color to the color of the set currentIndex
						// paint set
						std::vector<double>* colorOfCurrent = this->file->getSetColor(currentIndex);
						glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);

						glBegin(GL_LINE_STRIP);
						for (int j = 0; j < this->file->getDimensionAmount(); j++)
						{
							if (this->file->getDataDimensions()->at(j)->isVisible())
							{
								double currentData = this->file->getData(currentIndex, j);
								glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

								dimensionCount++;
							}
						}
						glEnd();
						dimensionCount = 0;
					}
				}

				// draw selector line
				if (!file->selectorLineIsHidden())
				{
					int selectedSetIndex = file->getSelectedSetIndex();
					std::vector<double>* colorOfCurrent = this->file->getSetColor(selectedSetIndex);
					glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);
					glBegin(GL_LINE_STRIP); // begins drawing lines
					for (int i = 0; i < this->file->getDimensionAmount(); i++)
					{
						if (this->file->getDataDimensions()->at(i)->isVisible())
						{
							double currentData = this->file->getData(selectedSetIndex, i);
							glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)), (currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

							dimensionCount++;
						}
					}
					glEnd(); // ends drawing line
					dimensionCount = 0;
				}


			}// end if histogram else

		} // end drawData()

		//NOMINAL SETS WITH CORRELATION:
		GLvoid drawNominalSetCorrelation(GLvoid)
		{
			//DomNominalSet visualization = DomNominalSet(file, worldHeight, worldWidth);
			this->visualizeDomNomVisualization();
		}//end Dom Nominal Sets.

		//===Draw Nominal Set Data===
		//Desc: Draws nominal set bars for eacha attribute.
		GLvoid drawNominalSetData(GLvoid)
		{

			int dimensionCount = 0; // Variable for the dimension index.
			int colorChoice = file->getNominalColor();
			glLineWidth(3.0); //Seting line width.
			double xAxisIncrement = this->worldWidth / (this->file->getVisibleDimensionCount() + 1); //Get calculated x axis spacing between lines.

			//Create a vector of unordered maps to hold the attributes:
			vector<unordered_map<double, double>*>* blockHeights = new vector<unordered_map<double, double>*>();

			//Fill the vector with unordered maps:
			for (int i = 0; i < this->file->getDimensionAmount(); i++)
			{
				blockHeights->push_back(new unordered_map<double, double>());
			}

			//Go over every row (set).
			for (int j = 0; j < this->file->getSetAmount(); j++)
			{
				//Go over every attribute of the row.
				for (int i = 0; i < this->file->getDimensionAmount(); i++)
				{
					//Get the current value at the attribute(i) for this row(j).
					double currentData = this->file->getData(j, i);

					//Get current unordered map:
					unordered_map<double, double>* curMap = blockHeights->at(i);

					//Check to see if current data is already in the unordered map.
					if (curMap->find(currentData) == blockHeights->at(i)->end())
					{
						//If not insert it.
						curMap->insert({ currentData, 1 });
					}
					else
					{
						//Increment by the curent data if it is already present.
						std::unordered_map<double, double>::iterator it = curMap->find(currentData);
						it->second++;
					}
				}
			}

			//At this point, there is a vector containing maps with the frequencies of the values.

			//====Get percentages for data and Normalazie data between 1 and 0:====

			//Iterate over vector:
			for (int i = 0; i < this->file->getDimensionAmount(); i++)
			{

				double allValues = 0; //Recording all values to section block heights.

				//Get current unordered map:
				unordered_map<double, double>* curMap = blockHeights->at(i);

				//Iterate over unordered map to find valus.
				for (std::unordered_map<double, double>::iterator iter = curMap->begin(); iter != curMap->end(); ++iter)
				{
					double freq = iter->second;
					allValues += freq;
				}

				//Apply the normilized values.
				for (std::unordered_map<double, double>::iterator iter = curMap->begin(); iter != curMap->end(); ++iter)
				{
					iter->second = (iter->second / allValues);
				}
			}

			//====Sort the kv pairs:====
			vector<pair<double, double>> sortVec;//Vector for sorting.
			vector<vector<pair<double, double>>> sortedVector;//Vector to hold sorted values.

			//Go over every attribute of the row.
			for (int i = 0; i < this->file->getDimensionAmount(); i++)
			{
				//Get current unordered map:
				unordered_map<double, double>* curMap = blockHeights->at(i);
				vector<pair<double, double>> toInsert;

				unordered_map<double, double>::iterator it;

				//add all kv to a vector:
				for (it = curMap->begin(); it != curMap->end(); it++)
				{
					sortVec.push_back(make_pair(it->second, it->first));
				}

				//Sort the vector.
				sort(sortVec.begin(), sortVec.end());

				//Insert sorted values into a new vector.
				for (int k = sortVec.size() - 1; k >= 0; k--)
				{
					toInsert.push_back({ sortVec[k].second, sortVec[k].first });
				}

				//Insert sorted sub vector into vector.
				sortedVector.push_back(toInsert);

				sortVec.clear();
			}

			//====Draw the rectangles====.

			const int HEIGHT_OF_ALL_BLOCKS = 435;

			//Go over every attribute.
			for (int i = 0; i < this->file->getDimensionAmount(); i++)
			{

				//Get current vector (attribute we are working with making blocks):
				vector<pair<double, double>> curVec = sortedVector[i];

				//Values to calculate where to put values.
				double blockOffsetVertical = 80;
				double prevHeight = 80;

				double colorIncR = 0;
				double colorIncG = 0;
				double colorIncB = 0;

				//Starting colors:
				if (colorChoice == 1)
				{
					colorIncR = 220;
					colorIncG = 0;
					colorIncB = 0;
				}
				else if (colorChoice == 2)
				{
					colorIncR = 0;
					colorIncG = 220;
					colorIncB = 0;
				}
				else if (colorChoice == 3)
				{
					colorIncR = 0;
					colorIncG = 0;
					colorIncB = 220;
				}
				else if (colorChoice == 4)
				{
					colorIncR = 50;
					colorIncG = 50;
					colorIncB = 50;
				}
				else
				{
					colorIncR = 220;
					colorIncG = 0;
					colorIncB = 0;
				}

				int state = 0;
				const int INC_DEC = 80; //Variable for color Increment / Decrement.

				//Iterate over vector to find valus.
				for (int j = 0; j < curVec.size(); j++)
				{
					//Get key and frequency and draw a rectangle.
					double key = curVec[j].first;
					double freq = curVec[j].second;

					//Draw the rectangle.
					glBegin(GL_QUADS);

					//Set Color:
					glColor4f(colorIncR / 255, colorIncG / 255, colorIncB / 255, 0.55);

					// draw bottom left
					glVertex2d(
						((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						(blockOffsetVertical)
					);

					// draw top left
					glVertex2d(
						((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
					);

					// draw top right
					glVertex2d(
						((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical))
					);

					// draw bottom right
					glVertex2d(
						((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
						(blockOffsetVertical)
					);

					glEnd();

					//Check if we need to draw the border.
					if (!(freq <= 0.014))
					{

						//==Draw border==:
						glBegin(GL_LINE_STRIP);

						glColor3f(0, 0, 0);
						glLineWidth(.5);

						//Top Left Point:
						glVertex2d(
							((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - 10),
							((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
						);

						glVertex2d(
							((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + 10),
							((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)-2)
						);

						//Top Right Point:
						glEnd();
					}

					//Mutate vector to draw lines in next step.
					curVec[j].second = (((freq / 2) * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical));
					sortedVector[i] = curVec;

					//Record the previous height.
					blockOffsetVertical += (((freq * HEIGHT_OF_ALL_BLOCKS) + (blockOffsetVertical)) - prevHeight);
					prevHeight = blockOffsetVertical;

					if (colorChoice == 1) //RED SCHEME
					{
						if (state == 0)
						{
							colorIncR += INC_DEC;

							//IF we get to the max of blue, start to decriment red. (255, 255, 0)
							if (colorIncR >= 255)
							{
								colorIncR = 255;
								state = 1;
							}

						}
						else if (state == 1)
						{
							colorIncG += INC_DEC;

							//IF we get to the max of blue, start to decriment red. (255, 255, 0)
							if (colorIncG >= 255)
							{
								colorIncG = 0;
								colorIncR = 220;
								state = 0;
							}
						}
					}
					else if (colorChoice == 2) //GREEN SCHEME
					{
						if (state == 0)
						{
							colorIncG += INC_DEC;

							//IF we get to the max of blue, start to decriment red. (255, 255, 0)
							if (colorIncG >= 255)
							{
								colorIncG = 255;
								state = 1;
							}

						}
						else if (state == 1)
						{
							colorIncB += INC_DEC;

							//IF we get to the max of blue, start to decriment red. (255, 255, 0)
							if (colorIncB >= 255)
							{
								colorIncB = 0;
								colorIncG = 220;
								state = 0;
							}
						}
					}
					else if (colorChoice == 3) //BLUE SCHEME
					{
						if (state == 0)
						{
							colorIncB += INC_DEC;

							//IF we get to the max of blue, start to decriment red. (255, 255, 0)
							if (colorIncB >= 255)
							{
								colorIncB = 255;
								state = 1;
							}

						}
						else if (state == 1)
						{
							colorIncG += INC_DEC;

							//IF we get to the max of blue, start to decriment red. (255, 255, 0)
							if (colorIncG >= 255)
							{
								colorIncG = 0;
								colorIncB = 220;
								state = 0;
							}
						}
					}
					else if (colorChoice == 4) //BLUE SCHEME
					{
						if (state == 0)
						{
							colorIncR += 20;
							colorIncG += 20;
							colorIncB += 20;


							//IF we get to the max of blue, start to decriment red. (255, 255, 0)
							if (colorIncR >= 255)
							{
								colorIncR = 0;
								colorIncG = 0;
								colorIncB = 0;
							}

						}
					}
					else //ALL SCHEME
					{
						//Change color.
						if (state == 0)
						{
							colorIncB += INC_DEC;

							//IF we get to the max of blue, start to decriment red. (255, 255, 0)
							if (colorIncB >= 255)
							{
								colorIncB = 255;
								state = 1;
							}

						}
						else if (state == 1)
						{
							colorIncR -= INC_DEC;

							//IF we get to the min of red, start incrementing green. (0, 255, 0)
							if (colorIncR <= 0)
							{
								colorIncR = 0;
								state = 2;
							}

						}
						else if (state == 2)
						{
							colorIncG += INC_DEC;

							//If we get to the max of green, start decrementing blue. (0, 255, 255)
							if (colorIncG >= 255)
							{
								colorIncG = 255;
								state = 3;
							}

						}
						else if (state == 3)
						{
							colorIncB -= INC_DEC;

							//If we get to the min of blue, start incrementing red. (0, 0, 255)
							if (colorIncB <= 255)
							{
								colorIncB = 0;
								state = 4;
							}
						}
						else if (state == 4)
						{
							colorIncR += INC_DEC;

							//If we get to the max of red, decrement green. (255, 0, 255)
							if (colorIncR >= 255)
							{
								colorIncR = 220;
								state = 5;
							}
						}
						else
						{
							colorIncG -= INC_DEC;

							//If we get to the min of green, restart with inc red. (255, 0, 0)
							if (colorIncG <= 0)
							{
								colorIncG = 0;
								state = 0;
							}
						}
					}

				}

				dimensionCount++;

			}

			//Reset dimension count.
			dimensionCount = 0;

			//Now, all of the unordered maps have the value and what position the top of the block stops at.


			//====Draw lines to show data:=====

			//Iterate over rows in data.
			for (int j = 0; j < this->file->getSetAmount(); j++)
			{

				//Get the colors for each class line.
				std::vector<double>* colorOfCurrent = this->file->getSetColor(j);
				glColor4d((*colorOfCurrent)[0], (*colorOfCurrent)[1], (*colorOfCurrent)[2], (*colorOfCurrent)[3]);

				//Iterate over attribute values for the row.
				glBegin(GL_LINE_STRIP); // begins drawing lines
				for (int i = 0; i < this->file->getDimensionAmount(); i++)
				{
					//Get current vecotor.
					vector<pair<double, double>> curVec = sortedVector[i];

					//Values to calculate where to put point for line.
					double blockOffsetVertical = 0;

					if (this->file->getDataDimensions()->at(i)->isVisible())
					{
						double currentData = this->file->getData(j, i);
						double valueOfCurrent;

						//find value in curVec iterating over kv pairs:
						for (int k = 0; k < curVec.size(); k++)
						{
							pair<double, double> p = curVec.at(k);

							if (p.first == currentData)
							{
								valueOfCurrent = p.second;
								break;
							}
						}

						double calcOfYCord = (((valueOfCurrent / 2) * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						glVertex2d(
							((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1))), //X - val is correct since its just going off the attribute line.
							(valueOfCurrent)
						);

						dimensionCount++;
					}
				}
				glEnd();

				dimensionCount = 0;
			}

		}

		GLvoid drawQuadData(GLvoid) {

			vector<SetCluster> blocks = file->getClusters();
			int dimensionCount = 0;
			glLineWidth(3.0);
			double xAxisIncrement = this->worldWidth / (this->file->getVisibleDimensionCount() + 1);
			glLineWidth(3.0);
			glColor4d(0.0, 0.0, 0.0, 0.1);

			vector<vector<int>> pairs = vector<vector<int>>();

			// find the set of all pairs
			for (int i = 0; i < file->getClusterAmount(); i++)
			{
				for (int j = 0; j < file->getClusterAmount(); j++)
				{
					if (i == j) continue;
					if (file->getClassOfSet(file->getClusters()[i].getSets()->at(0)) == file->getClassOfSet(file->getClusters()[j].getSets()->at(0))) continue;

					bool alreadyExists = false;
					for (int k = 0; k < pairs.size(); k++)
					{
						if ((pairs[k][0] == i && pairs[k][1] == j) ||
							(pairs[k][0] == j && pairs[k][1] == i))
						{
							alreadyExists = true;
						}
					}

					if (alreadyExists) continue;

					vector<int> pair = { i , j };
					pairs.push_back(pair);
				}
			}

			glColor4d(0.0, 0.0, 0.0, 1.0);//(1.0 / pairs.size()));

			// draw all non-overlapping dimensions for each pair
			for (int i = 0; i < pairs.size(); i++)
			{
				dimensionCount = 0;
				int firstIdx = pairs[i][0];
				int secondIdx = pairs[i][1];

				for (int j = 0; j < file->getDimensionAmount(); j++)
				{
					// if blocks overlapping, then draw openql quads
					if (blocks[firstIdx].getMaximum(j) <= blocks[secondIdx].getMinimum(j) ||
						blocks[firstIdx].getMinimum(j) >= blocks[secondIdx].getMaximum(j))
					{
						double floor = max(blocks[secondIdx].getMinimum(j), blocks[firstIdx].getMinimum(j));
						double ceil = min(blocks[secondIdx].getMaximum(j), blocks[firstIdx].getMaximum(j));

						///// draw first block /////
						glColor4d(0.0, 0.0, 0.0, (1.0 / 5));

						glBegin(GL_QUADS);

						// draw bottom left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - (xAxisIncrement / 4),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - (xAxisIncrement / 4),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + (xAxisIncrement / 4),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw bottom right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + (xAxisIncrement / 4),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));


						glEnd();

					}
					dimensionCount++;

				} // end dimension loop


			} // end pairs loop

		}

		GLvoid drawHistogramData(GLvoid) {
			int split = 10;
			double quantileSize = DBL_MAX;
			int dimensionCount = 0;
			glLineWidth(3.0);
			double xAxisIncrement = this->worldWidth / (this->file->getVisibleDimensionCount() + 1);
			glColor4d(0.0, 1.0, 0.0, 1.0);

			if (file->isPaintClusters()) // hyper blocks
			{
				SetCluster currCluster = file->getClusters()[file->getSelectedClusterIndex()];

				vector<int> quantileCount = vector<int>();
				for (int i = 0; i < split; i++) quantileCount.push_back(0);


				// find the smallest quantile
				for (int i = 0; i < file->getDimensionAmount(); i++)
				{
					double max = currCluster.getMaximum(i);
					double min = currCluster.getMinimum(i);
					double span = max - min;
					if (max != min)
						quantileSize = min(quantileSize, (span / (split * 1.0)));
				}

				for (int i = 0; i < file->getDimensionAmount(); i++)
				{
					if (!file->getDataDimensions()->at(i)->isVisible())
						continue;

					double max = currCluster.getMaximum(i);
					double min = currCluster.getMinimum(i);
					double span = max - min;

					split = (int)round(span / quantileSize) + 1;

					for (int k = 1; k <= split; k++)
					{
						double ceil = (quantileSize * k) + min;
						double floor = (quantileSize * (k - 1)) + min;
						int count = 0;

						for (int j = 0; j < currCluster.getSetNumber(); j++)
						{
							if (file->getData(currCluster.getSets()->at(j), i) <= ceil &&
								file->getData(currCluster.getSets()->at(j), i) >= floor)
							{
								count++;
							}
						}

						if (count == 1) count = 2;

						glColor4d(0.0, 255.0, 0.0, 1.0);
						glBegin(GL_QUADS);

						// draw bottom left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / currCluster.getSetNumber()) * count),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw bottom right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / currCluster.getSetNumber()) * count),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						glEnd();

						glColor4d(0.0, 0.0, 0.0, 1.0);
						glBegin(GL_LINE_STRIP);

						// draw top left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / currCluster.getSetNumber()) * count),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw bottom right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / currCluster.getSetNumber()) * count),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw bottom left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						glEnd();
					} // end k loop

					dimensionCount++;
				} // end dimension loop
			}
			else // all data
			{
				vector<int> quantileCount = vector<int>();
				for (int i = 0; i < split; i++) quantileCount.push_back(0);

				for (int i = 0; i < file->getDimensionAmount(); i++)
				{
					if (!file->getDataDimensions()->at(i)->isVisible())
						continue;

					for (int k = 1; k <= split; k++)
					{
						double ceil = (1.0 / split) * k;
						double floor = (1.0 / split) * (k - 1);
						int count = 0;

						for (int j = 0; j < file->getDataDimensions()->at(i)->size(); j++)
						{
							if (file->getDataDimensions()->at(i)->getData(j) <= ceil &&
								file->getDataDimensions()->at(i)->getData(j) >= floor)
							{
								count++;
							}
						}

						if (count == 1) count = 2;

						glColor4d(0.0, 255.0, 0.0, 1.0);
						glBegin(GL_QUADS);

						// draw bottom left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / file->getSetAmount()) * count),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw bottom right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / file->getSetAmount()) * count),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						glEnd();

						glColor4d(0.0, 0.0, 0.0, 1.0);
						glBegin(GL_LINE_STRIP);

						// draw top left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / file->getSetAmount()) * count),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw bottom right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / file->getSetAmount()) * count),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw bottom left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						glEnd();
					} // end k loop

					dimensionCount++;
				} // end dimension loop
			} // end else
		} // end drawHistogramMode

		GLvoid drawHistogramDataSimpleLine(GLvoid) {
			int dimensionCount = 0;
			glLineWidth(3.0);
			double xAxisIncrement = this->worldWidth / (this->file->getVisibleDimensionCount() + 1);

			if (file->isPaintClusters()) // hyper blocks
			{
				glColor4d(1.0, 0.0, 0.0, 1.0);
				int currentBlockIdx = file->getSelectedClusterIndex();
				int amountOfDimensions = file->getDimensionAmount();
				vector<SetCluster>  blocks = file->getClusters();
				vector<int>* points = blocks.at(currentBlockIdx).getSets();

				for (int i = 0; i < amountOfDimensions; i++)
				{
					if (this->file->getDataDimensions()->at(i)->isVisible())
					{
						vector<double> theData = vector<double>();

						for (int j = 0; j < points->size(); j++)
						{
							theData.push_back(file->getData(points->at(j), i));
						}

						std::sort(theData.begin(), theData.end());

						double previousData = theData.front();

						for (int j = 0; j < theData.size(); j++)
						{
							double currentData = theData.at(j);

							glBegin(GL_LINE_STRIP);

							// draw first point of line
							glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
								(currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

							// find end of the bar in the histogram
							int count = 0;
							for (int k = j; j < theData.size() && theData.at(j) == theData.at(k); j++)
							{
								count++;
							}

							// draw last point of line
							glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / points->size()) * count),
								(currentData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

							glEnd();

							if (j >= theData.size()) j--;

						}
						dimensionCount++;
					} // end visible check

				}// end dimension loop
				dimensionCount = 0;
			}
			else // all data
			{

			}
		}

		GLvoid drawFrequencyData(GLvoid) {

			vector<double> leftCoordinate = vector<double>();
			vector<double> rightCoordinate = vector<double>();
			vector<double> frequency = vector<double>();
			vector<int> colorIdx = vector<int>();
			bool alreadyExists = false;

			int dimensionCount = 0;
			double xAxisIncrement = worldWidth / (file->getVisibleDimensionCount() + 1);

			glColor4d(0.0, 0.0, 0.0, 1.0);

			if (file->isPaintClusters()) // hyper blocks
			{

				dimensionCount = 0;
				///// draw distribution shape on top of background /////
				dimensionCount = 0;
				int split = 3;
				double quantileSize = DBL_MAX;
				SetCluster currCluster = file->getClusters()[file->getSelectedClusterIndex()];

				// find the smallest quantile
				for (int i = 0; i < file->getDimensionAmount(); i++)
				{
					double max = currCluster.getMaximum(i);
					double min = currCluster.getMinimum(i);
					double span = max - min;
					if (max != min)
						quantileSize = min(quantileSize, (span / (split * 1.0)));
				}

				vector<int> quantileCount = vector<int>();
				for (int i = 0; i < split; i++) quantileCount.push_back(0);

				for (int i = 0; i < file->getDimensionAmount(); i++)
				{
					if (!file->getDataDimensions()->at(i)->isVisible())
						continue;

					double max = currCluster.getMaximum(i);
					double min = currCluster.getMinimum(i);
					double span = max - min;

					split = (int)round(span / quantileSize) + 1;

					for (int k = 1; k <= split; k++)
					{
						double ceil = (quantileSize * k) + min;
						double floor = (quantileSize * (k - 1)) + min;
						int count = 0;

						for (int j = 0; j < currCluster.getSetNumber(); j++)
						{
							if (file->getData(currCluster.getSets()->at(j), i) <= ceil &&
								file->getData(currCluster.getSets()->at(j), i) >= floor)
							{
								count++;
							}
						}

						if (count == 1) count = 2;

						glColor4d(255.0 / 255.0, 255.0 / 255.0, 255.0 / 255.0, 1.0);
						glBegin(GL_QUADS);

						// draw bottom left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - ((xAxisIncrement / currCluster.getSetNumber()) * (count / 2)),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - ((xAxisIncrement / currCluster.getSetNumber()) * (count / 2)),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / currCluster.getSetNumber()) * (count / 2)),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw bottom right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / currCluster.getSetNumber()) * (count / 2)),
							(floor * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						glEnd();

						// draw line to seperate quantiles

						glColor4d(0.0 / 255.0, 0.0 / 255.0, 0.0 / 255.0, 1.0);
						glBegin(GL_LINE_STRIP);

						// draw top left
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) - ((xAxisIncrement / currCluster.getSetNumber()) * (count / 2)),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						// draw top right
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)) + ((xAxisIncrement / currCluster.getSetNumber()) * (count / 2)),
							(ceil * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

						glEnd();
					} // end k loop

					dimensionCount++;
				} // end dimension loop

				///// draw selected cluster on top  /////

				dimensionCount = 0;

				// create array of coordinates and counts
				for (int i = 0; i < file->getDimensionAmount() - 1; i++)
				{
					if (!(file->getDataDimensions()->at(i)->isVisible())) continue;
					if (file->getVisibleDimensionCount() < 2) break;

					leftCoordinate.clear();
					rightCoordinate.clear();
					frequency.clear();
					colorIdx.clear();
					alreadyExists = false;

					int j = i + 1;
					while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
					if (j >= file->getDimensionAmount()) continue;

					for (int k = 0; k < currCluster.getSetNumber(); k++)
					{
						double left = file->getData(currCluster.getSets()->at(k), i);
						double right = file->getData(currCluster.getSets()->at(k), j);

						// see if exists already, if so increment count;else add and count = 1
						alreadyExists = false;
						for (int l = 0; l < frequency.size(); l++)
						{
							if (leftCoordinate[l] == left &&
								rightCoordinate[l] == right)
							{
								alreadyExists = true;
								frequency[l] = frequency[l] + 1.0;
								break; // break out of l loop
							}
						} // end l loop

						if (!alreadyExists)
						{
							leftCoordinate.push_back(left);
							rightCoordinate.push_back(right);
							frequency.push_back(1.0);
							colorIdx.push_back(k);
						}

					} // end k loop


					// draw the data between these two dimensions, scaling width to count
					for (int k = 0; k < frequency.size(); k++)
					{
						vector<double>* color = file->getSetColor(currCluster.getSets()->at(colorIdx[k]));
						double alpha = min(1.0, (frequency[k] / (currCluster.getSetNumber() / 8)));
						// double alpha = 1.0;
						glColor4d((*color)[0], (*color)[1], (*color)[2], alpha);
						// glColor4d(0.0, 1.0, 0.0, alpha);
						double width = 1.0 + (2.0 * min(2.0, (frequency[k] / (currCluster.getSetNumber() / 8))));
						// double width = 3.0;
						glLineWidth(width);
						double leftData = leftCoordinate[k];
						double rightData = rightCoordinate[k];

						// draw a line using both points
						glBegin(GL_LINE_STRIP);
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(leftData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 2)),
							(rightData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
						glEnd();
					}
					dimensionCount++;

				} // end i loop for drawing selected cluster on top

				dimensionCount = 0;

				///// draw mean line on top /////

				if (this->file->drawCenterLine())
				{
					vector<int>* currSets = currCluster.getSets();
					glLineWidth(3.0);
					glColor4d(0.0, 0.0, 0.0, 1.0);
					glBegin(GL_LINE_STRIP);
					for (int i = 0; i < this->file->getDimensionAmount(); i++)
					{
						if (this->file->getDataDimensions()->at(i)->isVisible())
						{
							// get average for this dimensions
							double avgData = 0.0;
							double total = 0.0;
							int amount = currCluster.getSetNumber();

							for (int j = 0; j < amount; j++)
							{
								total += file->getData(currSets->at(j), i);
							}

							avgData = total / amount;

							glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
								(avgData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));

							dimensionCount++;
						}
					}
					glEnd();
					dimensionCount = 0;
				} // end draw center line

			}
			else //  all data
			{
				// create array of coordinates and counts
				for (int i = 0; i < file->getDimensionAmount() - 1; i++)
				{
					if (!(file->getDataDimensions()->at(i)->isVisible())) continue;
					if (file->getVisibleDimensionCount() < 2) break;

					leftCoordinate.clear();
					rightCoordinate.clear();
					frequency.clear();
					colorIdx.clear();
					alreadyExists = false;

					int j = i + 1;
					while (j < file->getDimensionAmount() && !(file->getDataDimensions()->at(j)->isVisible())) j++;
					if (j >= file->getDimensionAmount()) continue;

					for (int k = 0; k < file->getSetAmount(); k++)
					{
						double left = file->getData(k, i);
						double right = file->getData(k, j);

						// see if exists already, if so increment count;else add and count = 1
						alreadyExists = false;
						for (int l = 0; l < frequency.size(); l++)
						{
							if (leftCoordinate[l] == left &&
								rightCoordinate[l] == right)
							{
								alreadyExists = true;
								frequency[l] = frequency[l] + 1.0;
								break; // break out of l loop
							}
						} // end l loop

						if (!alreadyExists)
						{
							leftCoordinate.push_back(left);
							rightCoordinate.push_back(right);
							frequency.push_back(1.0);
							colorIdx.push_back(k);
						}

					} // end k loop


					// draw the data between these two dimensions, scaling width to count
					for (int k = 0; k < frequency.size(); k++)
					{
						vector<double>* color = file->getSetColor(colorIdx[k]);
						double alpha = min(1.0, (frequency[k] / (file->getSetAmount() / 8)));
						// double alpha = 1.0;
						glColor4d((*color)[0], (*color)[1], (*color)[2], alpha);
						double width = 1.0 + (2.0 * min(2.0, (frequency[k] / (file->getSetAmount() / 8))));
						// double width = 3.0;
						glLineWidth(width);
						double leftData = leftCoordinate[k];
						double rightData = rightCoordinate[k];

						// draw a line using both points
						glBegin(GL_LINE_STRIP);
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 1)),
							(leftData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
						glVertex2d((-this->worldWidth / 2.0) + ((xAxisIncrement) * (dimensionCount + 2)),
							(rightData * (this->worldHeight * 0.5)) + (0.175 * this->worldHeight));
						glEnd();
					}
					dimensionCount++;

				} // end i loop

			} // end else all data
		} // end draw frequency

		// INITIALIZE THE GL SETTINGS
		GLvoid Init(GLvoid)
		{
			this->Background(194, 206, 218);	        // background is blue-ish gray
		}

		// RESIZE AND INITIALIZE THE GL WINDOW
		GLvoid Reshape(GLsizei width, GLsizei height)
		{
			// compute aspect ratio of the new window
			if (height == 0)
			{
				height = 1; // To prevent divide by 0
			}

			// set the viewport to cover the new window
			glViewport(0, 0, width, height);

			// set the orthosphere and keep center of the screen
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D((((GLdouble)-width + this->tempXWorldMouseDifference) + this->zoom) / 2.0, (((GLdouble)width + this->tempXWorldMouseDifference) - this->zoom) / 2.0, ((((GLdouble)-height / 2.5) + this->zoom) - this->tempYWorldMouseDifference), ((((GLdouble)height - this->zoom) + 100) - this->tempYWorldMouseDifference));
		}

		// THIS IS WHERE ANY BUTTON CLICKS GO // the parent window will need to handle the other key presses
		virtual void WndProc(Message% msg) override
		{
			switch (msg.Msg)
			{

			case WM_LBUTTONDOWN:
			{
				// get the X and Y coordinates of the mouse position
				this->worldMouseX = this->getWorldMouseX();
				this->worldMouseY = this->getWorldMouseY();

				this->worldMouseYOnClick = this->getWorldMouseY();
				this->worldMouseXOnClick = this->getWorldMouseX();

				// ensures that the clicked dimension is valid
				this->clickedDimension = this->findClickedDimension(this->worldMouseX, this->worldMouseY); //1					

				if (this->invertDimensionToggled)
				{
					this->file->invertDimension(this->clickedDimension);
				}

				//If the DNS Hide Coordinates is selected:
				if (this->file->getDNSHideCoordinatesMode())
				{
					this->file->addDimensionToHideDNS(this->clickedDimension);
				}

				double shiftAmount = this->file->getDimensionShift(clickedDimension);
				this->drawingDragged = true;
			}
			break;

			case WM_MOUSEMOVE:
			{

				// get the X and Y coordinates of the mouse position
				this->worldMouseX = this->getWorldMouseX();
				this->worldMouseY = this->getWorldMouseY();

				// get the dropped dimension
				int droppedDimension = this->findClickedDimension(this->worldMouseX, this->worldMouseY);

				//If DNS active:
				if (file->getDomNominalSetsMode() && droppedDimension != -1)
				{
					file->setDimensionHover(droppedDimension);
				}

				// update by swapping while passing over dimension
				if (this->drawingDragged && this->shiftHorizontal && this->file->moveData(this->clickedDimension, droppedDimension)) {
					this->clickedDimension = droppedDimension;
				}
				else if (this->drawingDragged && this->shiftVertical) {
					this->file->setDimensionShift(this->clickedDimension, (this->shiftAmount + (this->worldMouseYOnClick - this->worldMouseY) / (this->worldHeight * 0.65)));
				}
				else if (this->drawingDragged && !this->shiftVertical && !this->shiftHorizontal && !this->invertDimensionToggled) {
					this->tempXWorldMouseDifference = worldMouseXOnClick - this->getWorldMouseX();
					this->tempYWorldMouseDifference = worldMouseYOnClick - this->getWorldMouseY();
				}

			}
			break;
			case WM_LBUTTONUP:
			{

				if (this->drawingDragged) {
					// get the X and Y coordinates of the mouse position
					/*this->worldMouseX = this->getWorldMouseX();
					this->worldMouseY = this->getWorldMouseY();
					int droppedDimension = this->findClickedDimension(this->worldMouseX, this->worldMouseY);
					this->file->moveData(this->clickedDimension, droppedDimension);*/
					shiftAmount = 0.0;
					this->drawingDragged = false;
				}

			}
			break;
			}


			NativeWindow::WndProc(msg);

		}
	};
}




// Zooming call reshape 
