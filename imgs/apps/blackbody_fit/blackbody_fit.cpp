#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include "imgs/radiometry/blackbody_fit/BlackbodyFit.h"

using namespace std;

int main(int argc, char* argv[]) {
  auto filename = argv[1];

  // Check if file exists
  auto exists = std::filesystem::exists(filename);
  if (exists) {
    auto msg = "Found specified file";
    cout << msg << endl;
  } else {
    auto msg = "Specified file not found, exiting ...";
    throw std::runtime_error(msg);
  }

  // Create vectors to hold comma-separated data
  std::vector<double> wavelength;
  std::vector<double> radiance;

  // Open the input file and instantiate the input file stream
  std::ifstream file(filename);

  // Read each line from the file, create a string stream from each line,
  // parse the string stream into the indivudually delimited string
  // elements, convert the string elements to numeric types, and push each
  // element on to its respective vector 
  std::string line;
  std::getline(file, line); // skipping the first line as described by Meg
  while (std::getline(file, line)) {
    std::stringstream ss(line);
    std::string str;
    std::getline(ss, str, ',');
    wavelength.push_back(stod(str));
    std::getline(ss, str, ',');
    radiance.push_back(stod(str));
  }

  // Send each vector to the standard output for examination
  cout << "Column 1" << endl;
  for (auto element : x) {
    cout << element << endl;
  }
  cout << endl;
  cout << "Column 2" << endl;
  for (auto element : y) {
    cout << element << endl;
  }

  double derived_temp = radiometry::BlackbodyFit(wavelength, radiance);
}