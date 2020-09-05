/** Implementation file for image enhancement using linear histogram statistics
 *
 *  \file ipcv/histogram_enhancement/LinearLut.cpp
 *  \author Josh Carstens, Septuple Threat (jdc3498@rit.edu)
 *  \date 5 Sep 2020
 *  \note I REALLY miss those PDFs that give u the whole project rundown from
 * ICC
 */

#include "LinearLut.h"

#include <iostream>

#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace ipcv {

/** Create a 3-channel (color) LUT using linear histogram enhancement
 *
 *  \param[in] src          source cv::Mat of CV_8UC3
 *  \param[in] percentage   the total percentage to remove from the tails
 *                          of the histogram to find the extremes of the
 *                          linear enhancemnt function
 *  \param[out] lut         3-channel look up table in cv::Mat(3, 256)
 */
bool LinearLut(const cv::Mat& src, const int percentage, cv::Mat& lut) {
  // Histogram calculations based on the given src image
  cv::Mat_<int> src_hist;
  ipcv::Histogram(src, src_hist);
  cv::Mat_<double> src_cdf;
  ipcv::HistogramToCdf(src_hist, src_cdf);

  // Initializing the LUT
  lut = cv::Mat_<uint8_t>::zeros(3, 256);

  for (int channel_idx = 0; channel_idx < src_cdf.rows; channel_idx++) {
    // Finding the lowest brightness value in the image, accounting for the
    int low;
    for (int brightness = 0; brightness < src_cdf.cols; brightness++) {
      if (src_cdf.at<double>(channel_idx, brightness) >= percentage / 200.0) {
        low = brightness;
        break;
      }
    }

    // Finding the highest brightness value in the image
    int high;
    for (int brightness = src_cdf.cols; brightness >= 0; brightness--) {
      if (src_cdf.at<double>(channel_idx, brightness) <=
          1 - percentage / 200.0) {
        high = brightness;
        break;
      }
    }

    // Gonna be honest I barely remember how the slope fits into things but this
    // is where we calculate it
    auto slope = 255 / (high - low);

    // Deriving the y intercept based on the slope
    auto intercept = -(slope * low);

    // Calculating the LUT's current value and clamping it to avoid clipping
    // before writing to the LUT matrix
    for (int brightness = 0; brightness < lut.cols; brightness++) {
      auto current_val = slope * brightness + intercept;
      current_val = clamp(current_val, 0, 255);
      lut.at<uint8_t>(channel_idx, brightness) = current_val;
    }
  }
  return true;
}
}  // namespace ipcv
