/*F***************************************************************************
 * 
 * openSMILE - the Munich open source Multimedia Interpretation by 
 * Large-scale Extraction toolkit
 * 
 * This file is part of openSMILE.
 * 
 * openSMILE is copyright (c) by audEERING GmbH. All rights reserved.
 * 
 * See file "COPYING" for details on usage rights and licensing terms.
 * By using, copying, editing, compiling, modifying, reading, etc. this
 * file, you agree to the licensing terms in the file COPYING.
 * If you do not agree to the licensing terms,
 * you must immediately destroy all copies of this file.
 * 
 * THIS SOFTWARE COMES "AS IS", WITH NO WARRANTIES. THIS MEANS NO EXPRESS,
 * IMPLIED OR STATUTORY WARRANTY, INCLUDING WITHOUT LIMITATION, WARRANTIES OF
 * MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE, ANY WARRANTY AGAINST
 * INTERFERENCE WITH YOUR ENJOYMENT OF THE SOFTWARE OR ANY WARRANTY OF TITLE
 * OR NON-INFRINGEMENT. THERE IS NO WARRANTY THAT THIS SOFTWARE WILL FULFILL
 * ANY OF YOUR PARTICULAR PURPOSES OR NEEDS. ALSO, YOU MUST PASS THIS
 * DISCLAIMER ON WHENEVER YOU DISTRIBUTE THE SOFTWARE OR DERIVATIVE WORKS.
 * NEITHER TUM NOR ANY CONTRIBUTOR TO THE SOFTWARE WILL BE LIABLE FOR ANY
 * DAMAGES RELATED TO THE SOFTWARE OR THIS LICENSE AGREEMENT, INCLUDING
 * DIRECT, INDIRECT, SPECIAL, CONSEQUENTIAL OR INCIDENTAL DAMAGES, TO THE
 * MAXIMUM EXTENT THE LAW PERMITS, NO MATTER WHAT LEGAL THEORY IT IS BASED ON.
 * ALSO, YOU MUST PASS THIS LIMITATION OF LIABILITY ON WHENEVER YOU DISTRIBUTE
 * THE SOFTWARE OR DERIVATIVE WORKS.
 * 
 * Main authors: Florian Eyben, Felix Weninger, 
 * 	      Martin Woellmer, Bjoern Schuller
 * 
 * Copyright (c) 2008-2013, 
 *   Institute for Human-Machine Communication,
 *   Technische Universitaet Muenchen, Germany
 * 
 * Copyright (c) 2013-2015, 
 *   audEERING UG (haftungsbeschraenkt),
 *   Gilching, Germany
 * 
 * Copyright (c) 2016,	 
 *   audEERING GmbH,
 *   Gilching Germany
 ***************************************************************************E*/


/*  openSMILE component:

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#ifndef __CSIMPLEVISUALISERGUI_HPP
#define __CSIMPLEVISUALISERGUI_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>
#include <wx/wx.h>

#define COMPONENT_DESCRIPTION_CSIMPLEVISUALISERGUI "This is an example of a cDataSink descendant. It reads data from the data memory and prints it to the console. This component is intended as a template for developers."
#define COMPONENT_NAME_CSIMPLEVISUALISERGUI "cSimpleVisualiserGUI"

#undef class

class paintDataEvent {
public:
  paintDataEvent(int _nvals=1) : val(NULL) {
    nVals = _nvals;
    if (_nvals > 0) {
      val = new int[_nvals];
    }
  }
  ~paintDataEvent() { if (val != NULL) delete[] val;  }

  int action; // type of plot
  int matMultiplier;
  int nVals;
  int *val;
  const char **colours;
  const char **names;
  FLOAT_DMEM scale0;
  FLOAT_DMEM offset0;
};



#include <wx/sizer.h>
 
class wxRtplotPanel : public wxPanel
    {
        //wxBitmap image;
        
        
    public:
        paintDataEvent *paintEventCur;
        paintDataEvent *paintEventLast;

        bool showImg;
        smileMutex wxMtx;
        //int curval, lastval;
        int curT, lastT;
        //const  char * featureName;
        //FLOAT_DMEM yscale100;

        wxRtplotPanel(wxFrame* parent);
        
        void paintEvent(wxPaintEvent & evt);
        void paintNow();
        
        void render(wxDC& dc);
        void renderMoving2dPlot(wxDC& dc);
        void renderMovingMatPlot(wxDC& dc);
        
        DECLARE_EVENT_TABLE()
    };
 
 
  


// the ID we'll use to identify our event
const int PLOT_UPDATE_ID = 100001;

class visFrame: public wxFrame
{
  //wxBitmap bmp;
  wxRtplotPanel * drawPane;   
  wxCheckBox *m_cb;

public:
    //FLOAT_DMEM yscale100;
    smileMutex wxMtx;

    visFrame(const wxString& title, const wxPoint& pos, const wxSize& size, smileMutex _wxMtx);

    void OnQuit(wxCommandEvent& event);
/*    void OnAbout(wxCommandEvent& event);*/

    // this is called when the event from the thread is received
    void onPlotUpdate(wxCommandEvent& evt)
    {
      //int val = evt.GetInt();

      // update the plot data (cyclic buffer)

      smileMutexLock(wxMtx);
      // replot
      drawPane->paintEventCur = (paintDataEvent *)evt.GetClientData();
      smileMutexUnlock(wxMtx);

/*
      drawPane->yscale100 = yscale100;
      drawPane->featureName = evt.GetString();
      drawPane->curval = val;
      */
      drawPane->showImg = true;
      drawPane->paintNow();

      
    }


    

    DECLARE_EVENT_TABLE()
};


class visApp: public wxApp
{
  virtual bool OnInit();
public:
  //FLOAT_DMEM yscale100;
  smileMutex wxMtx;

  visFrame * vframe;
  
};

#define VIS_ACT_MOVING2DPLOT 1
#define VIS_ACT_MOVINGMATPLOT 2

class cSimpleVisualiserGUI : public cDataSink {
  private:
    int action; int fullinput;
    int nInputs;
    int *inputsIdx;
    FLOAT_DMEM *inputScale;
    FLOAT_DMEM *inputOffset;

    const char **inputNames;
    const char **inputColours;
    
    //wxCommandEvent &myevent; //( wxEVT_COMMAND_TEXT_UPDATED, VAD_UPDATE_ID );
    //int old;

    smileThread guiThr;
    //gcroot<openSmilePluginGUI::TestForm^> fo;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    int getElidxFromName(const char *_name); 

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

  public:
    SMILECOMPONENT_STATIC_DECL
    //FLOAT_DMEM yscale100;

    int matMultiplier;
    int terminate;
    visApp* pApp;
    smileMutex wxMtx;

    cSimpleVisualiserGUI(const char *_name);

    virtual ~cSimpleVisualiserGUI();
};




#endif // __CSIMPLEVISUALISERGUI
