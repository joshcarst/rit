/** Interface file for computing an image histogram
 *
 *  \file ipcv/utils/Histogram.h
 *  \author Josh Carstens, Tommy Cash's biggest fan (jdc3498@rit.edu)
 *  \date 26 Apr 2020
 */

#pragma once

#include <opencv2/core.hpp>

namespace ipcv {

/** Compute the 3-channel image histogram of the provided source image
 *
 *  \param[in] src  source cv::Mat of CV_8UC3
 *  \param[out] h   the grey-level histogram for the source image
 */
void Histogram(const cv::Mat& src, cv::Mat& h);
}
