/** Implementation file for mapping a source quad on to a target quad
 *
 *  \file ipcv/geometric_transformation/MapQ2Q.cpp
 *  \author Josh Carstens, Burnout Technician (jc@mail.rit.edu)
 *  \date 3 Oct 2020
 *  \note Out of sorrow over last week's project I present thee a pristine
 * example of code at an expedited pace
 */

#include "MapQ2Q.h"

#include <iostream>

#include <eigen3/Eigen/Dense>
#include <opencv2/core.hpp>

using namespace std;

namespace ipcv {

/** Find the source coordinates (map1, map2) for a quad to quad mapping
 *
 *  \param[in] src       source cv::Mat of CV_8UC3
 *  \param[in] tgt       target cv::Mat of CV_8UC3
 *  \param[in] src_vertices
 *                       vertices cv:Point of the source quadrilateral (CW)
 *                       which is to be mapped to the target quadrilateral
 *  \param[in] tgt_vertices
 *                       vertices cv:Point of the target quadrilateral (CW)
 *                       into which the source quadrilateral is to be mapped
 *  \param[out] map1     cv::Mat of CV_32FC1 (size of the destination map)
 *                       containing the horizontal (x) coordinates at
 *                       which to resample the source data
 *  \param[out] map2     cv::Mat of CV_32FC1 (size of the destination map)
 *                       containing the vertical (y) coordinates at
 *                       which to resample the source data
 */
bool MapQ2Q(const cv::Mat src, const cv::Mat tgt,
            const vector<cv::Point> src_vertices,
            const vector<cv::Point> tgt_vertices, cv::Mat& map1,
            cv::Mat& map2) {
  // Setting map sizes to that of the target matrix
  map1.create(tgt.size(), CV_32FC1);
  map2.create(tgt.size(), CV_32FC1);

  // Left-hand sides of plane equations for both source and target datasets
  Eigen::MatrixXf src_lhs(3, 3);
  src_lhs << src_vertices[0].y, src_vertices[1].y, src_vertices[2].y,
      src_vertices[0].x, src_vertices[1].x, src_vertices[2].x, 1, 1, 1;

  Eigen::MatrixXf tgt_lhs(3, 3);
  tgt_lhs << tgt_vertices[0].y, tgt_vertices[1].y, tgt_vertices[2].y,
      tgt_vertices[0].x, tgt_vertices[1].x, tgt_vertices[2].x, 1, 1, 1;

  // Right-hand sides of the same equations
  Eigen::VectorXf src_rhs(3);
  src_rhs << src_vertices[3].y, src_vertices[3].x, 1;

  Eigen::VectorXf tgt_rhs(3);
  tgt_rhs << tgt_vertices[3].y, tgt_vertices[3].x, 1;

  // Calculating the weights to scale B/A matrix values by
  Eigen::VectorXf src_weights = src_lhs.inverse() * src_rhs;

  Eigen::VectorXf tgt_weights = tgt_lhs.inverse() * tgt_rhs;

  // Producing B/A matrices that are scaled by weights vectors
  Eigen::MatrixXf B_bar(3, 3);
  B_bar << src_weights(0) * src_vertices[0].y,
      src_weights(1) * src_vertices[1].y, src_weights(2) * src_vertices[2].y,
      src_weights(0) * src_vertices[0].x, src_weights(1) * src_vertices[1].x,
      src_weights(2) * src_vertices[2].x, src_weights(0), src_weights(1),
      src_weights(2);

  Eigen::MatrixXf A_bar(3, 3);
  A_bar << tgt_weights(0) * tgt_vertices[0].y,
      tgt_weights(1) * tgt_vertices[1].y, tgt_weights(2) * tgt_vertices[2].y,
      tgt_weights(0) * tgt_vertices[0].x, tgt_weights(1) * tgt_vertices[1].x,
      tgt_weights(2) * tgt_vertices[2].x, tgt_weights(0), tgt_weights(1),
      tgt_weights(2);

  // Forming the actual projection matrix to go from map space to source space
  Eigen::MatrixXf p_ms = B_bar * A_bar.inverse();

  // Vectors for holding map position data
  Eigen::VectorXf input(3);
  Eigen::VectorXf output(3);

  // X and Y are all we care about so the third point is always 1
  input(2) = 1;
  // Looping through all map points
  for (int row = 0; row < map1.rows; row++) {
    input(0) = row;
    for (int col = 0; col < map1.cols; col++) {
      input(1) = col;
      // Calculating output map positions
      output = p_ms * input;
      // Assigning to map matrices
      map1.at<float>(row, col) = output(1) / output(2);
      map2.at<float>(row, col) = output(0) / output(2);
    }
  }

  return true;
}
}  // namespace ipcv
