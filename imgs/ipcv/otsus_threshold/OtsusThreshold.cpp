/** Implementation file for finding Otsu's threshold
 *
 *  \file ipcv/otsus_threshold/OtsusThreshold.cpp
 *  \author Josh Carstens (jc@mail.rit.edu)
 *  \date 12 Sep 2020
 *  \note I'm absolutely horrified I used more for loops than necessary
 * My calculation time is .009s instead of Carl's .008s help
 * How can you do this in 6 lines in Python or whatever wtf
 * Also yes that's my real email
 * I know I'm pretty cool
 * http://start.rit.edu/MailPreference
 */

#include "OtsusThreshold.h"

#include <iostream>

#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace ipcv {

/** Find Otsu's threshold for each channel of a 3-channel (color) image
 *
 *  \param[in] src          source cv::Mat of CV_8UC3
 *  \param[out] threshold   threshold values for each channel of a 3-channel
 *                          color image in cv::Vec3b
 */
bool OtsusThreshold(const cv::Mat& src, cv::Vec3b& threshold) {
  threshold = cv::Vec3b();

  // Initial PDF calculation
  cv::Mat_<int> src_hist;
  ipcv::Histogram(src, src_hist);
  cv::Mat_<double> src_pdf;
  ipcv::HistogramToPdf(src_hist, src_pdf);

  for (int channel_idx = 0; channel_idx < src_pdf.rows; channel_idx++) {
    // Loops for finding the first and last non-zero DC's
    // Re-purposed from the histogram enhancement project
    int low;
    for (int brightness = 0; brightness < src_pdf.cols; brightness++) {
      if (src_pdf.at<double>(channel_idx, brightness) != 0) {
        low = brightness;
        break;
      }
    }

    int high;
    for (int brightness = src_pdf.cols - 1; brightness >= 0; brightness--) {
      if (src_pdf.at<double>(channel_idx, brightness) != 0) {
        high = brightness;
        break;
      }
    }

    // Loop to calculate the mean of every brightness value in the whole channel
    // It'd be cool to figure out how to use cv::sum for this to avoid a loop
    double total_mean = 0;
    for (int brightness = low; brightness < high; brightness++) {
      total_mean += brightness * src_pdf.at<double>(channel_idx, brightness);
    }

    // I'm sure these aren't the variable names everyone else used
    // Initializing them out here instead of in the loop to save time
    // Because I think that's how it works?
    double k_mean, c0_mean, c1_mean, k_sum, c0_sum, c1_sum;

    // Here we only want an array of variance values that's as big as the range
    // between high and low values because that's all we're doing calculations
    // for
    cv::Mat variance = cv::Mat_<double>::zeros(1, high - low);
    // Iterating through every possible k value
    for (int k = low; k < high; k++) {
      k_sum = 0;
      k_mean = 0;
      // Calculating the sum of every PDF value below the current k value
      // As well as the mean of the brightness values below k
      for (int k_idx = low; k_idx < k; k_idx++) {
        k_sum += src_pdf.at<double>(channel_idx, k_idx);
        k_mean += k_idx * src_pdf.at<double>(channel_idx, k_idx);
      }
      // Just doing what Otsu tells me
      c0_mean = k_mean / k_sum;
      c1_mean = (total_mean - k_mean) / (1 - k_sum);
      c0_sum = k_sum;
      c1_sum = 1 - k_sum;
      // Subtracting the low value from the k value we're writing to so it
      // starts at 0
      variance.at<double>(k - low) =
          c0_sum * c1_sum * (c1_mean - c0_mean) * (c1_mean - c0_mean);
    }
    // It's kind of annoying they make you calculate the min and max values and
    // the min location when all I want is the max location
    double min, max;
    cv::Point min_loc, max_loc;
    cv::minMaxLoc(variance, &min, &max, &min_loc, &max_loc);
    // Adding the low value to the max location so it's properly aligned between
    // 0 and 255
    threshold[channel_idx] = max_loc.x + low;
  }

  return true;
}
}  // namespace ipcv
