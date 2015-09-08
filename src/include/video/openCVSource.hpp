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

OpenCVSource
Captures frames from either webcam or file stream and extracts a square region containing the face(s).
Author: Florian Gross

*/


#ifndef __OPENCV_SOURCE_HPP
#define __OPENCV_SOURCE_HPP


#include <core/smileCommon.hpp>
#include <core/dataSource.hpp>

#ifdef HAVE_OPENCV

#define BUILD_COMPONENT_OpenCVSource

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"

#include <video/openCV_LBPHistogram.hpp>
#include <video/openCV_HSVHistogram.hpp>
#include <video/openCV_OpticalFlow.hpp>

#include <iostream>
#include <stdio.h>

using namespace cv;

class ExtractedFace {
public:
	bool faceFound, eyesFound;
	cv::Rect faceRect;
	cv::Point leftEye, rightEye;
	cv::Mat faceGray, faceColor;
};

#define COMPONENT_DESCRIPTION_COPENCVSOURCE "Captures frames from either webcam or file stream, extracts a square region containing the face and creates a LBP, HSV and optical flow histogram."
#define COMPONENT_NAME_COPENCVSOURCE "cOpenCVSource"

#undef class
class DLLEXPORT cOpenCVSource : public cDataSource {
  private:
    int cfgDisplay; // Display video output
	String cfgVideoSource; // WEBCAM or FILE
	String cfgFilename;
	String cfgFace_cascade_name;
	String cfgEyes_cascade_name;

	int cfgExtractFace;
	int cfgExtractHSVHist;
	int cfgExtractLBPHist;
	int cfgExtractOpticalFlow;
	
	int cfgIncludeFaceFeatures;
	
	int cfgIgnoreInvalid;
	int cfgFaceWidth;
	int cfgLBPUniformPatterns;
	
	int cfgHueBins, cfgSatBins, cfgValBins;
	int cfgFlowBins;
	int cfgNormalizeHistograms;
	FLOAT_DMEM cfgMaxFlow;
	FLOAT_DMEM cfgFlowDownsample;
	int cfgUseLBPC;
	int cfgLBPCRadius, cfgLBPCPoints;

	double cfgFps;

	cv::Mat mCurrentFrame_bgr, mCurrentFrame_gray, mPrevFrame_gray_resized, mCurrentFrameDisplay_bgr;
	VideoCapture mVideoCapture;
	CascadeClassifier mFace_cascade;
	CascadeClassifier mEyes_cascade;

	int mVectorSize;

	std::map<unsigned char, unsigned char> mIdentityMap;
	std::map<unsigned char, unsigned char> mUniformMap;
	int mLBPSize;

	double avgVal[7], avgNum[7];
    
  protected:
    SMILECOMPONENT_STATIC_DECL_PR
    
    virtual void fetchConfig();
    virtual int myConfigureInstance();
    virtual int myFinaliseInstance();
    virtual int myTick(long long t);


    virtual int configureWriter(sDmLevelConfig &c);
    virtual int setupNewNames(long nEl);
    
	virtual ExtractedFace extractFace(cv::Mat& frame, cv::Mat& frame_gray);

  public:
    SMILECOMPONENT_STATIC_DECL
    
    cOpenCVSource(const char *_name);

    virtual ~cOpenCVSource();
};


#endif // HAVE_OPENCV

#endif // __LBPHIST_SOURCE_HPP
