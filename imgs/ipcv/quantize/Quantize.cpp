/** Implementation file for image quantization
 *
 *  \file ipcv/quantize/quantize.cpp
 *  \author Josh Carstens, Master of None (jdc3498@rit.edu)
 *  \date 17 Mar 2018
 *  \note If u thought I was gonna stop putting dumb notes n comments in my code
 * ur dead wrong kid
 */

#include "Quantize.h"

#include <iostream>

using namespace std;

/** Perform uniform grey-level quantization on a color image
 *
 *  \param[in] src                 source cv::Mat of CV_8UC3
 *  \param[in] quantization_levels the number of levels to which to quantize
 *                                 the image
 *  \param[out] dst                destination cv:Mat of CV_8UC3
 */
void Uniform(const cv::Mat& src, const int quantization_levels, cv::Mat& dst) {
  // You would think looping through the channels instead of manually addressing
  // each channel within the column loop would be slower, but in fact that is
  // not the case because I timed it and it wasn't
  for (int channel_idx = 0; channel_idx < 3; channel_idx++) {
    for (int row_idx = 0; row_idx < src.rows; row_idx++) {
      for (int col_idx = 0; col_idx < src.cols; col_idx++) {
        // At each channel for each pixel, we read the original value and write
        // the result of multiplying it by (q_levels / output_levels (always 256
        // in our case)) to the corresponding pixel on the destination matrix
        dst.at<cv::Vec3b>(row_idx, col_idx)[channel_idx] =
            static_cast<int>(src.at<cv::Vec3b>(row_idx, col_idx)[channel_idx] *
                             (quantization_levels / 256.0));
      }
    }
  }
}

/** Perform improved grey scale quantization on a color image
 *
 *  \param[in] src                 source cv::Mat of CV_8UC3
 *  \param[in] quantization_levels the number of levels to which to quantize
 *                                 the image
 *  \param[out] dst                destination cv:Mat of CV_8UC3
 */
void Igs(const cv::Mat& src, const int quantization_levels, cv::Mat& dst) {
  for (int channel_idx = 0; channel_idx < 3; channel_idx++) {
    // One would expect there to be artifacts associated with using one
    // remainder integer instead of three (one for each channel), but I couldn't
    // find any visible ones if you do an entire channel at a time instead of
    // alternating between channels in each row/column with the one integer
    int remainder = 0;
    for (int row_idx = 0; row_idx < src.rows; row_idx++) {
      for (int col_idx = 0; col_idx < src.cols; col_idx++) {
        // Saving the original pixel value to an int for shorter lines
        int src_pixel = (src.at<cv::Vec3b>(row_idx, col_idx)[channel_idx]);
        // Adding the remainder of the previous pixel's value divided by (256 /
        // q_levels) to the current pixel's value, but only if the resulting
        // value doesn't exceed 255
        if ((src_pixel + remainder) <= 255) {
          src_pixel += remainder;
          // Calculating the remainder for use with the next pixel
          remainder = src_pixel % (256 / quantization_levels);
        } else {
          remainder = 0;
        }
        // Scaling and writing the new pixel value to the destination matrix
        dst.at<cv::Vec3b>(row_idx, col_idx)[channel_idx] =
            src_pixel * (quantization_levels / 256.0);
      }
    }
  }
}

namespace ipcv {

bool Quantize(const cv::Mat& src, const int quantization_levels,
              const QuantizationType quantization_type, cv::Mat& dst) {
  dst.create(src.size(), src.type());

  switch (quantization_type) {
    case QuantizationType::uniform:
      Uniform(src, quantization_levels, dst);
      break;
    case QuantizationType::igs:
      Igs(src, quantization_levels, dst);
      break;
    default:
      cerr << "Specified quantization type is unsupported" << endl;
      return false;
  }

  return true;
}
}  // namespace ipcv
