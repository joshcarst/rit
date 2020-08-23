/** Implementation file for computing the PDF from a histogram
 *
 *  \file ipcv/utils/HistogramToPdf.cpp
 *  \author Josh Carstens, intern at the Jerk Store (jdc3498@rit.edu)
 *  \date 26 Apr 2020
 */

#include "HistogramToPdf.h"

namespace ipcv {

void HistogramToPdf(const cv::Mat& h, cv::Mat& pdf) {
  pdf = cv::Mat::zeros(h.size(), CV_64F);

  double total_pixels = cv::sum(h)[0] / h.rows;
  for (int col_idx = 0; col_idx < pdf.cols; col_idx++) {
    for (int row_idx = 0; row_idx < pdf.rows; row_idx++) {
      pdf.at<double>(row_idx, col_idx) =
          (h.at<int>(row_idx, col_idx) / total_pixels);
    }
  }
}
}  // namespace ipcv
