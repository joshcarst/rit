/** Implementation file for image enhancement using histogram matching
 *
 *  \file ipcv/histogram_enhancement/MatchingLut.cpp
 *  \author Josh Carstens, Cable Cutter (jdc3498@rit.edu)
 *  \date 5 Sep 2020
 *  \note For future context I go back and add notes and comments after I finish
 * the whole project
 */

#include "MatchingLut.h"

#include <iostream>

#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace ipcv {

/** Create a 3-channel (color) LUT using histogram matching
 *
 *  \param[in] src   source cv::Mat of CV_8UC3
 *  \param[in] h     the histogram in cv:Mat(3, 256) that the
 *                   source is to be matched to
 *  \param[out] lut  3-channel look up table in cv::Mat(3, 256)
 */
bool MatchingLut(const cv::Mat& src, const cv::Mat& h, cv::Mat& lut) {
  // Histogram calculations based on the given src image/tgt histogram
  cv::Mat_<int> src_hist;
  ipcv::Histogram(src, src_hist);
  cv::Mat_<double> src_cdf;
  ipcv::HistogramToCdf(src_hist, src_cdf);
  cv::Mat_<double> tgt_cdf;
  ipcv::HistogramToCdf(h, tgt_cdf);

  // Initializing the LUT
  lut = cv::Mat_<uint8_t>::zeros(3, 256);

  // Iterating through every brightness value for each of the three color
  // channels
  for (int channel_idx = 0; channel_idx < lut.rows; channel_idx++) {
    for (int brightness = 0; brightness < lut.cols; brightness++) {
      // Moving the target histogram down by the current source CDF value and
      // taking the absolute value so we can later find the minimum value (which
      // will indicate the value closest to 0)
      double csrc = src_cdf.at<double>(channel_idx, brightness);
      cv::Mat diff_cdf = cv::Mat_<double>::zeros(1, 256);
      for (int col_idx = 0; col_idx < tgt_cdf.cols; col_idx++) {
        diff_cdf.at<double>(col_idx) =
            abs(tgt_cdf.at<double>(channel_idx, col_idx) - csrc);
      }

      // Some needed variables for minMaxLoc
      double minimum, maximum;
      cv::Point min_loc, max_loc;

      // Finding the aforementioned minimum value that's closest to 0 and
      // writing it to the LUT
      cv::minMaxLoc(diff_cdf, &minimum, &maximum, &min_loc, &max_loc);
      lut.at<uint8_t>(channel_idx, brightness) = min_loc.x;
    }
  }

  return true;
}
}  // namespace ipcv
