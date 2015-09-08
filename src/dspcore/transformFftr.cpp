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

fast fourier transform using fft4g library
output: complex values of fft or real signal values (for iFFT)

*/


#include <dspcore/transformFftr.hpp>

#define MODULE "cTransformFFTr"

SMILECOMPONENT_STATICS(cTransformFFTr)

SMILECOMPONENT_REGCOMP(cTransformFFTr)
{
  SMILECOMPONENT_REGCOMP_INIT

  scname = COMPONENT_NAME_CTRANSFORMFFTR;
  sdescription = COMPONENT_DESCRIPTION_CTRANSFORMFFTR;

  // we inherit cVectorProcessor configType and extend it:
  SMILECOMPONENT_INHERIT_CONFIGTYPE("cVectorProcessor")

  SMILECOMPONENT_IFNOTREGAGAIN(
    ct->setField("inverse", "1 = perform inverse FFT", 0);
  )
  SMILECOMPONENT_MAKEINFO(cTransformFFTr);
}

SMILECOMPONENT_CREATE(cTransformFFTr)

//-----

cTransformFFTr::cTransformFFTr(const char *_name) :
  cVectorProcessor(_name),
  fftwork_(NULL),
  newFsSet_(0),
  frameSizeSec_out_(0.0) {}

void cTransformFFTr::fetchConfig()
{
  cVectorProcessor::fetchConfig();
  
  inverse_ = getInt("inverse");
  if (inverse_) {
    SMILE_IDBG(2, "transformFFT set for inverse FFT");
  }
}

int cTransformFFTr::configureWriter(sDmLevelConfig &c)
{
  // determine new frameSizeSec resulting from rounding up to closest power of 2
  int i;
  for (i=0; i<c.Nf; i++) {
    long nEl = c.fmeta->field[i].N;

    /* for each field we must ensure the power of 2 constraint and adjust the frameSize if necessary*/
    if (!smileMath_isPowerOf2(nEl)) {
      if (inverse_ == 1) {
        SMILE_IERR(1,"cannot perform zero-padding for inverse real FFT (this would mean zero padding frequencies in the complex domain...)! A framesize which is power of 2 is required here! (current framesize = %i)",nEl);
        COMP_ERR("aborting");
        
      } else {
        long nElOld = nEl;
        nEl = smileMath_ceilToNextPowOf2(nEl);  // TODO:: change frameSizeSec in write Level!
        if (!newFsSet_) {
          // compute new frame size in seconds:
          c.lastFrameSizeSec = c.frameSizeSec; // save last frame size
          c.frameSizeSec *= (double)nEl / (double)nElOld;
          newFsSet_ = 1;
        }
      }
    }
    if (inverse_ == 1) {
       //TODO: detect frames which were originally zero-padded, and output truncated frames

    }
    if (newFsSet_) break;
  }

  frameSizeSec_out_ = c.frameSizeSec;
  return 1;
}

// generate "frequency axis information", i.e. the frequency in Hz for each spectral bin
// which is to be saved as meta-data in the dataMemory level field (FrameMetaInfo->FieldMetaInfo->info)
// &infosize is initialized with the number of fft bins x 2 (= number of input samples)
//   and should contain the number of complex bins at the end of this function
void * cTransformFFTr::generateSpectralVectorInfo(long &infosize)
{
  int i;
  infosize /= 2;
  infosize++; // nyquist and DC...
  double *inf = (double*)calloc(1,sizeof(double)*infosize);
  
  double F0;
  if (frameSizeSec_out_ > 0.0) {
    F0 = (double)(1.0) / (double)frameSizeSec_out_;
    for (i=0; i<infosize; i++) {
      inf[i] = F0*(double)i;
    }
  }

  return (void *)inf;
}

int cTransformFFTr::setupNamesForField(int i, const char*name, long nEl)
{
  // round of nEl to closest power of 2, setup output fields (and thus, Ndst passed to processVectorXXX, etc.)

  /* for each field we must ensure the power of 2 constraint and adjust the frameSize if necessary*/
  if (!smileMath_isPowerOf2(nEl)) {
    if (inverse_ == 1) { COMP_ERR("error with input framesize, not a power of 2!"); }
    long nElOld = nEl;
    nEl = smileMath_ceilToNextPowOf2(nEl);  
  }
  if (nEl < 4) nEl = 4;

  //TODO? : append name "fftc" -> set the nameAppend field : ct->setField("nameAppend",(const char*) NULL,"fftc");
  int ret = cVectorProcessor::setupNamesForField(i,name,nEl);
  long tmp = nEl;
  void *fmap = generateSpectralVectorInfo(tmp);
  writer_->setFieldInfo(-1,DATATYPE_SPECTRUM_BINS_COMPLEX,fmap,tmp*sizeof(double));
  return ret;
}

int cTransformFFTr::myFinaliseInstance()
{
  int ret = cVectorProcessor::myFinaliseInstance();
  
  if (ret) {
    if (fftwork_ != NULL) { 
      multiConfFree(fftwork_); 
      fftwork_ = NULL; 
    }
    fftwork_ = (sFftrData**)multiConfAlloc(); 
  }
  return ret;
}

// a derived class should override this method, in order to implement the actual processing
int cTransformFFTr::processVectorFloat(const FLOAT_DMEM *src, FLOAT_DMEM *dst, long Nsrc, long Ndst, int idxi) // idxi=input field index
{
  int i;
  FLOAT_TYPE_FFT *x;

  idxi = getFconf(idxi);
  sFftrData *data = fftwork_[idxi];
  
  x = (FLOAT_TYPE_FFT*)malloc(sizeof(FLOAT_TYPE_FFT) * Ndst);
  for (i = 0; i < Nsrc; i++) {
    x[i] = (FLOAT_TYPE_FFT)src[i];
  }

  if (inverse_ == 0) { 
    for (i = Nsrc; i < Ndst; i++) {  // zeropadding
      x[i] = 0.0;
    }
  } else {
// TODO!  x and y loaded with symmetric complex data
  } 

  //perform FFT
  if (inverse_ == 0) {
    fftr(x, &data, Ndst);
  } else {
// TODO!  x and y loaded with symmetric complex data
//    ifftr(x, inverse, x, _ip, _w);
  }

  if (inverse_ == 1) {
    //FLOAT_DMEM norm = (FLOAT_DMEM)2.0/((FLOAT_DMEM)Ndst+1);
    //for (i=0; i<Ndst; i++) {
    //  dst[i] = ((FLOAT_DMEM)x[i])*norm;
    //}
  } else {
    // real part
    dst[0] = (FLOAT_DMEM)x[0];
    for (i=1; i<Ndst/2; i++) {
      dst[i*2] = (FLOAT_DMEM)x[i];
    }
    // imaginary part
    dst[1] = (FLOAT_DMEM)x[Ndst/2];  // nyquist
    for (i=1; i<Ndst/2; i++) {
      dst[i*2+1] = (FLOAT_DMEM)data->y_[i];
    }
  }
  free(x);

  fftwork_[idxi] = data;
  return 1;
}

cTransformFFTr::~cTransformFFTr()
{
  if (fftwork_ != NULL) {
    for (int i = 0; i < getNf(); ++i) {
      if (fftwork_[i] != NULL) delete fftwork_[i];
    }
    free(fftwork_);
  }
}

