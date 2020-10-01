/** Implementation file for remapping source values to map locations
 *
 *  \file ipcv/geometric_transformation/Remap.cpp
 *  \author Josh Carstens, Scared Boi (jc@mail.rit.edu)
 *  \date 26 Sep 2020
 *  \note This is by far the most stressful assignment I've had by far
 */

#include "Remap.h"

#include <iostream>

using namespace std;

namespace ipcv {

/** Remap source values to the destination array at map1, map2 locations
 *
 *  \param[in] src            source cv::Mat of CV_8UC3
 *  \param[out] dst           destination cv::Mat of CV_8UC3 for remapped values
 *  \param[in] map1           cv::Mat of CV_32FC1 (size of the destination map)
 *                            containing the horizontal (x) coordinates at
 *                            which to resample the source data
 *  \param[in] map2           cv::Mat of CV_32FC1 (size of the destination map)
 *                            containing the vertical (y) coordinates at
 *                            which to resample the source data
 *  \param[in] interpolation  interpolation to be used for resampling
 *  \param[in] border_mode    border mode to be used for out of bounds pixels
 *  \param[in] border_value   border value to be used when constant border mode
 *                            is to be used
 */
bool Remap(const cv::Mat& src, cv::Mat& dst, const cv::Mat& map1,
           const cv::Mat& map2, const Interpolation interpolation,
           const BorderMode border_mode, const uint8_t border_value) {
  dst.create(map1.size(), src.type());

  float x_out, y_out;

  for (int x = 0; x < dst.rows; x++) {
    for (int y = 0; y < dst.cols; y++) {
      if (interpolation == ipcv::Interpolation::NEAREST) {
        // Nearest neighbor implementation
        // Another case of having to switch x and y to get it to work. I can't
        // explain it
        y_out = floor(map1.at<float>(x, y) + 0.5);
        x_out = floor(map2.at<float>(x, y) + 0.5);
        if (border_mode == ipcv::BorderMode::CONSTANT) {
          // Constant border condition
          // If the x/y prime positions exit the source area, fill with
          // background color Otherwise write the current prime pixel to the
          // destination
          if (x_out < 0 || y_out < 0 || x_out >= src.rows ||
              y_out >= src.cols) {
            dst.at<cv::Vec3b>(x, y) = border_value;
          } else {
            dst.at<cv::Vec3b>(x, y) = src.at<cv::Vec3b>(x_out, y_out);
          }
        } else {
          // Replicate border condition
          // WRITE 0 IF BELOW 0, WRITE MAX ROW VALUE IF ABOVE MAX ROW VALUE
          dst.at<cv::Vec3b>(x, y) =
              src.at<cv::Vec3b>(clamp(x_out, static_cast<float>(0.0),
                                      static_cast<float>(src.rows - 1)),
                                clamp(y_out, static_cast<float>(0.0),
                                      static_cast<float>(src.cols - 1)));
        }
      } else {
        // Broken bilinear implementation
        // I just couldn't figure it out
        y_out = floor(map1.at<float>(x, y));
        x_out = floor(map2.at<float>(x, y));
        cv::Vec3b src_xy, src_x_plus_1, src_y_plus_1, src_xy_plus_1;
        float y_0, y_1;
        src_xy = src.at<cv::Vec3b>(x, y);
        src_y_plus_1 = src.at<cv::Vec3b>(x, y + 1);
        src_x_plus_1 = src.at<cv::Vec3b>(x + 1, y);
        src_xy_plus_1 = src.at<cv::Vec3b>(x + 1, y + 1);
        for (int row = 0; row < 3; row++) {
          y_0 = (src_x_plus_1[row] - src_xy[row]) * (x_out - x) + src_xy[row];
          y_1 = (src_xy_plus_1[row] - src_y_plus_1[row]) * (x_out - x) +
                   src_y_plus_1[row];
          dst.at<cv::Vec3b>(x, y)[row] =
              static_cast<uint8_t>(floor((y_1 - y_0) * (y_out - y) + y_0));
        }
      }
    }
  }

  return true;
}
}  // namespace ipcv
