// Again, not sure how many of these includes are needed
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "imgs/color/color.h"
#include "imgs/numerical/interpolation/interpolation.h"
#include "imgs/plot/plot.h"
#include <eigen3/Eigen/Dense>

using namespace std;

int main(int argc, char* argv[]) {
  // I create this so we can use the enumerations and get the vectors when we
  // graph
  color::CIE cie;

  // Putting the variables up near the top for ease of access. These could be
  // turned into command line arguments
  int RI = color::CIE::ReferenceIlluminant::a;
  int SO = color::CIE::StandardObserver::two_deg;

  size_t header_lines = 1;
  size_t label_line = 1;

  // One of my biggest problems is how we have basically all the same code
  // twice, I didn't think about how creating objects in an if statement would
  // limit them to the if statement's code when I began the project
  if (argc < 2) {
    color::Spectrum steve;

    Eigen::Vector3d xyz = steve.xyz(RI, SO);

    // We don't need to scale because the reflectance is just a buncha 1s
    bool scale = false;

    Eigen::Vector3d srgb = steve.srgb(RI, SO, scale);

    cout << "X, Y, Z = [" << xyz[0] << ", " << xyz[1] << ", " << xyz[2] << "]"
         << endl;
    cout << "sRGB = [" << srgb[0] << ", " << srgb[1] << ", " << srgb[2] << "]"
         << endl;

    plot::plot2d::Params params;
    params.set_x_label("Wavelength [nm]");
    params.set_y_label("Reflectance");
    params.set_linestyle(params.LINES);
    params.set_y_min(0.0);
    params.set_y_max(2.0);
    plot::Plot2d(steve.wavelengths(), steve.reflectance(), params);

    // Getting vectors from the cie object to graph
    Eigen::VectorXd graph2_x = cie.wavelengths();
    Eigen::VectorXd graph2_y = cie.reference_illuminant(RI);

    params.set_x_label("Wavelength [nm]");
    params.set_y_label("Spectral Power");
    params.set_linestyle(params.LINES);
    params.set_y_min(0.0);
    params.set_y_max(300.0);
    plot::Plot2d(graph2_x, graph2_y, params);

    // These mfing three lines of code are why I submitted files 7 minutes late
    // (besides the insane compile times of course)
    Eigen::MatrixXd graph3_y = cie.standard_observer(SO);
    Eigen::MatrixXd graph3_x(graph3_y.rows(), graph3_y.cols());
    graph3_x << graph2_x.array(), graph2_x.array(), graph2_x.array();

    // Graff
    params.set_x_label("Wavelength [nm]");
    params.set_y_label("Spectral Sensitivity");
    params.set_linestyle(params.LINES);
    params.set_y_min(0.0);
    params.set_y_max(2.05);
    plot::Plot2d(graph3_x, graph3_y, params);

    // Finally, scaling the srgb values by 255 and displaying in a window. This
    // seems to produce a slightly off-white square, not sure if that's intended
    // or a symptom of unneeded interpolation
    steve.patch();

  } else {
    // Storing the filename
    std::string filename = argv[1];

    // Checking if the file exists
    auto exists = filesystem::exists(filename);
    if (exists == false) {
      throw runtime_error("Specified file not found, exiting...");
    }

    color::Spectrum steve(filename, header_lines, label_line);

    Eigen::Vector3d xyz = steve.xyz(RI, SO);

    // I suppose this should technically be moved up to the global variables
    // since a csv file with non-scaled values could be used
    bool scale = true;

    Eigen::Vector3d srgb = steve.srgb(RI, SO, scale);

    cout << "X, Y, Z = [" << xyz[0] << ", " << xyz[1] << ", " << xyz[2] << "]"
         << endl;
    cout << "sRGB = [" << srgb[0] << ", " << srgb[1] << ", " << srgb[2] << "]"
         << endl;

    // Doing all the plotting functions again, this is basically exactly the
    // same as above
    plot::plot2d::Params params;
    params.set_x_label("Wavelength [nm]");
    params.set_y_label("Reflectance");
    params.set_linestyle(params.LINES);
    params.set_y_min(0.0);
    params.set_y_max(100.0);
    plot::Plot2d(steve.wavelengths(), steve.reflectance(), params);

    Eigen::VectorXd graph2_x = cie.wavelengths();
    Eigen::VectorXd graph2_y = cie.reference_illuminant(RI);

    params.set_x_label("Wavelength [nm]");
    params.set_y_label("Spectral Power");
    params.set_linestyle(params.LINES);
    params.set_y_min(0.0);
    params.set_y_max(300.0);
    plot::Plot2d(graph2_x, graph2_y, params);

    Eigen::MatrixXd graph3_y = cie.standard_observer(SO);
    Eigen::MatrixXd graph3_x(graph3_y.rows(), graph3_y.cols());
    graph3_x << graph2_x.array(), graph2_x.array(), graph2_x.array();

    params.set_x_label("Wavelength [nm]");
    params.set_y_label("Spectral Sensitivity");
    params.set_linestyle(params.LINES);
    params.set_y_min(0.0);
    params.set_y_max(2.05);
    plot::Plot2d(graph3_x, graph3_y, params);

    steve.patch();
  }
}