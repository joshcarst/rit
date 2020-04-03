/** Blackbody Temperature binary search test application
 *
 * \file Blackbody_Temperature/blackbody_temperature.cpp
 * \author Josh Carstens, low on fuel (jdc3498@rit.edu)
 * \date 25 Februrary 2020
 * \note don't blink, they'll come near
 */

// including headers and libraries
#include <iostream>
#include <imgs/radiometry/blackbody/Blackbody.h>
#include <imgs/radiometry/blackbody_temperature/BlackbodyTemperature.h>

using namespace std;

int main() {
  // making all my doubles on one line because I'm optimal
  double rad_search, wavelength, tolerance;
  // gathering variables
  cout << "Radiance [W/m^2/sr/micron]: ";
  cin >> rad_search;
  cout << "Wavelength [micron]: ";
  cin >> wavelength;
  cout << "Acceptable tolerance [K]: ";
  cin >> tolerance;
  cout << "Temperature = "
       << Temperature::CalcTemp(rad_search, wavelength, tolerance) << " [K]"
       << endl;
}
