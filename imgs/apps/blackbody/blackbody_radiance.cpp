/*
Blackbody Class test application by Josh Carstens
ok lets go
*/
#include <iostream>

#include "imgs/radiometry/blackbody/Blackbody.h"

using namespace std;

int main()
{
    double temperature = 200; //starting off with some test values
    double wavelength = 8; //accepting in units of microns even though it'll be converted to meters in the header file

    radiometry::Blackbody steve(temperature);

    cout << "Temperature = " << steve.temperature() << " [K]" << endl;
    cout << "Wavelength = " << wavelength << " [micron]" << endl;
    cout << "Exitance = " << steve.exitance(wavelength) << " [W/m^2/micron]" << endl;
    cout << "Radiance = " << steve.radiance(wavelength) << " [W/m^2/sr/micron]" << endl;
    cout << "Peak Wavelength = " << steve.peak() << " [micron]" << endl;
    cout << endl;

    steve.set_temperature(300); //using a setter for the temperature but not the wavelength bc that's how blackbodies be i guess
    wavelength = 10;

    cout << "Temperature = " << steve.temperature() << " [K]" << endl;
    cout << "Wavelength = " << wavelength << " [micron]" << endl;
    cout << "Exitance = " << steve.exitance(wavelength) << " [W/m^2/micron]" << endl;
    cout << "Radiance = " << steve.radiance(wavelength) << " [W/m^2/sr/micron]" << endl;
    cout << "Peak Wavelength = " << steve.peak() << " [micron]" << endl;
}