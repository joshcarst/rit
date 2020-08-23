/** Implementation file for computing a CDF from a histogram
 *
 *  \file ipcv/utils/HistogramToCdf.cpp
 *  \author Josh Carstens, refuses to eat fortune cookies (jdc3498@rit.edu)
 *  \date 26 Apr 2020
 */

#include "HistogramToCdf.h"

#include "imgs/ipcv/utils/HistogramToPdf.h"

namespace ipcv {

void HistogramToCdf(const cv::Mat& h, cv::Mat& cdf) {
  cv::Mat pdf;
  HistogramToPdf(h, pdf);

  cdf = cv::Mat::zeros(h.size(), CV_64F);

  double sum = 0;
  for (int row_idx = 0; row_idx < cdf.rows; row_idx++) {
    for (int col_idx = 0; col_idx < cdf.cols; col_idx++) {
      sum += pdf.at<double>(row_idx, col_idx);
      cdf.at<double>(row_idx, col_idx) = sum;
    }
    sum = 0;
  }
}
}  // namespace ipcv
