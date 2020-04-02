/** Implementation file blackbody-fit TES
 *
 * \file Blackbody_Fit/BlackbodyFit.cpp
 * \author Josh Carstens,  (jdc3498@rit.edu)
 * \date 1 April 2020
 * \note 
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include "imgs/radiometry/blackbody_fit/BlackbodyFit.h"
// #include "imgs/plot/plot.h"

using namespace std;

namespace radiometry {

double BlackbodyFit(const std::vector<double>& wavelength, const std::vector<double>& radiance){
    vector<double> wl_modified = wavelength;
    vector<double> rad_modified = radiance;

    if (wl_modified.front() > wl_modified.back()) {
        sort(wl_modified.begin(), wl_modified.end());
        sort(rad_modified.begin(), rad_modified.end());
    }

}

}