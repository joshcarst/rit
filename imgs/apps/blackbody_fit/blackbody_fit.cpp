#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "imgs/radiometry/blackbody_fit/BlackbodyFit.h"
#include "imgs/plot/plot.h"

using namespace std;

int main(int argc, char* argv[]) {
  // Check if file was specified in CLI
  if (argc < 2) {
    throw runtime_error("File not specified, exiting...");
  }

  // Store the file name
  auto filename = argv[1];

  // Check if file exists
  auto exists = filesystem::exists(filename);
  if (exists == false) {
    throw runtime_error("Specified file not found, exiting...");
  }

  // Create vectors to hold comma-separated data
  vector<double> wavelength;
  vector<double> radiance;

  // Open the input file and instantiate the input file stream
  ifstream file(filename);

  // Read each line from the file, create a string stream from each line,
  // parse the string stream into the indivudually delimited string
  // elements, convert the string elements to numeric types, and push each
  // element on to its respective vector
  string line;
  getline(
      file,
      line);  // skipping the header, not a very adaptable solution but whatever
  while (getline(file, line)) {
    stringstream ss(line);
    string str;
    getline(ss, str, ',');
    wavelength.push_back(stod(str));
    getline(ss, str, ',');
    radiance.push_back(stod(str));
  }

  vector<double>
      em_spec;  // making an empty vector to fill with emissivity spectrum
  auto tolerance = 0.0001;  // setting the acceptable error
  auto lower_limit = 7.0;   // setting the lower and upper bounds for the graph
  auto upper_limit = 15.0;
  double derived_temp = radiometry::BlackbodyFit(
      wavelength, radiance, tolerance, lower_limit, upper_limit, em_spec);

  cout << "Derived temperature = " << derived_temp << " [K]" << endl;

  // plot parameters
  plot::plot2d::Params params;
  params.set_x_label("Wavelength (microns)");
  params.set_y_label("Emissivity");
  params.set_linestyle(params.LINES);
  params.set_x_min(lower_limit);
  params.set_x_max(upper_limit);
  params.set_y_min(0.0);
  params.set_y_max(1.05);
  plot::Plot2d(wavelength, em_spec, params);
}