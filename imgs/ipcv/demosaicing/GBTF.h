/** Interface file for gradient-based threshold-free CFA demosaic function
 *
 *  \file ipcv/demosaic/Bilinear.h
 *  \author Josh Carstens, Man with No Game (jc@mail.rit.edu)
 *  \date 03 Mar 2021
 *  \note Help I have no game how do I get game
 */

#pragma once

#include <string>

#include <opencv2/core.hpp>

using namespace std;

namespace ipcv {

/** Interpolate CFA using GBTF interpolation
 *
 *  \param[in] src       source cv::Mat of CV_16UC1 containing CFA
 *  \param[in] dst       destination cv::Mat of CV_8UC3 containing image
 *  \param[in] pattern   a string defining the CFA layout:
 *                         'GBRG'  -  G B  Raspberry Pi (OmniVision OV5647)
 *                                    R G
 *                         'GRBG'  -  G R
 *                                    B G
 *                         'BGGR'  -  B G
 *                                    G R
 *                         'RGGB'  -  R G
 *                                    G B
 *
 *  \return              none (void)
 */
void GBTF(const cv::Mat &src, cv::Mat &dst, string pattern);
}
