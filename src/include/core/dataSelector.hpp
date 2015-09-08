/*F***************************************************************************
 * openSMILE - the open-Source Multimedia Interpretation by Large-scale
 * feature Extraction toolkit
 * 
 * (c) 2008-2011, Florian Eyben, Martin Woellmer, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2012-2013, Florian Eyben, Felix Weninger, Bjoern Schuller: TUM-MMK
 * 
 * (c) 2013-2014 audEERING UG, haftungsbeschränkt. All rights reserved.
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

example for dataProcessor descendant

*/


#ifndef __CDATASELECTOR_HPP
#define __CDATASELECTOR_HPP

#include <core/smileCommon.hpp>
#include <core/dataProcessor.hpp>

#define COMPONENT_DESCRIPTION_CDATASELECTOR "This component copies data from one level to another, thereby selecting frame fields and elements by their element/field name."
#define COMPONENT_NAME_CDATASELECTOR "cDataSelector"

typedef struct {
  long eIdx; // element index

// actually these two are not needed... ??
  long fIdx; // field index
  long aIdx; // array index, if field is an array, else 0
  long N;
} sDataSelectorSelData;

#undef class
class DLLEXPORT cDataSelector : public cDataProcessor {
  private:
    int elementMode, selectionIsRange, dummyMode;
    const char *outputSingleField;
    cVector *vecO;

    const char * selFile;
    int nSel; // number of names in names array
    char **names;
    
    int nElSel, nFSel;  // actual number of elements(!) or fields selected (size of mapping array)
    sDataSelectorSelData *mapping; // actual data selection map
    long *idxSelected;

    // for loading files...
    int fselType;

    int loadSelection();

  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    virtual int setupNewNames(long nEl);
    //virtual int configureWriter(const sDmLevelConfig *c);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cDataSelector(const char *_name);

    virtual ~cDataSelector();
};




#endif // __CDATASELECTOR_HPP
