/** Implementation file for bilateral filtering
 *
 *  \file ipcv/bilateral_filtering/BilateralFilter.cpp
 *  \author Josh Carstens, Too Slow (jc@mail.rit.edu)
 *  \date 29 Oct 2020
 */

#include "BilateralFilter.h"

#include <iostream>

using namespace std;

namespace ipcv {

/** Bilateral filter an image
 *
 *  \param[in] src             source cv::Mat of CV_8UC3
 *  \param[out] dst            destination cv::Mat of ddepth type
 *  \param[in] sigma_distance  standard deviation of distance/closeness filter
 *  \param[in] sigma_range     standard deviation of range/similarity filter
 *  \param[in] radius          radius of the bilateral filter (if negative, use
 *                             twice the standard deviation of the distance/
 *                             closeness filter)
 *  \param[in] border_mode     pixel extrapolation method
 *  \param[in] border_value    value to use for constant border mode
 */
bool BilateralFilter(const cv::Mat& src, cv::Mat& dst,
                     const double sigma_distance, const double sigma_range,
                     const int radius, const BorderMode border_mode,
                     uint8_t border_value) {
  dst.create(src.size(), src.type());

  // do the new radius if it negative
  int new_radius = radius;
  if (new_radius < 0) {
    new_radius = 2 * sigma_distance;
  }

  // oh god uhh lab conversion
  cv::Mat src_lab;
  cv::cvtColor(src, src_lab, cv::COLOR_BGR2Lab);

  // make it all black
  dst = 0;

  for (int channel = 0; channel < 3; channel++) {
    for (int row = 0; row < src.rows; row++) {
      for (int col = 0; col < src.cols; col++) {
        // some initialization bada bing bada boom
        int x_neighbor = 0;
        int y_neighbor = 0;
        double neighborhood_sum = 0;
        double scale = 0;
        float range = 0;
        // loop through every pixel in the neighborhood!!
        for (int row2 = 0; row2 < new_radius * 2; row2++) {
          for (int col2 = 0; col2 < new_radius * 2; col2++) {
            x_neighbor = row - (new_radius - row2);
            y_neighbor = col - (new_radius - col2);
            float distance =
                src_lab.at<cv::Vec3b>(x_neighbor, y_neighbor)[channel] -
                src_lab.at<cv::Vec3b>(row, col)[channel];
            range = float(sqrt(((row - x_neighbor) * (row - x_neighbor)) +
                               ((col - y_neighbor) * (col - y_neighbor))));
            double g_distance = exp(-(distance * distance) /
                                    (2 * sigma_distance * sigma_distance));
            double g_range =
                exp(-(range * range) / (2 * sigma_range * sigma_range));
            neighborhood_sum +=
                (src_lab.at<cv::Vec3b>(x_neighbor, y_neighbor)[channel] *
                 g_distance * g_range);
            scale += (g_distance * g_range);
          }
        }
        // scale that boi down
        neighborhood_sum /= scale;
        dst.at<cv::Vec3b>(row, col)[channel] = neighborhood_sum;
      }
    }
  }

  // convert back to rgb!!
  cv::cvtColor(dst, dst, cv::COLOR_Lab2BGR);

  return true;
}
}  // namespace ipcv
