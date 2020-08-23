/** Interface file for computing a PDF from a histogram
 *
 *  \file ipcv/utils/HistogramToPdf.h
 *  \author Josh Carstens, entering hypersleep (jdc3498@rit.edu)
 *  \date 26 Apr 2020
 */

#pragma once

#include <opencv2/core.hpp>

namespace ipcv {

/** Compute the probability density function from a 3-channel image histogram
 *
 *  \param[in] h   the 3-channel image histogram
 *  \param[out] pdf the probability density for the supplied histogram
 */
void HistogramToPdf(const cv::Mat& h, cv::Mat& pdf);
}
