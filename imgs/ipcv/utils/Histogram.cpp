/** Implementation file for computing an image histogram
 *
 *  \file ipcv/utils/Histogram.cpp
 *  \author Josh Carstens, buyer of fine art (jdc3498@rit.edu)
 *  \date 26 Apr 2020
 */

#include "Histogram.h"

namespace ipcv {

void Histogram(const cv::Mat& src, cv::Mat& h) {
  h = cv::Mat_<int>::zeros(3, 256);

  for (int channel_idx = 0; channel_idx < 3; channel_idx++) {
    for (int col_idx = 0; col_idx < src.cols; col_idx++) {
      for (int row_idx = 0; row_idx < src.rows; row_idx++) {
        int brightness = src.at<cv::Vec3b>(row_idx, col_idx)[channel_idx];
        h.at<int>(channel_idx, brightness) += 1;
      }
    }
  }
}
}  // namespace ipcv
