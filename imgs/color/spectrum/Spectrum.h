/** Interface file for color science computations
 *
 * \file imgs/color/spectrum/Spectrum.h
 * \author Josh Carstens, feature creature (jdc3498@rit.edu)
 * \date 20 April 2020
 * \note I'm pretty sure I don't even need at least a quarter of those includes
 */

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

#include "imgs/color/color.h"
#include "imgs/utils/utils.h"
#include "imgs/numerical/interpolation/interpolation.h"
#include <eigen3/Eigen/Dense>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

namespace color {

class Spectrum {
 public:
  Spectrum() {
    wavelengths_ = cie.wavelengths();

    reflectance_ = Eigen::VectorXd::Ones(wavelengths_.size());
  }

  Spectrum(std::string filename, size_t header_lines, size_t label_line) {
    utils::CsvFile f;
    f.set_filename(filename);
    f.set_header_lines(header_lines);
    f.set_label_line(label_line);

    auto wav_vect = f.get_column<double>(1);
    Eigen::Map<Eigen::VectorXd> wavelengths(wav_vect.data(), wav_vect.size());

    wavelengths_ = wavelengths;

    auto ref_vect = f.get_column<double>(2);
    Eigen::Map<Eigen::VectorXd> reflectance(ref_vect.data(), ref_vect.size());

    reflectance_ = reflectance;
  }

  // Getters
  Eigen::VectorXd wavelengths() const { return wavelengths_; }
  Eigen::VectorXd reflectance() const { return reflectance_; }

  Eigen::Vector3d xyz(int RI, int SO);

  Eigen::Vector3d srgb(int RI, int SO, bool scale);

  void patch();

 private:
  // lol I'm probably not supposed to do this
  color::CIE cie;

  Eigen::VectorXd wavelengths_ = Eigen::VectorXd();
  Eigen::VectorXd reflectance_ = Eigen::VectorXd();
  Eigen::Vector3d xyz_ = Eigen::Vector3d();
  Eigen::Vector3d srgb_ = Eigen::Vector3d();
};
}  // namespace color