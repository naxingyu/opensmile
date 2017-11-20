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


#ifndef __CCLASSIFIERRESULTGUI_HPP
#define __CCLASSIFIERRESULTGUI_HPP

#include <smileCommon.hpp>
#include <smileComponent.hpp>
#include <wx/wx.h>

#define COMPONENT_DESCRIPTION_CCLASSIFIERRESULTGUI "This is an example of a cDataSink descendant. It reads data from the data memory and prints it to the console. This component is intended as a template for developers."
#define COMPONENT_NAME_CCLASSIFIERRESULTGUI "cClassifierResultGUI"

#undef class


#include <wx/sizer.h>
 
class wxMultiImagePanel : public wxPanel
    {
      const char **clsName;
      int classActive;
      int nClasses;
      wxBitmap *image;
      float *confidence;

      int nRegression;
      const char ** regressionText;
      float * regressionValue;

    public:
        wxMultiImagePanel(wxFrame* parent,  int _nClasses=0, int nRegr=0);
        
        void loadClassImage(int clsIdx, const char *clsname, wxString file, wxBitmapType format);
        void setRegressionText(int nr, const char *text) {
          if (regressionText != NULL && nr>=0 && nr < nRegression) {
            regressionText[nr] = text;
          }
        }

        void paintEvent(wxPaintEvent & evt);
        void paintNow();
        
        void renderGauge(wxDC&  dc, int x, int y, int w, int h, int val, const char * label, const char *col="blue");
        void render(wxDC& dc);
        
        void setClassResult(int nr) {
          if (nr >= 0 && nr < nClasses) {
            classActive = nr;
          }
        }
        void setClassConfidence(int idx, float conf) {
          if (confidence != NULL && idx >= 0 && idx < nClasses) {
            confidence[idx] = conf;
          }
        }
        void setRegressionVal(int nr, float val) {
          if (regressionValue != NULL && nr >= 0 && nr < nRegression) {
            regressionValue[nr] = val; 
          }
        }

          ~wxMultiImagePanel() {
            if (regressionText != NULL) free(regressionText);
            if (clsName != NULL) free(clsName);
            if (regressionValue != NULL) delete[] regressionValue;
            if (confidence != NULL) delete[] confidence;
            if (image != NULL) delete[] image;
          }

        DECLARE_EVENT_TABLE()
    };
 
 
class classificationEvent {
public:
  classificationEvent(int _nClasses=0, int _maxClsIdx=0, const char *_classname=NULL, int nRegr=0) : 
      confidences(NULL), nClasses(_nClasses), maxClsIdx(_maxClsIdx), classname(_classname), nRegression(nRegr), regUpdate(-1)
      {
        if (nClasses > 0) { confidences = new float[nClasses]; }
        if (nRegression > 0) { regressionValue = new float[nRegression]; }
      }
      ~classificationEvent() { 
        if (confidences != NULL) delete[] confidences;
        if (regressionValue != NULL) delete[] regressionValue;
      }

  const char *classname;
  int maxClsIdx;
  int nClasses;
  int nRegression;
  int regUpdate; // the index to update, set to -1 to update all
  float *confidences;
  float *regressionValue;
  
};
 

// the ID we'll use to identify our event
const int RESULT_UPDATE_ID = 100002;

class ClsResultFrame: public wxFrame
{
  //wxBitmap bmp;
    
  wxCheckBox *m_cb;

public:
    wxMultiImagePanel * drawPane; 

    ClsResultFrame(const wxString& title, const wxPoint& pos, const wxSize& size, int nCls, int nReg);

/*    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);*/

    // this is called when the event from the thread is received
    void onResultUpdate(wxCommandEvent& evt)
    {
      //smileMutexLock(wxMtx);
      // replot
      classificationEvent *ce = (classificationEvent *)evt.GetClientData();
      if (ce->nClasses > 0) {
        drawPane->setClassResult(ce->maxClsIdx);
      } else {
      if (ce->regUpdate >= 0) {
            printf("event: clsidx = %i  val=%f\n",ce->regUpdate,ce->regressionValue[ce->regUpdate]);

        drawPane->setRegressionVal(ce->regUpdate, ce->regressionValue[ce->regUpdate]);
      } else {
        int i;
        for (i=0; i<ce->nRegression; i++) {
          drawPane->setRegressionVal(i, ce->regressionValue[i]);
        }
      }
      }

      int i;
      for (i=0; i<ce->nClasses; i++) {
        drawPane->setClassConfidence(i,ce->confidences[i]);
      }

      drawPane->paintNow();
      //smileMutexUnlock(wxMtx);
    }

    DECLARE_EVENT_TABLE()
};

class cClassifierResultGUI;

class ClsResultApp: public wxApp
{
  virtual bool OnInit();
public:
  cClassifierResultGUI * smileObj;
  ClsResultFrame * mframe;
  
};


class cClassifierResultGUI : public cSmileComponent {
  private:
    smileThread guiThr;
    
    //gcroot<openSmilePluginGUI::TestForm^> fo;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    int getClassIndex(const char *name);
    int getRegressionIndex(const char *name);

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);
    virtual int processComponentMessage( cComponentMessage *_msg );

  public:
    SMILECOMPONENT_STATIC_DECL
    
    int nClasses;
    int nRegression;
    const char **className;
    const char **imageFile;
    const char **regressionName;

    smileMutex wxMtx;
    ClsResultApp* pApp;
    cClassifierResultGUI(const char *_name);

    virtual ~cClassifierResultGUI();
};




#endif // __CCLASSIFIERRESULTGUI_HPP
