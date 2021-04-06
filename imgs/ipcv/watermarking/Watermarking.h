#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace ipcv {


int watermarking(const cv::Mat& src, cv::Mat& dst, cv::Mat& key, cv::Mat& decode);
		
}
