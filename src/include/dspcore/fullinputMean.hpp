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

computes mean of full input ?

*/


#ifndef __CFULLINPUTMEAN_HPP
#define __CFULLINPUTMEAN_HPP

#include <core/smileCommon.hpp>
#include <core/vectorProcessor.hpp>

#define COMPONENT_DESCRIPTION_CFULLINPUTMEAN "This component performs mean normalizing on a data series. A 2-pass analysis of the data is performed, which makes this component unusable for on-line analysis. In the first pass, no output is produced and the mean value (over time) is computed for each input element. In the second pass the mean vector is subtracted from all input frames, and the result is written to the output dataMemory level. Attention: Due to the 2-pass processing the input level must be large enough to hold the whole data sequence."
#define COMPONENT_NAME_CFULLINPUTMEAN "cFullinputMean"

enum cFullinputMean_meanType {
  MEANTYPE_AMEAN = 0,
  MEANTYPE_RQMEAN = 1,
  MEANTYPE_ABSMEAN = 2,
  MEANTYPE_ENORM = 3   // htk compatible energy normalisation
};

#undef class
class DLLEXPORT cFullinputMean : public cDataProcessor {
  private:
    int print_means_;
    int first_frame_;
    long reader_pointer_, reader_pointer2_;
    int multi_loop_mode_;
    enum cFullinputMean_meanType mean_type_;
    int spec_enorm_;
    int symm_subtract_;
    int symm_subtract_clip_to_zero_;
    int flag_; //, htkcompatible; long idx0;
    int eoi_flag_;  // this is set after an EOI condition
    cVector *means_, *means2_;
    long n_means_, n_means2_;


  protected:
    SMILECOMPONENT_STATIC_DECL_PR

    int readNewData();
    void meanSubtract(cVector *vec);
    int doMeanSubtract();
    int finaliseMeans();

    virtual void fetchConfig();
    //virtual int myConfigureInstance();
    //virtual int myFinaliseInstance();
    virtual int myTick(long long t);

    //virtual int configureWriter(sDmLevelConfig &c);

    //virtual void configureField(int idxi, long __N, long nOut);
    //virtual int setupNamesForField(int i, const char*name, long nEl);
    //virtual int processVectorInt(const INT_DMEM *src, INT_DMEM *dst, long Nsrc, long Ndst, int idxi);
    //virtual int processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cFullinputMean(const char *_name);

    virtual ~cFullinputMean();
};




#endif // __CFULLINPUTMEAN_HPP
