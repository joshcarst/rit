/** Implementation file for plot parameter specification
 *
 *  \file plot/plot2d/Params.cpp
 *  \author Carl Salvaggio, Ph.D. (salvaggio@cis.rit.edu)
 *  \date 4 April 2020
 */

#include "Params.h"

#include <iostream>

using namespace std;

namespace plot::plot2d {

Params::Params() {}

Params::Params(std::string x_label, std::string y_label) {
  x_label_ = x_label;
  y_label_ = y_label;
}

Params::~Params() {}

std::string Params::x_label() const { return x_label_; }
void Params::set_x_label(std::string x_label) { x_label_ = x_label; }

std::string Params::y_label() const { return y_label_; }
void Params::set_y_label(std::string y_label) { y_label_ = y_label; }

std::string Params::title() const { return title_; }
void Params::set_title(std::string title) { title_ = title; }

double Params::x_min() const { return x_min_; }
void Params::set_x_min(double x_min) { x_min_ = x_min; }
double Params::x_max() const { return x_max_; }
void Params::set_x_max(double x_max) { x_max_ = x_max; }

double Params::y_min() const { return y_min_; }
void Params::set_y_min(double y_min) { y_min_ = y_min; }
double Params::y_max() const { return y_max_; }
void Params::set_y_max(double y_max) { y_max_ = y_max; }

bool Params::x_tics() const { return x_tics_; }
void Params::set_x_tics(bool x_tics) { x_tics_ = x_tics; }
bool Params::y_tics() const { return y_tics_; }
void Params::set_y_tics(bool y_tics) { y_tics_ = y_tics; }

int Params::linestyle() const { return linestyle_[0]; }
int Params::linestyle(size_t series_idx) const {
  return linestyle_[series_idx % 6];
}
void Params::set_linestyle(int linestyle) { linestyle_[0] = linestyle; }
void Params::set_linestyle(size_t series_idx, int linestyle) {
  linestyle_[series_idx % 6] = linestyle;
}

double Params::xvline() const { return xvline_; }
void Params::set_xvline(double xvline) { xvline_ = xvline; }

std::string Params::destination_filename() const { return dst_filename_; }
void Params::set_destination_filename(std::string dst_filename) {
  dst_filename_ = dst_filename;
}

void Params::Print() const {
  cout << "Params dump:" << endl;
  cout << "  x_label: " << x_label_ << endl;
  cout << "  y_label: " << y_label_ << endl;
  cout << "  title: " << title_ << endl;
  cout << "  x_min: " << x_min_ << endl;
  cout << "  x_max: " << x_max_ << endl;
  if (x_min_ >= x_max_) {
    cout << "  *** Autoscaling being used for the x-axis" << endl;
  }
  cout << "  y_min: " << y_min_ << endl;
  cout << "  y_max: " << y_max_ << endl;
  if (y_min_ >= y_max_) {
    cout << "  *** Autoscaling being used for the y-axis" << endl;
  }
  cout << "  x_tics: " << x_tics_ << endl;
  cout << "  y_tics: " << y_tics_ << endl;
  cout << "  xvline: " << xvline_ << endl;
  cout << "  dst_filename: " << dst_filename_ << endl;
}
}
