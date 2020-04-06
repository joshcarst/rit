/** Implementation file blackbody-fit TES
 *
 * \file Blackbody_Fit/BlackbodyFit.cpp
 * \author Josh Carstens, creator of ants (jdc3498@rit.edu)
 * \date 1 April 2020
 * \note I'm also the destroyer of ants
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include "imgs/radiometry/blackbody_fit/BlackbodyFit.h"
#include "imgs/radiometry/blackbody/Blackbody.h"

using namespace std;

namespace radiometry {

double BlackbodyFit(vector<double>& wavelength,
                    vector<double>& radiance, const double tolerance,
                    const double lower_limit, const double upper_limit,
                    vector<double>& em_spec) {
  // checking if wavelength is in ascending order and reversing the vectors if
  // it isn't
  bool reversed = false;
  if (wavelength.front() > wavelength.back()) {
    reverse(wavelength.begin(), wavelength.end());
    reverse(radiance.begin(), radiance.end());
    reversed = true;
  }

  // getting the indexes of our vectors at the desired upper and lower bounds
  auto w_lower =
      distance(wavelength.begin(),
               lower_bound(wavelength.begin(), wavelength.end(), lower_limit));
  auto w_upper =
      distance(wavelength.begin(),
               upper_bound(wavelength.begin(), wavelength.end(), upper_limit));

  double temperature = 0.0;  // setting an initial temperature

  radiometry::Blackbody steve(temperature);  // making Steve

  double tolerance_calc = 1000;  // setting our initial test tolerance
  while (tolerance_calc >
         tolerance) {  // checking+repeating if our test tolerance hasn't
                       // reached our acceptable error
    for (double idx = w_lower; idx <= w_upper;
         idx++) {  // looping within our bounds
      double difference =
          (steve.radiance(wavelength[idx]) -
           radiance[idx]);  // getting the difference between blackbody radiance
                            // and measured radiance
      if (difference < 0) {
        temperature += tolerance_calc;  // resetting the temperature value if
                                        // our blackbody radiance goes too low
        steve.set_temperature(temperature);
        tolerance_calc /= 10;  // decreasing the next increment amount
      }
    }
    temperature -= tolerance_calc;
    steve.set_temperature(temperature);
  }

  // putting the vectors back if they got reversed
  if (reversed == true) {
    reverse(wavelength.begin(), wavelength.end());
    reverse(radiance.begin(), radiance.end());
    reversed = false;
  }

  // actually doing our blackbody-fit calculation for spectral emissivity
  for (double idx = 0; idx <= radiance.size(); idx++) {
    em_spec.push_back(radiance[idx] / steve.radiance(wavelength[idx]));
  }

  return temperature;
}
}  // namespace radiometry