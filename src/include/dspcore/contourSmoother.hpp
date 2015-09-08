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


/*  openSMILE component: contour smoother

smooth data contours by moving average filter

*/



#ifndef __CCONTOURSMOOTHER_HPP
#define __CCONTOURSMOOTHER_HPP

#include <core/smileCommon.hpp>
#include <core/windowProcessor.hpp>

#define COMPONENT_DESCRIPTION_CCONTOURSMOOTHER "This component smooths data contours by applying a moving average filter of configurable length."
#define COMPONENT_NAME_CCONTOURSMOOTHER "cContourSmoother"

#undef class
class DLLEXPORT cContourSmoother : public cWindowProcessor {
  private:
    int smaWin;
    int noZeroSma;

  protected:
    SMILECOMPONENT_STATIC_DECL_PR


    virtual void fetchConfig();

    //virtual int configureWriter(const sDmLevelConfig *c);
    //virtual int setupNamesForField(int i, const char*name, long nEl);

    // buffer must include all (# order) past samples
    virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post );
    
    
  public:
    SMILECOMPONENT_STATIC_DECL
    
    cContourSmoother(const char *_name);

    virtual ~cContourSmoother();
};


#endif // __CCONTOURSMOOTHER_HPP
