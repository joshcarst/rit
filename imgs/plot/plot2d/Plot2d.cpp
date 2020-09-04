/** Implementation file for 2D plotting
 *
 *  \file imgs/plot/plot2d/Plot2d.cpp
 *  \author Carl Salvaggio, Ph.D. (salvaggio@cis.rit.edu)
 *  \date 4 April 2020
 */

#include <iostream>

#include "imgs/plot/plot2d/Params.h"

using namespace std;

namespace plot::plot2d {

void set_common_parameters(string& gp_msg, const plot2d::Params& params) {
  if (params.destination_filename() == "") {
    gp_msg += "set terminal x11\n";
  } else {
    string f = params.destination_filename();
    string extension = f.substr(f.length() - 4);
    if (extension == ".eps") {
      gp_msg += "set terminal postscript color\n";
      gp_msg += "set output '" + f + "'\n";
    } else if (extension == ".png") {
      gp_msg += "set terminal png\n";
      gp_msg += "set output '" + f + "'\n";
    } else {
      cerr << "*** ERROR *** ";
      cerr << "Destination file format not supported, using X11" << endl;
      gp_msg += "set terminal x11\n";
    }
  }
  gp_msg += "unset key\n";
  gp_msg += "set xlabel '" + params.x_label() + "'\n";
  gp_msg += "set ylabel '" + params.y_label() + "'\n";
  gp_msg += "set title '" + params.title() + "'\n";
  if (params.x_min() < params.x_max()) {
    gp_msg += "set xrange [" + to_string(params.x_min()) + ":" +
              to_string(params.x_max()) + "]\n";
  }
  if (params.y_min() < params.y_max()) {
    gp_msg += "set yrange [" + to_string(params.y_min()) + ":" +
              to_string(params.y_max()) + "]\n";
  }
  if (params.x_tics()) {
    gp_msg += "set grid xtics\n";
  }
  if (params.y_tics()) {
    gp_msg += "set grid ytics\n";
  }
  if (params.xvline() != 0) {
    gp_msg += "set arrow from " + to_string(params.xvline()) + ", graph 0 to " +
              to_string(params.xvline()) + ", graph 1 nohead\n";
  }
}

}
