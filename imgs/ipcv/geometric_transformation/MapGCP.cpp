/** Implementation file for finding source image coordinates for a source-to-map
 *  remapping using ground control points
 *
 *  \file ipcv/geometric_transformation/MapGCP.cpp
 *  \author Josh Carstens, Nervous Boi (jc@mail.rit.edu)
 *  \date 26 Sep 2020
 *  \note Lol I didn't write a single line of code in this file until about 9pm
 * on Saturday :')
 */

#include "MapGCP.h"

#include <iostream>

#include <eigen3/Eigen/Dense>
#include <opencv2/core.hpp>

using namespace std;

namespace ipcv {

/** Find the source coordinates (map1, map2) for a ground control point
 *  derived mapping polynomial transformation
 *
 *  \param[in] src   source cv::Mat of CV_8UC3
 *  \param[in] map   map (target) cv::Mat of CV_8UC3
 *  \param[in] src_points
 *                   vector of cv::Points representing the ground control
 *                   points from the source image
 *  \param[in] map_points
 *                   vector of cv::Points representing the ground control
 *                   points from the map image
 *  \param[in] order  mapping polynomial order
 *                      EXAMPLES:
 *                        order = 1
 *                          a0*x^0*y^0 + a1*x^1*y^0 +
 *                          a2*x^0*y^1
 *                        order = 2
 *                          a0*x^0*y^0 + a1*x^1*y^0 + a2*x^2*y^0 +
 *                          a3*x^0*y^1 + a4*x^1*y^1 +
 *                          a5*x^0*y^2
 *                        order = 3
 *                          a0*x^0*y^0 + a1*x^1*y^0 + a2*x^2*y^0 + a3*x^3*y^0 +
 *                          a4*x^0*y^1 + a5*x^1*y^1 + a6*x^2*y^1 +
 *                          a7*x^0*y^2 + a8*x^1*y^2 +
 *                          a9*x^0*y^3
 *  \param[out] map1  cv::Mat of CV_32FC1 (size of the destination map)
 *                    containing the horizontal (x) coordinates at which to
 *                    resample the source data
 *  \param[out] map2  cv::Mat of CV_32FC1 (size of the destination map)
 *                    containing the vertical (y) coordinates at which to
 *                    resample the source data
 */
bool MapGCP(const cv::Mat src, const cv::Mat map,
            const vector<cv::Point> src_points,
            const vector<cv::Point> map_points, const int order, cv::Mat& map1,
            cv::Mat& map2) {

  map1.create(map.size(), CV_32FC1);
  map2.create(map.size(), CV_32FC1);

  // Wall of initializations
  Eigen::MatrixXf xbar_1(src_points.size(),3);
  Eigen::MatrixXf xbar_2(src_points.size(),6);
  Eigen::MatrixXf xbar_3(src_points.size(),10);
  Eigen::MatrixXf y_bar(src_points.size(), 2);
  Eigen::MatrixXf C;
  Eigen::VectorXf Input(2);
  Eigen::VectorXf Output(3);

  // Making an Eigen Matrix that's just src_points bc i'm mega dumm
  for (int yikes = 0; yikes < static_cast<int>(src_points.size()); yikes++) {
    y_bar(yikes,0) = src_points[yikes].x;
    y_bar(yikes,1) = src_points[yikes].y;
  }

  // Wall of hardcoding (that also doesn't seem to work)
  for (int frick = 0; frick < static_cast<int>(map_points.size()); frick++) {
    // Assigning two ints for easy access
    int x = map_points[frick].x;
    int y = map_points[frick].y;
    if (order == 1) {
      xbar_1(frick,0) = (x^0 * y^0);
      xbar_1(frick,1) = (x^1 * y^0);
      xbar_1(frick,2) = (x^0 * y^1);
      // This has to be wrong because it produces a buncha god dang -nan
      C = (xbar_1.transpose() * xbar_1).inverse() * xbar_1.transpose() * y_bar;
      Input(0) = x;
      Input(1) = y;
      Output = C * Input;
      // I'm pretty sure I wouldn't want to use x and y for map positioning here
      // but it's too late
      map1.at<float>(x, y) = Output(1);
      map2.at<float>(x, y) = Output(0);
    } else if (order == 2) {
      xbar_2(frick,0) = (x^0 * y^0);
      xbar_2(frick,1) = (x^1 * y^0);
      xbar_2(frick,2) = (x^2 * y^0);
      xbar_2(frick,3) = (x^0 * y^1);
      xbar_2(frick,4) = (x^1 * y^1);
      xbar_2(frick,5) = (x^0 * y^2);
      C = (xbar_2.transpose() * xbar_2).inverse() * xbar_2.transpose() * y_bar;
      Input(0) = x;
      Input(1) = y;
      Output = C * Input;
      map1.at<float>(x, y) = Output(1);
      map2.at<float>(x, y) = Output(0);
    } else if (order == 3) {
      xbar_3(frick,0) = (x^0 * y^0);
      xbar_3(frick,1) = (x^1 * y^0);
      xbar_3(frick,2) = (x^2 * y^0);
      xbar_3(frick,3) = (x^3 * y^0);
      xbar_3(frick,4) = (x^0 * y^1);
      xbar_3(frick,5) = (x^1 * y^1);
      xbar_3(frick,6) = (x^2 * y^1);
      xbar_3(frick,7) = (x^0 * y^2);
      xbar_3(frick,8) = (x^1 * y^2);
      xbar_3(frick,9) = (x^0 * y^3);
      C = (xbar_3.transpose() * xbar_3).inverse() * xbar_3.transpose() * y_bar;
      Input(0) = x;
      Input(1) = y;
      Output = C * Input;
      map1.at<float>(x, y) = Output(1);
      map2.at<float>(x, y) = Output(0);
    } else {
      cout << "Nice try dude" << endl;
    }
  }

  

  return true;
}
}  // namespace ipcv
