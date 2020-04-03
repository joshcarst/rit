/** Blackbody Temperature binary search implementation
 *
 * \file Blackbody_Temperature/BlackbodyTemperature.cpp
 * \author Josh Carstens, space ghost (jdc3498@rit.edu)
 * \date 25 Februrary 2020
 * \note vaporwave is the best genre
 */

// more headers and libraries
#include <cmath>
#include <vector>
#include "imgs/radiometry/blackbody_temperature/BlackbodyTemperature.h"
#include "imgs/radiometry/blackbody/Blackbody.h"

namespace Temperature {

// passing our variables from the application to the implementation
double CalcTemp(double rad_search, double wavelength, double tolerance) {
  // a vector with sequential values from 1-6000
  // I was initially going from 0-6000 but then I was getting 301 K instead of
  // 300 K
  std::vector<double> temp_search;
  for (size_t idx = 0; idx < 6000; idx++) {
    temp_search.push_back(idx + 1);
  }

  // basically doing what it says on the flow chart
  double low = 0;
  double high = temp_search.size() - 1;
  double middle = (low + high) / 2;
  double range = high - low;
  while (range > tolerance) {
    // calling our blackbody function for the current middle temperature
    radiometry::Blackbody steve(temp_search[middle]);
    // comparing the radiance we're searching for to the radiance at the middle
    // temperature
    if (rad_search < steve.radiance(wavelength)) {
      high = middle;
    } else if (rad_search > steve.radiance(wavelength)) {
      low = middle;
    } else {
      break;
    }
    // redefining middle and range with our new values
    middle = (low + high) / 2;
    range = high - low;
  }
  // sending the number we have after the loop back to the application file
  return middle;
}
}  // namespace Temperature