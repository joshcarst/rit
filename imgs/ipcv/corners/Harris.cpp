/** Implementation file for finding corner features using Harris
 *
 *  \file ipcv/corners/Harris.cpp
 *  \author Josh Carstens, Uploaded His Consciousness to MediaFire
 * (jc@mail.rit.edu) \date 7 Nov 2020 \note https://youtu.be/QojMooSYJGk
 */

#include "Corners.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"

using namespace std;

namespace ipcv {

/** Apply the Harris corner detector to a color image
 *
 *  \param[in] src     source cv::Mat of CV_8UC3
 *  \param[out] dst    destination cv:Mat of CV_32FC1
 *  \param[in] sigma   standard deviation of the Gaussian blur kernel
 *  \param[in] k       free parameter in the equation
 *                        dst = (lambda1)(lambda2) - k(lambda1 + lambda2)^2
 */
bool Harris(const cv::Mat& src, cv::Mat& dst, const float sigma,
            const float k) {
  cv::Mat src_gray;
  cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);

  dst.create(src_gray.size(), CV_32FC1);

  // Matrices for the horizontal and vertical partial derivatives
  cv::Mat I_x;
  cv::Mat I_y;

  // Forming the transformation kernels for both directions
  cv::Mat kernel_x;
  kernel_x.create(3, 3, CV_32FC1);
  kernel_x.at<float>(0, 0) = -1;
  kernel_x.at<float>(1, 0) = -1;
  kernel_x.at<float>(2, 0) = -1;
  kernel_x.at<float>(0, 1) = 0;
  kernel_x.at<float>(1, 1) = 0;
  kernel_x.at<float>(2, 1) = 0;
  kernel_x.at<float>(0, 2) = 1;
  kernel_x.at<float>(1, 2) = 1;
  kernel_x.at<float>(2, 2) = 1;

  cv::Mat kernel_y;
  kernel_y.create(3, 3, CV_32FC1);
  kernel_y.at<float>(0, 0) = -1;
  kernel_y.at<float>(0, 1) = -1;
  kernel_y.at<float>(0, 2) = -1;
  kernel_y.at<float>(1, 0) = 0;
  kernel_y.at<float>(1, 1) = 0;
  kernel_y.at<float>(1, 2) = 0;
  kernel_y.at<float>(2, 0) = 1;
  kernel_y.at<float>(2, 1) = 1;
  kernel_y.at<float>(2, 2) = 1;

  // Applying the transformations
  cv::filter2D(src_gray, I_x, CV_32FC1, kernel_x, cv::Point(-1, -1), 0);
  cv::filter2D(src_gray, I_y, CV_32FC1, kernel_y, cv::Point(-1, -1), 0);

  // Matrices for the directional gradients
  cv::Mat A, B, C;
  cv::pow(I_x, 2, A);
  cv::pow(I_y, 2, B);
  cv::multiply(I_x, I_y, C);

  // Applying the w kernel across each matrix the easy way
  cv::GaussianBlur(A, A, cv::Size(0, 0), sigma);
  cv::GaussianBlur(B, B, cv::Size(0, 0), sigma);
  cv::GaussianBlur(C, C, cv::Size(0, 0), sigma);

  // Repurposing matrices here so it gets a little confusing
  // I_x and I_y become the two components of Det(M) (AB and C^2)
  cv::multiply(A, B, I_x);
  cv::pow(C, 2, I_y);
  // Then dst becomes Tr(M)^2 ((A + B)^2)
  cv::pow((A + B), 2.0, dst);
  // Then we scale Tr(M)^2 by k
  dst *= k;
  // And finally form the entire Det(M) (AB - C^2) and calculate R
  dst = (I_x - I_y) - dst;

  // This part is the nms calculation. First we take what we currently have in
  // dst and store it in a new matrix and clear dst
  cv::Mat dst_dupe;
  dst.copyTo(dst_dupe);
  dst = 0;
  // Then we loop through the image in 5x5 neighborhoods (I know Carl said 3x3
  // but that didn't work)
  for (int row = 2; row < dst_dupe.rows - 2; row++) {
    for (int col = 2; col < dst_dupe.cols - 2; col++) {
      // We form each neighborhood as its own matrix and see what the maximum
      // value is
      cv::Mat neighborhood(dst_dupe, cv::Range(row - 2, row + 2),
                           cv::Range(col - 2, col + 2));
      double min, max;
      cv::minMaxLoc(neighborhood, &min, &max);
      // If the center pixel is the maximum value and is greater than 0 then we
      // put a white pixel there in the actual dst matrix
      if (neighborhood.at<float>(2, 2) == max &&
          neighborhood.at<float>(2, 2) > 0) {
        dst.at<float>(row, col) = 255;
      }
    }
  }

  return true;
}
}  // namespace ipcv
