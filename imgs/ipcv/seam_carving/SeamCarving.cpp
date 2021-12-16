/** Implementation file for seam carving
 *
 *  \file ipcv/seam_carving/SeamCarving.cpp
 *  \author Josh Carstens, Looking for a Hot Date (jc@mail.rit.edu)
 *  \date 30 Apr 2021
 *  \note So this is it huh
 */

#include <iostream>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "SeamCarving.h"

using namespace std;

namespace ipcv {

cv::Mat IndivSeam(const cv::Mat& src, char direction) {
  cv::Mat kernel_x;
  cv::Mat kernel_y;
  kernel_x.create(3, 3, CV_32FC1);
  kernel_x.at<float>(0, 0) = -1;
  kernel_x.at<float>(0, 1) = -2;
  kernel_x.at<float>(0, 2) = -1;
  kernel_x.at<float>(2, 0) = 1;
  kernel_x.at<float>(2, 1) = 2;
  kernel_x.at<float>(2, 2) = 1;
  cv::transpose(kernel_x, kernel_y);

  kernel_x /= 9.0;
  kernel_y /= 9.0;
  cv::Point anchor;
  anchor.x = 1;
  anchor.y = 1;
  cv::Mat filter_x = cv::Mat::zeros(src.rows, src.cols, CV_32FC1);
  cv::Mat filter_y = cv::Mat::zeros(src.rows, src.cols, CV_32FC1);
  int ddepth = CV_32FC1;

  cv::filter2D(src, filter_x, ddepth, kernel_x, anchor);
  cv::filter2D(src, filter_y, ddepth, kernel_y, anchor);
  cv::Mat energy = cv::Mat::zeros(src.rows, src.cols, src.type());
  cv::Mat point;
  point.create(1, 256, CV_32FC1);
  cv::Mat section;
  section.create(9, 9, CV_32FC1);
  float total_energy = 10000;
  float current_energy = 10000;
  cv::Mat dst;
  cv::Mat src_dupe;

  for (int row = 4; row < src.rows - 4; row++) {
    for (int col = 4; col < src.cols - 4; col++) {
      for (float row_2 = -4; row_2 < 5; row_2++) {
        for (float col_2 = -4; col_2 < 5; col_2++) {
          int idx = src.at<float>(row + row_2, col + col_2);
          section.at<float>(row_2 + 4, col_2 + 4) =
              src.at<float>(row + row_2, col + col_2);
          point.at<float>(0, idx) = point.at<float>(0, idx) + 1;
        }
      }
      point /= 81;
      float energy_count = 0;
      for (int row_2 = 0; row_2 < 256; row_2++) {
        if (point.at<float>(0, row_2) != 0) {
          energy_count = energy_count + (-1 * point.at<float>(0, row_2) *
                                         log2(point.at<float>(0, row_2)));
        }
      }
      energy.at<float>(row, col) = energy_count;
      point = cv::Mat::zeros(1, 256, CV_32FC1);
    }
  }

  cv::Mat ent_component = cv::Mat::zeros(src.rows, src.cols, CV_32FC1);
  for (int row = 0; row < src.rows; row++) {
    for (int col = 0; col < src.cols; col++) {
      float x_component = filter_x.at<float>(row, col);
      float y_component = filter_y.at<float>(row, col);
      ent_component.at<float>(row, col) =
          sqrt(x_component * x_component + y_component * y_component);
    }
  }
  ent_component = abs(ent_component);

  cv::Mat entropy = energy + ent_component;

  for (int row = 0; row < src.rows; row++) {
    for (int col = 0; col < src.cols; col++) {
      if ((row < 4 || row > src.rows - 5) || (col < 4 || col > src.cols - 5)) {
        entropy.at<float>(row, col) = 10000;
      }
    }
  }
  if (direction == 'r') {
    dst.create(src.rows - 1, src.cols, CV_32FC1);
    float current_row = 0;
    for (int row = 4; row < src.rows - 5; row++) {
      if (total_energy < current_energy) {
        current_energy = total_energy;
        current_row = row - 1;
      }
      float current_val = row;
      total_energy = 0;
      for (int col = 4; col < src.cols - 5; col++) {
        float w = entropy.at<float>(current_val - 1, col + 1);
        float c = entropy.at<float>(current_val, col + 1);
        float e = entropy.at<float>(current_val + 1, col + 1);
        if (w < c && w < e) {
          current_val = current_val - 1;
        } else if (c < w && c < e) {
          current_val = current_val;
        } else if (e < c && e < w) {
          current_val = current_val + 1;
        }
        total_energy = total_energy + entropy.at<float>(current_val, col + 1);
      }
    }
    cout << "Current size: " << src.size() << endl;
    cout << "Current row: " << current_row << endl;
    cout << "Row energy: " << current_energy << endl;
    cout << endl;

    int current_val = current_row;
    src.copyTo(src_dupe);
    for (int col = 0; col < src.cols; col++) {
      if (col <= 4) {
        src_dupe.at<float>(current_row, col) = 10001;
      } else if (col > 3 && col < src.cols - 5) {
        float w = entropy.at<float>(current_val - 1, col + 1);
        float c = entropy.at<float>(current_val, col + 1);
        float e = entropy.at<float>(current_val + 1, col + 1);
        if (w < c && w < e) {
          current_val = current_val - 1;
        } else if (c < w && c < e) {
          current_val = current_val;
        } else if (e < c && e < w) {
          current_val = current_val + 1;
        }
        src_dupe.at<float>(current_val, col + 1) = 10000;
      } else {
        src_dupe.at<float>(current_val, col) = 10001;
      }
    }

    for (int col = 0; col < src.cols; col++) {
      int r_0 = 0;
      for (int row = 0; row < src.rows - 1; row++) {
        dst.at<float>(row, col) = src.at<float>(r_0, col);
        if (src_dupe.at<float>(row + 1, col) == 10000) {
          r_0 = r_0 + 2;
        } else {
          r_0 = r_0 + 1;
        }
      }
    }
  } else if (direction == 'c') {
    dst.create(src.rows, src.cols - 1, CV_32FC1);
    float current_col = 0;
    for (int col = 4; col < src.cols - 5; col++) {
      if (total_energy < current_energy) {
        current_energy = total_energy;
        current_col = col - 1;
      }
      float current_val = col;
      total_energy = 0;
      for (int row = 4; row < src.rows - 5; row++) {
        float w = entropy.at<float>(row + 1, current_val - 1);
        float c = entropy.at<float>(row + 1, current_val);
        float e = entropy.at<float>(row + 1, current_val + 1);
        if (w < c && w < e) {
          current_val = current_val - 1;
        } else if (c < w && c < e) {
          current_val = current_val;
        } else if (e < c && e < w) {
          current_val = current_val + 1;
        }
        total_energy = total_energy + entropy.at<float>(row + 1, current_val);
      }
    }
    cout << "Current size: " << src.size() << endl;
    cout << "Current column: " << current_col << endl;
    cout << "Column energy: " << current_energy << endl;
    cout << endl;

    int current_val = current_col;
    src.copyTo(src_dupe);
    for (int row = 0; row < src.rows; row++) {
      if (row <= 4) {
        src_dupe.at<float>(row, current_col) = 10001;
      } else if (row > 3 && row < src.rows - 5) {
        float w = entropy.at<float>(row + 1, current_val - 1);
        float c = entropy.at<float>(row + 1, current_val);
        float e = entropy.at<float>(row + 1, current_val + 1);
        if (w < c && w < e) {
          current_val = current_val - 1;
        } else if (c < w && c < e) {
          current_val = current_val;
        } else if (e < c && e < w) {
          current_val = current_val + 1;
        }
        src_dupe.at<float>(row + 1, current_val) = 10000;
      } else {
        src_dupe.at<float>(row, current_val) = 10001;
      }
    }

    for (int row = 0; row < src.rows; row++) {
      int col_2 = 0;
      for (int col = 0; col < src.cols - 1; col++) {
        dst.at<float>(row, col) = src.at<float>(row, col_2);
        if (src_dupe.at<float>(row, col + 1) == 10000) {
          col_2 = col_2 + 2;
        } else {
          col_2 = col_2 + 1;
        }
      }
    }
  }

  return dst;
}

cv::Mat SeamCarving(const cv::Mat& src, int rows, int cols) {
  cv::Mat dst = src;
  for (int idx = 0; idx < rows; idx++) {
    dst = ipcv::IndivSeam(dst, 'r');
  }
  for (int idx = 0; idx < cols; idx++) {
    dst = ipcv::IndivSeam(dst, 'c');
  }
  return dst;
}

}  // namespace ipcv
