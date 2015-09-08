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


/*  openSMILE component: delta coefficients

compute delta regression using regression formula

*/



#ifndef __CDELTAREGRESSION_HPP
#define __CDELTAREGRESSION_HPP

#include <core/smileCommon.hpp>
#include <core/windowProcessor.hpp>

#define COMPONENT_DESCRIPTION_CDELTAREGRESSION "This component computes delta regression coefficients using the regression equation from the HTK book."
#define COMPONENT_NAME_CDELTAREGRESSION "cDeltaRegression"

#undef class
class DLLEXPORT cDeltaRegression : public cWindowProcessor {
private:
  int halfWaveRect, absOutput;
  int deltawin;
  FLOAT_DMEM norm;

  int zeroSegBound;
  int onlyInSegments;

protected:
  SMILECOMPONENT_STATIC_DECL_PR


  int isNoValue(FLOAT_DMEM x) {
    if (onlyInSegments && x==0.0) return 1;
    if (isnan(x)) return 1;
    return 0;
  }

  virtual void fetchConfig();

  //virtual int configureWriter(const sDmLevelConfig *c);
  //virtual int setupNamesForField(int i, const char*name, long nEl);

  // buffer must include all (# order) past samples
  virtual int processBuffer(cMatrix *_in, cMatrix *_out, int _pre, int _post );


public:
  SMILECOMPONENT_STATIC_DECL

    cDeltaRegression(const char *_name);

  virtual ~cDeltaRegression();
};


#endif // __CDELTAREGRESSION_HPP
