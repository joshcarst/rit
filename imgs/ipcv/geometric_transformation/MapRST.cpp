/** Implementation file for finding map coordinates for an RST transformation
 *
 *  \file ipcv/geometric_transformation/MapRST.cpp
 *  \author Carl Salvaggio, Ph.D. (salvaggio@cis.rit.edu)
 *  \date 26 Sep 2019
 */

#include "MapRST.h"

#include <iostream>

#include <eigen3/Eigen/Dense>

using namespace std;

namespace ipcv {

/** Find the map coordinates (map1, map2) for an RST transformation
 *
 *  \param[in] src           source cv::Mat of CV_8UC3
 *  \param[in] angle         rotation angle (CCW) [radians]
 *  \param[in] scale_x       horizontal scale
 *  \param[in] scale_y       vertical scale
 *  \param[in] translation_x horizontal translation [+ right]
 *  \param[in] translation_y vertical translation [+ up]
 *  \param[out] map1         cv::Mat of CV_32FC1 (size of the destination map)
 *                           containing the horizontal (x) coordinates at
 *                           which to resample the source data
 *  \param[out] map2         cv::Mat of CV_32FC1 (size of the destination map)
 *                           containing the vertical (y) coordinates at
 *                           which to resample the source data
 */
bool MapRST(const cv::Mat src, const double angle, const double scale_x,
            const double scale_y, const double translation_x,
            const double translation_y, cv::Mat& map1, cv::Mat& map2) {

  Eigen::MatrixXf RST(2,3);
  RST << scale_x * cos(angle), scale_y * sin(angle), scale_x * translation_y * cos(angle) + scale_y * translation_x * sin(angle),
         -scale_x * sin(angle), scale_y * cos(angle), scale_y * translation_x * cos(angle) - scale_x * translation_y * sin(angle);

  map1 = cv::Mat(((src.cols * sin(fmod(angle, 90))) + (src.rows * cos(fmod(angle, 90)))) * scale_x,((src.cols * cos(fmod(angle, 90))) + (src.rows * sin(fmod(angle, 90)))) * scale_y,CV_32FC1);
  map2 = cv::Mat(((src.cols * sin(fmod(angle, 90))) + (src.rows * cos(fmod(angle, 90)))) * scale_x,((src.cols * cos(fmod(angle, 90))) + (src.rows * sin(fmod(angle, 90)))) * scale_y,CV_32FC1);

  Eigen::VectorXf Input(3);
  Eigen::VectorXf Output(3);
  Input(2) = 1;
  for (float y = 0; y < map1.cols; y++) {
    Input(1) = map1.cols / 2 - y;
    for (float x = 0; x < map1.rows; x++) {
      Input(0) = x - map1.rows / 2;
      Output = RST * Input;
      Output(0) += src.rows / 2;
      Output(1) = src.cols / 2 - Output(1);
      map1.at<float>(x, y) = Output(1);
      map2.at<float>(x, y) = Output(0);
    }
  }

  return true;
}
}
