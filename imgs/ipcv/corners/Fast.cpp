/** Implementation file for finding corner features using FAST
 *
 *  \file ipcv/corners/Fast.cpp
 *  \author Josh Carstens, Zooted Off of IPCV (jc@mail.rit.edu)
 *  \date 07 Nov 2020
 *  \note I actually think I really like humans in general rn not sure why
 */

#include "Corners.h"

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "imgs/utils/utils.h"
#include <eigen3/Eigen/Dense>

using namespace std;

namespace ipcv {

/** Apply the FAST corner detector to a color image
 *
 *  \param[in] src     source cv::Mat of CV_8UC3
 *  \param[out] dst    destination cv:Mat of CV_8UC3
 *  \param[in] difference_threshold
 *                     brightness threshold to be used to determine whether
 *                     a surrounding pixels is brighter than or darker than
 *                     the candidate corner pixel
 *  \param[in] contiguous_threshold
 *                     number of contiguous pixels that must appear in
 *                     sequence in order for a candidate pixel to be
 *                     considered a corner pixel
 *  \param[in] nonmaximal_suppression
 *                     boolean parameter indicating whether non-maximal
 *                     suppression should be used to eliminate "clumping"
 *                     of identified corner points
 */
bool Fast(const cv::Mat& src, cv::Mat& dst, const int difference_threshold,
          const int contiguous_threshold, const bool nonmaximal_supression) {
  cv::Mat src_gray;
  cv::cvtColor(src, src_gray, cv::COLOR_BGR2GRAY);

  dst.create(src.size(), CV_8UC1);

  for (int row = 3; row < dst.rows - 3; row++) {
    for (int col = 3; col < dst.cols - 3; col++) {
      // First get surrounding pixel positions
      // Of course it's hard coded cuz I'm not gonna try to figure out polar
      // coords
      int current_pixel = src_gray.at<uint8_t>(row, col);
      Eigen::VectorXd radius(16);
      radius(0) = src_gray.at<uint8_t>(row - 3, col + 0);
      radius(1) = src_gray.at<uint8_t>(row - 3, col + 1);
      radius(2) = src_gray.at<uint8_t>(row - 2, col + 2);
      radius(3) = src_gray.at<uint8_t>(row - 1, col + 3);
      radius(4) = src_gray.at<uint8_t>(row - 0, col + 3);
      radius(5) = src_gray.at<uint8_t>(row + 1, col + 3);
      radius(6) = src_gray.at<uint8_t>(row + 2, col + 2);
      radius(7) = src_gray.at<uint8_t>(row + 3, col + 1);
      radius(8) = src_gray.at<uint8_t>(row + 3, col + 0);
      radius(9) = src_gray.at<uint8_t>(row + 3, col - 1);
      radius(10) = src_gray.at<uint8_t>(row + 2, col - 2);
      radius(11) = src_gray.at<uint8_t>(row + 1, col - 3);
      radius(12) = src_gray.at<uint8_t>(row + 0, col - 3);
      radius(13) = src_gray.at<uint8_t>(row - 1, col - 3);
      radius(14) = src_gray.at<uint8_t>(row - 2, col - 2);
      radius(15) = src_gray.at<uint8_t>(row - 3, col - 1);

      // Then iterate through each surrounding pixel and see if it's different
      // enough from the center
      Eigen::VectorXd difference(16);
      for (int idx = 0; idx < 16; idx++) {
        if (radius(idx) > current_pixel + difference_threshold) {
          // It's a 1 if it's sufficiently brighter
          difference(idx) = 1;
        } else if (radius(idx) < current_pixel - difference_threshold) {
          // A -1 if it's darker
          difference(idx) = -1;
        } else {
          // And a 0 if it's not different enough
          difference(idx) = 0;
        }
      }

      // This loops 16 times because that's the maximum number of times you can
      // shift the difference matrix and have it be unique
      for (int radius_pos = 0; radius_pos < 16; radius_pos++) {
        // This value counts how many concurrent brighter/darker pixels there
        // are
        int counter = 0;
        // Looping through each of the difference values
        for (int idx = 0; idx < 16; idx++) {
          // Adding a number if there's a concurrent bright or dark pixel, but
          // NOT if there's concurrent similar pixels
          if (difference((idx + 1) % 16) == difference(idx % 16) &&
              difference(idx) != 0) {
            counter++;
          }
        }

        if (counter >= contiguous_threshold) {
          // Setting a white pixel in our dst matrix if it's a corner and
          // exiting the loop because we already determined that it's a corner
          dst.at<uint8_t>(row, col) = 255;
          break;
        } else {
          // Shifting the difference vector so we can try again
          utils::ShiftVector(difference, 1);
        }
      }
    }
  }

  return true;
}
}  // namespace ipcv
