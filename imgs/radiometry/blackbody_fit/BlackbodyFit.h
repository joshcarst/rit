/** Interface file for blackbody-fit TES
 *
 * \file Blackbody_Fit/BlackbodyFit.h
 * \author Josh Carstens, big dumpy (jdc3498@rit.edu)
 * \date 1 April 2020
 * \note I watched the Big Short recently, good movie
 */

namespace radiometry {
double BlackbodyFit(std::vector<double>& wavelength,
                    std::vector<double>& radiance, const double tolerance,
                    const double lower_limit, const double upper_limit,
                    std::vector<double>& em_spec);
}  // namespace radiometry