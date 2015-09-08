/*F***************************************************************************
 * openSMILE - the Munich open source Multimedia Interpretation by Large-scale
 * Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller @ TUM-MMK
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller @ TUM-MMK (c)
 * 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
 * 
 * Any form of commercial use and redistribution is prohibited, unless another
 * agreement between you and audEERING exists. See the file LICENSE.txt in the
 * top level source directory for details on your usage rights, copying, and
 * licensing conditions.
 * 
 * See the file CREDITS in the top level directory for information on authors
 * and contributors. 
 ***************************************************************************E*/


/*  openSMILE component:

example dataSink:
reads data from data memory and outputs it to console/logfile (via smileLogger)
this component is also useful for debugging

*/


#ifndef __CEXAMPLEGUIPLUGINSINK_HPP
#define __CEXAMPLEGUIPLUGINSINK_HPP

#include <smileCommon.hpp>
#include <dataSink.hpp>
#include <wx/wx.h>

#define COMPONENT_DESCRIPTION_CEXAMPLEGUIPLUGINSINK "This is an example of a cDataSink descendant. It reads data from the data memory and prints it to the console. This component is intended as a template for developers."
#define COMPONENT_NAME_CEXAMPLEGUIPLUGINSINK "cExampleGuipluginSink"

#undef class


#include <wx/sizer.h>
 
class wxImagePanel : public wxPanel
    {
        wxBitmap image;
        
        
    public:
        bool showImg;

        wxImagePanel(wxFrame* parent, wxString file, wxBitmapType format);
        
        void paintEvent(wxPaintEvent & evt);
        void paintNow();
        
        void render(wxDC& dc);
        
        DECLARE_EVENT_TABLE()
    };
 
 
 
 

// the ID we'll use to identify our event
const int VAD_UPDATE_ID = 100000;

class MyFrame: public wxFrame
{
  //wxBitmap bmp;
  wxImagePanel * drawPane;   
  wxCheckBox *m_cb;

public:
    
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

/*    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);*/

    // this is called when the event from the thread is received
    void onVadUpdate(wxCommandEvent& evt)
    {
      // get the number sent along the event and use it to update the GUI
      if (evt.GetInt()==1) {
        //m_cb->SetValue( true );
        drawPane->showImg = true;
        drawPane->paintNow();
      } else {
        //m_cb->SetValue( false );
        drawPane->showImg = false;
        drawPane->paintNow();
      }
    }


    

    DECLARE_EVENT_TABLE()
};


class MyApp: public wxApp
{
  virtual bool OnInit();
public:
  MyFrame * mframe;
  
};


class cExampleGuipluginSink : public cDataSink {
  private:
    const char *filename;
    FILE * fHandle;
    int lag;
    //wxCommandEvent &myevent; //( wxEVT_COMMAND_TEXT_UPDATED, VAD_UPDATE_ID );
    int old;

    smileThread guiThr;
    //gcroot<openSmilePluginGUI::TestForm^> fo;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    MyApp* pApp;
    cExampleGuipluginSink(const char *_name);

    virtual ~cExampleGuipluginSink();
};




#endif // __CEXAMPLEGUIPLUGINSINK_HPP
