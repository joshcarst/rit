// \imgs\ipcv\seam_carving\SeamCarve.h

#pragma once

#include <string>

#include <opencv2/core.hpp>

using namespace std;

namespace ipcv {
cv::Mat IndivSeam(const cv::Mat& src, char direction);
cv::Mat SeamCarving(const cv::Mat &src, int rows, int cols);
}
