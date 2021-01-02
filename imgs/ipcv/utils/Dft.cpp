/** Implementation file for computing the DFT of a vector
 *
 *  \file ipcv/utils/Dft.cpp
 *  \author Josh Carstens, Executive Producer of Shark Tale (jc@mail.rit.edu)
 *  \date 14 November 2020
 *  \note I'm thinking for Christmas ima ask for my car to by vinyl wrapped to
 * look like a giant hunk of granite. What do you think
 */

#include "Dft.h"

using namespace std;

namespace ipcv {

cv::Mat Dft(cv::Mat f, const int flag) {
  const std::complex<double> i(0, 1);

  // Create the output cv::Mat (double-precision complex)
  double N = f.rows;
  cv::Mat F(N, 1, CV_64FC2);

  // Determine if the input cv::Mat is already complex (CV_64FC2).
  // If it is, create a shared pointer to the original data so no new
  // memory is required.
  // If it is not, create a local complex version of the input for
  // this computation.
  // In either case, the local version of the data to be transformed
  // will be referred to by the variable f_complex.
  cv::Mat f_complex;

  uchar depth = f.type() & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (f.type() >> CV_CN_SHIFT);
  if (depth == CV_64F && chans == 2) {
    // Create a shared pointer with the original cv::Mat
    f_complex = f;
  } else {
    // Convert the input cv::Mat to CV_64F representing the real part
    auto real = cv::Mat(f.size(), CV_64F);
    f.convertTo(real, CV_64F);

    // Create a CV_64F cv::Mat representing the imagingary part
    auto imag = cv::Mat(f.size(), CV_64F, cv::Scalar(0));

    // Merge the real and imagingary to form a CV_64FC2 cv::Mat
    std::vector<cv::Mat> channels;
    channels.push_back(real);
    channels.push_back(imag);
    cv::merge(channels, f_complex);
  }

  if (flag == 2) {
    // Inverse case
    // Loop through each value from the input vector
    for (double u = 0; u < N; u++) {
      // Target value to add to in the summation
      complex<double> output_sum(0, 0);
      // The summation
      for (double x = 0; x < N; x++) {
        // Pretty much the magic equation from the slides idk how math works
        output_sum +=
            f_complex.at<complex<double>>(x) *
            (cos(2.0 * M_PI * u * x / N) - i * sin(2.0 * M_PI * u * x / N));
      }
      // Assigning the final sum to the output vector and scaling by N
      F.at<complex<double>>(u) = output_sum / N;
    }
  } else {
    // Scaling case
    // Pretty much the same as above but adding the sin component and not
    // scaling
    for (double x = 0; x < N; x++) {
      complex<double> output_sum(0, 0);
      for (double u = 0; u < N; u++) {
        output_sum +=
            f_complex.at<complex<double>>(u) *
            (cos(2.0 * M_PI * u * x / N) + i * sin(2.0 * M_PI * u * x / N));
      }
      F.at<complex<double>>(x) = output_sum;
    }
  }

  return F;
}
}  // namespace ipcv
