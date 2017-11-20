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

/*
openCV_LBPHistogram
Various helper functions for computing the LBP representation and histogram of an image.
Author: Florian Gross

*/

#ifndef OPENCV_HSVHISTOGRAM_HPP_
#define OPENCV_HSVHISTOGRAM_HPP_

#ifdef HAVE_OPENCV

#include <core/smileCommon.hpp>
#include <core/dataSource.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

namespace HSVHistogram {

/**
 * @function euclideanDistance
 * @brief Computes the euclidean distance of two points ( \f$ d = \sqrt{(x_2-x_1)^2+(y_2-y_1)^2}\f$ ).
 */
inline float euclideanDistance(cv::Point& p1, cv::Point& p2)
{
  float dx = p1.x - p2.x;
  float dy = p1.y - p2.y;
  return std::sqrt(dx * dx + dy * dy);
};

/**
 * @function computeSingleHistogram
 * @brief Computes a single histogram for a given single channel image (cv::Mat).
 * @param image Matrix representation of image, single channel
 * @return Histogram (cv::Mat)
 */
inline cv::Mat computeSingleHistogram(cv::Mat& input, int& histSize, const float range[], int normalize)
{
	cv::Mat retHist;
	cv::calcHist(&input, 1, 0, cv::Mat(), retHist, 1, &histSize, &range, true, false);

	if(normalize == 1)
	{
		cv::normalize(retHist, retHist, 1, 0, cv::NORM_L1, -1, cv::Mat()); // L1-norm
	}

	return retHist;
}

/**
 * @function computeHSVHistogram
 * @brief Computes the HSV histogram for a given image (cv::Mat).
 * @param image Matrix representation of image, BGR format
 * @return Array of histograms, ([0] => H, [1] => S, [2] => V)
 */
inline std::vector<cv::Mat> computeHSVHistogram(cv::Mat& image, int cfgHueSize, int cfgSatSize, int cfgValSize, int normalize)
{
	cv::Mat hsvImage;
	cv::cvtColor(image, hsvImage, CV_BGR2HSV);

	std::vector<cv::Mat> hsvPlanes;
	cv::split(hsvImage, hsvPlanes);

	const float hranges[] = { 0, 180 };
	const float sranges[] = { 0, 256 };
	const float vranges[] = { 0, 256 };

	cv::Mat h_hist = computeSingleHistogram(hsvPlanes[0], cfgHueSize, hranges, normalize);
	cv::Mat s_hist = computeSingleHistogram(hsvPlanes[1], cfgSatSize, sranges, normalize);
	cv::Mat v_hist = computeSingleHistogram(hsvPlanes[2], cfgValSize, vranges, normalize);

	std::vector<cv::Mat> hists_splitted;
	hists_splitted.push_back(h_hist);
	hists_splitted.push_back(s_hist);
	hists_splitted.push_back(v_hist);

	return hists_splitted;
}


} // End namespace LBPHistogram

#endif // HAVE_OPENCV

#endif /* OPENCV_HSVHISTOGRAM_HPP_ */
