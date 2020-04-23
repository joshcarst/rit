/** Implementation file for color science computations
 *
 * \file imgs/color/spectrum/Spectrum.cpp
 * \author Josh Carstens, can drift his car (jdc3498@rit.edu)
 * \date 20 April 2020
 * \note lol the comments in all three of these files may be out of date because
 * I did some vast last minute changes without the time to write new ones
 */

#include "imgs/color/color.h"

using namespace std;

namespace color {

Eigen::Vector3d Spectrum::xyz(int RI, int SO) {
  double x, y, z;

  // Getting some vectors from the CIE class
  Eigen::VectorXd wavelengths = cie.wavelengths();
  Eigen::VectorXd illuminant = cie.reference_illuminant(RI);
  Eigen::VectorXd xbar = cie.xbar(SO);
  Eigen::VectorXd ybar = cie.ybar(SO);
  Eigen::VectorXd zbar = cie.zbar(SO);

  // We interpolate if there aren't the same number of wavelength values,
  // although this might not catch differing wavelength spans in some cases
  if (wavelengths_.size() != wavelengths.size()) {
    // Making interpolated versions based off of our wavelength vector
    Eigen::VectorXd ill_interp =
        numerical::Interp1(wavelengths_, wavelengths, illuminant);
    Eigen::VectorXd xbar_interp =
        numerical::Interp1(wavelengths_, wavelengths, xbar);
    Eigen::VectorXd ybar_interp =
        numerical::Interp1(wavelengths_, wavelengths, ybar);
    Eigen::VectorXd zbar_interp =
        numerical::Interp1(wavelengths_, wavelengths, zbar);

    // Calculating the normalizing factor
    Eigen::ArrayXd norm = ill_interp.array() * ybar_interp.array();
    double n = norm.sum();

    // Multiplying vectors before taking their summation
    Eigen::ArrayXd x_unsummed =
        ill_interp.array() * reflectance_.array() * xbar_interp.array();
    Eigen::ArrayXd y_unsummed =
        ill_interp.array() * reflectance_.array() * ybar_interp.array();
    Eigen::ArrayXd z_unsummed =
        ill_interp.array() * reflectance_.array() * zbar_interp.array();

    // Taking summation and calculating final values for XYZ
    x = x_unsummed.sum() / n;
    y = y_unsummed.sum() / n;
    z = z_unsummed.sum() / n;
  } else {
    // Calculating the normalizing factor
    Eigen::ArrayXd norm = illuminant.array() * ybar.array();
    double n = norm.sum();

    // Multiplying vectors before taking their summation
    Eigen::ArrayXd x_unsummed =
        illuminant.array() * reflectance_.array() * xbar.array();
    Eigen::ArrayXd y_unsummed =
        illuminant.array() * reflectance_.array() * ybar.array();
    Eigen::ArrayXd z_unsummed =
        illuminant.array() * reflectance_.array() * zbar.array();

    // Taking summation and calculating final values for XYZ
    x = x_unsummed.sum() / n;
    y = y_unsummed.sum() / n;
    z = z_unsummed.sum() / n;
  }

  // Putting them in a vector
  Eigen::Vector3d xyz;
  xyz << x, y, z;

  // Saving to the private section
  xyz_ = xyz;

  return xyz;
}

Eigen::Vector3d Spectrum::srgb(int RI, int SO, bool scale) {
  Eigen::Vector3d xyz = xyz_;

  // Doing the transformation as described in the project doc
  Eigen::Matrix3d transform;
  transform << 3.2404542, -1.5371385, -0.4985314, -0.9692660, 1.8760108,
      0.0415560, 0.0556434, -0.2040259, 1.0572252;
  if (scale == true) {
    xyz *= 0.01;
  }

  Eigen::Vector3d rgb_linear = transform * xyz;

  Eigen::Vector3d srgb_calc;

  // The ol piecewise non-linear transformation
  for (int idx = 0; idx < rgb_linear.size(); idx++) {
    if (rgb_linear(idx) <= 0.0031308) {
      srgb_calc(idx) = 12.92 * rgb_linear(idx);
    } else {
      srgb_calc(idx) = 1.055 * std::pow(rgb_linear(idx), (1 / 2.4)) - 0.055;
    }
    // Clipping the values that exceed or go below the range [0, 1]
    if (srgb_calc(idx) > 1.0) {
      srgb_calc(idx) = 1.0;
    }
    if (srgb_calc(idx) < 0.0) {
      srgb_calc(idx) = 0.0;
    }
  }

  // Saving to the private section
  srgb_ = srgb_calc;

  return srgb_calc;
}

void Spectrum::patch() {
  // Scaling that bad boy to 255
  Eigen::Vector3d srgb = srgb_ * 255;

  // Making the initial mat with size and data type as specified in project doc
  cv::Mat color_patch(256, 256, CV_8UC3, cv::Scalar(srgb[2], srgb[1], srgb[0]));

  // Making a string for the window name, I don't think this is the most
  // efficient but it's kind of readable
  std::string window_name = "(";
  window_name += std::to_string(static_cast<int>(srgb[0]));
  window_name += ", ";
  window_name += std::to_string(static_cast<int>(srgb[1]));
  window_name += ", ";
  window_name += std::to_string(static_cast<int>(srgb[2]));
  window_name += ")";

  // Displaying the window
  cv::imshow(window_name, color_patch);
  cv::waitKey(0);
  auto k = cv::waitKey(0);
  if (k == 27) {  // Escape key
    cv::destroyWindow(window_name);
  }
}
}  // namespace color