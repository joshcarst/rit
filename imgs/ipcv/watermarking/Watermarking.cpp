/** Implementation file for phase dispersion watermarking function
 *
 *  \file ipcv/watermarking/Watermarking.cpp
 *  \author Josh Carstens, Kinda Fast (jc@mail.rit.edu)
 *  \date 12 Mar 2021
 *  \note If you're reading this please message me "jupiter down" on Discord for
 * verification
 */

#include <iostream>
#include <cmath>
#include <complex>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

namespace ipcv {

int watermarking(const cv::Mat& src, cv::Mat& dst, cv::Mat& key,
                 cv::Mat& decode) {
  // First make a carrier matrix which is really just random noise
  cv::Mat carrier;
  carrier.create(key.size(), CV_32F);
  for (int row = 0; row < carrier.rows; row++) {
    for (int col = 0; col < carrier.cols; col++) {
      carrier.at<float>(row, col) = rand() % 256;
    }
  }

  // Take just one channel (the blue one) and slam it to either 255 or 0
  // No particular reason for using the blue channel we just need a single
  // channel
  cv::Mat key_B, key_BGR[3];
  cv::split(key, key_BGR);
  key_B = key_BGR[0];
  key_B.convertTo(key_B, CV_32F);
  for (int row = 0; row < key_B.rows; row++) {
    for (int col = 0; col < key_B.cols; col++) {
      if (key_B.at<float>(row, col) < 128) {
        key_B.at<float>(row, col) = 0;
      } else {
        key_B.at<float>(row, col) = 255;
      }
    }
  }

  // Take DFTs of both the carrier image and the key image and then convolve
  // them
  cv::Mat carrier_dft, key_dft, convolution_dft;
  carrier_dft.create(carrier.size(), CV_32F);
  key_dft.create(key.size(), CV_32F);
  convolution_dft.create(key.size(), CV_32FC1);
  cv::dft(key_B, key_dft, cv::DFT_COMPLEX_OUTPUT + cv::DFT_SCALE);
  cv::dft(carrier, carrier_dft, cv::DFT_COMPLEX_OUTPUT + cv::DFT_SCALE);
  convolution_dft = key_dft.mul(carrier_dft);

  // Take the inverse DFT of the convolution result
  cv::Mat convolution;
  convolution.create(key.size(), CV_32F);
  cv::dft(convolution_dft, convolution, cv::DFT_INVERSE + cv::DFT_REAL_OUTPUT);

  // Scale factor (this should be way lower than 1 but oops)
  float scale = 1;

  // Subtract the avg of the convolution result out and scale it
  cv::Scalar average;
  average = cv::mean(convolution);
  convolution -= average;
  convolution *= .01;

  // Tile the convolution result across the size of the source image
  while (convolution.cols < src.cols) {
    cv::hconcat(convolution, convolution, convolution);
  }
  while (convolution.rows < src.rows) {
    convolution.push_back(convolution);
  }

  // Duplicate the single convolved channel across the other two
  cv::Mat convolution_merge;
  convolution_merge.create(key.size(), CV_32FC3);
  cv::Rect crop_size(0, 0, src.rows, src.cols);
  cv::Mat merged[] = {convolution(crop_size), convolution(crop_size),
                      convolution(crop_size)};
  cv::merge(merged, 3, convolution_merge);

  // Add the merged convolution matrix to the source to create an embedded image
  cv::Mat dst_calc;
  dst_calc.create(src.size(), CV_32F);
  src.convertTo(dst_calc, CV_32F);
  dst_calc = dst_calc + convolution_merge;
  dst_calc.convertTo(dst, CV_8U);

  // Create a matrix the size of the key that's a key-sized grid of dst squares
  // summed together
  int row_count = 0;
  int col_count = 0;
  cv::Mat dst_sum = cv::Mat::zeros(cv::Size(key.size()), CV_32F);
  for (int row = 0; row < dst.rows; row++) {
    row_count++;
    if (row_count == key.rows) {
      row_count = 0;
    }
    for (int col = 0; col < dst.cols; col++) {
      col_count++;
      if (col_count == key.cols) {
        col_count = 0;
      }
      dst_sum.at<float>(row_count, col_count) += dst_calc.at<float>(row, col);
    }
  }

  // Take the DFT of the dst sum
  cv::Mat summed_dft;
  summed_dft.create(key.size(), CV_32F);
  cv::dft(dst_sum, summed_dft, cv::DFT_COMPLEX_OUTPUT + cv::DFT_SCALE);

  // Take the cyclic correlation of the DFT of the dst sum and the DFT of the
  // carrier
  cv::Mat correlation_dft;
  correlation_dft.create(key.size(), CV_32F);
  summed_dft.copyTo(correlation_dft);
  correlation_dft = 0;
  cv::mulSpectrums(summed_dft, carrier_dft, correlation_dft, 0, true);

  // Take the inverse DFT of the result
  cv::Mat correlation;
  correlation.create(key.size(), CV_32F);
  cv::dft(correlation_dft, correlation, cv::DFT_INVERSE + cv::DFT_REAL_OUTPUT);

  // Normalize that boi bc the numbers were huge
  cv::normalize(correlation, correlation, 0, 255, cv::NORM_MINMAX, CV_32FC1);

  // Scale the original key matrix and add it to the decoded key matrix
  // Idk seems like cheating but that's what the paper said to do
  key_B *= scale;

  // Finally add the scaled original key matrix in with the correlation to
  // produce a result
  decode = key_B + correlation;
  decode.convertTo(decode, CV_8U);

  return 0;
}
}  // namespace ipcv
