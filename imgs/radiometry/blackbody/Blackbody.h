/*
Blackbody Class by Josh Carstens
help me
*/

#include <cmath>

namespace radiometry
{

class Blackbody
{
public:
  Blackbody(double temperature)
  {
    temperature_ = temperature;
  }

  double temperature() const { return temperature_; }
  void set_temperature(double temperature) { temperature_ = temperature; }

  //storing some common physical constants
  const double h = (6.62607004 * pow(10, -34));
  const double c = (2.99792458 * pow(10, 8));
  const double kb = (1.38064852 * pow(10, -23));
  const double b = (2.897771955 * pow(10, 3)); //this is to the positive 3rd and not negative 3rd because it's in terms of microns instead of meters

  double exitance(double wavelength)
  {
    wavelength *= 0.000001; //converting microns to meters
    return (((2 * M_PI * h * pow(c, 2) * pow(10, -6)) / (pow(wavelength, 5) * (exp((h * c) / (wavelength * kb * temperature_)) - 1)))); //Planck’s equation for radiant exitance
  }

  double radiance(double wavelength) { return (this->exitance(wavelength) / M_PI); }

  double peak() { return (b / temperature_); }

private:
  double temperature_;
};

} // namespace radiometry
