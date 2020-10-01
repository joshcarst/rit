/** Implementation file for finding map coordinates for an RST transformation
 *
 *  \file ipcv/geometric_transformation/MapRST.cpp
 *  \author Josh Carstens, Desperate Boi (jc@mail.rit.edu)
 *  \date 26 Sep 2020
 *  \note Fun fact, this is the least messed up of my three files
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
  // So for some reason when I was scaling it would go the wrong way, i.e. it
  // would scale x by 0.8 when I typed 1.2. This is a pretty jank fix but it's a
  // fix nonetheless
  double scale_x_inverse = (2 * (1 - scale_x) + scale_x);
  double scale_y_inverse = (2 * (1 - scale_y) + scale_y);

  // Filling up the RST matrix. I found the math for this in an old RIT DIP
  // lecture, it's scary but more efficient
  Eigen::MatrixXf RST(2, 3);
  RST << scale_y_inverse * cos(angle), scale_x_inverse * sin(angle),
      scale_y_inverse * translation_y * cos(angle) +
          scale_x_inverse * translation_x * sin(angle),
      -scale_y_inverse * sin(angle), scale_x_inverse * cos(angle),
      scale_x_inverse * translation_x * cos(angle) -
          scale_y_inverse * translation_y * sin(angle);

  // Determining the size of the map. Based on
  // https://iiif.io/api/annex/notes/rotation/
  map1 = cv::Mat(
      floor(src.cols * abs(sin(angle)) + src.rows * abs(cos(angle))) * scale_y,
      floor(src.cols * abs(cos(angle)) + src.rows * abs(sin(angle))) * scale_x,
      CV_32FC1);
  map2 = cv::Mat(
      floor(src.cols * abs(sin(angle)) + src.rows * abs(cos(angle))) * scale_y,
      floor(src.cols * abs(cos(angle)) + src.rows * abs(sin(angle))) * scale_x,
      CV_32FC1);

  // Making vectors for input and output coordinates. They need to be 3 tall
  // because the RST matrix is 3 long but we don't actually need the third value
  // for anything so it's just always 1
  Eigen::VectorXf Input(3);
  Eigen::VectorXf Output(3);
  Input(2) = 1;
  for (float y = 0; y < map1.cols; y++) {
    // Shifting the origin in preparation for rotation
    Input(1) = map1.cols / 2 - y;
    for (float x = 0; x < map1.rows; x++) {
      // Doing a similar shift for the x axis
      Input(0) = x - map1.rows / 2;
      // The actual transform
      Output = RST * Input;
      // Putting the origin back so everything's in the right place
      Output(0) += src.rows / 2;
      Output(1) = src.cols / 2 - Output(1);
      // Writing to the maps. The x and y are switched in the output for some
      // reason but it works
      map1.at<float>(x, y) = Output(1);
      map2.at<float>(x, y) = Output(0);
    }
  }

  return true;
}
}  // namespace ipcv
