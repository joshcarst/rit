/** Application/implementation file for FFT composition
 *
 *  \file apps/examples/fft_display/fft_display.cpp
 *  \author Josh Carstens, Spooked Out (jc@mail.rit.edu)
 *  \date 24 Nov 2020
 *  \note Can't wait to panic twice as hard next semester
 */

// Once again there's a good chance I can remove many of these includes
#include <ctime>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"
#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  // Not sure why I used Boost when there's only one real argument to pass
  bool verbose = false;
  string src_filename = "";

  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
      "source-filename,i", po::value<string>(&src_filename), "source filename");

  po::positional_options_description positional_options;
  positional_options.add("source-filename", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(options)
                .positional(positional_options)
                .run(),
            vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [options] ";
    cout << "source-filename ";
    cout << endl;
    cout << options << endl;
    return EXIT_SUCCESS;
  }

  cv::Mat src = cv::imread(src_filename, cv::IMREAD_UNCHANGED);
  if (!boost::filesystem::exists(src_filename)) {
    cerr << "*** ERROR *** ";
    cerr << "Provided source file does not exist" << endl;
    return EXIT_FAILURE;
  }

  if (verbose) {
    cout << "Source filename: " << src_filename << endl;
    cout << "Size: " << src.size() << endl;
    cout << "Channels: " << src.channels() << endl;
  }

  // Showing the source in int form before converting to float for calculations
  cv::imshow(src_filename, src);
  src.convertTo(src, CV_64F);

  // Taking the DFT of the source, making it logarithmic, and displaying it
  cv::Mat dft_src;
  cv::dft(src, dft_src, cv::DFT_COMPLEX_OUTPUT + cv::DFT_SCALE);
  cv::Mat dft_log = ipcv::DftMagnitude(
      dft_src, ipcv::DFT_MAGNITUDE_LOG + ipcv::DFT_MAGNITUDE_CENTER +
                   ipcv::DFT_MAGNITUDE_NORMALIZE);
  cv::imshow("log|DFT|", dft_log);

  // Getting the raw magnitude values in a separate matrix
  cv::Mat dft_mag = ipcv::DftMagnitude(dft_src);
  // Matrix which holds one magnitude value at a time in its original pixel
  // position. Needs to be complex because it's for calculations and not display
  cv::Mat cc = cv::Mat_<complex<double>>::zeros(src.rows, src.cols);
  // Matrix which will hold the resulting inverse DFTs from the above matrix
  cv::Mat cc_sinusoid = cv::Mat_<double>::zeros(src.rows, src.cols);
  // Matrix we'll keep adding values from cc_sinusoid to to form the final image
  cv::Mat cc_sum = cv::Mat_<double>::zeros(src.rows, src.cols);
  // Matrix that shows all the current components from the original DFT used so
  // far
  cv::Mat cc_fill = cv::Mat_<double>::zeros(src.rows, src.cols);
  cv::Mat_<cv::Point> cc_max;
  // idx is used to skip duplicate magnitude values from being processed
  double idx, min, max;
  idx = 0;
  // Once max reaches 0 that means we've gone through every component
  while (max != 0) {
    cv::Point min_loc, max_loc;
    // Find current highest magnitude component in the DFT
    minMaxLoc(dft_mag, &min, &max, &min_loc, &max_loc);
    cc_max.push_back(max_loc);
    // At the pixel location of the current highest magnitude point, copy that
    // value over to cc (so it's one point of magnitude in a sea of 0s)
    cc.at<complex<double>>(max_loc.y, max_loc.x) =
        dft_src.at<complex<double>>(max_loc.y, max_loc.x);
    // Basic play/pause controls
    auto k = cv::waitKey(100);
    bool paused = false;
    if (k == 'p' || k == 'P') {
      if (paused == true) {
        cv::waitKey(100);
        paused = false;
      } else if (paused == false) {
        cv::waitKey(0);
        paused = true;
      }
    } else if (k == 'q' || k == 'Q') {
      // This seems to only work when it isn't paused, not sure what's up with
      // that
      cv::destroyAllWindows();
      break;
    } else if (k == ' ') {
      // This just straight up doesn't work, it's supposed to advance one frame
      if (paused == true) {
        continue;
      }
    }
    // Copying over current highest magnitude point to current component display
    // window
    cc_fill.at<double>(src.rows / 2, src.cols / 2) =
        dft_log.at<double>(src.rows / 2, src.cols / 2);
    // Operations to do upon a new magnitude value. idx == 0 is an alternate
    // option because the idx kept getting set to 0 on the first loop through
    // for some reason
    if (idx == max || idx == 0) {
      // Calculate the inverse DFT of the single-point cc matrix
      cv::dft(cc, cc_sinusoid, cv::DFT_REAL_OUTPUT + cv::DFT_INVERSE);
      // Add the resulting sinusoid shape to the summed components matrix
      cc_sum += cc_sinusoid;
      // Getting the row and col positions of the max magnitude and its mirrored
      // point
      double x = cc_max(0).x;
      double y = cc_max(0).y;
      double next_x = cc_max(1).x;
      double next_y = cc_max(1).y;
      if (idx == 0) {
        next_x = x;
        next_y = y;
      }
      // Printing positions
      cout << "...at [" << fmod((next_x + (src.rows / 2)), src.rows) << ", "
           << fmod((next_y + (src.cols / 2)), src.cols) << "], ["
           << fmod((x + (src.rows / 2)), src.rows) << ", "
           << fmod((y + (src.cols / 2)), src.cols) << "], |F(u,v)| = " << max
           << endl;
      // This is a bit of a mess, I'm sorry
      cc_fill.at<double>(fmod((y + (src.cols / 2)), src.cols),
                         fmod((x + (src.rows / 2)), src.rows)) =
          dft_log.at<double>(fmod((y + (src.cols / 2)), src.cols),
                             fmod((x + (src.rows / 2)), src.rows));
      cc_fill.at<double>(fmod((next_y + (src.cols / 2)), src.cols),
                         fmod((next_x + (src.rows / 2)), src.rows)) =
          dft_log.at<double>(fmod((next_y + (src.cols / 2)), src.cols),
                             fmod((next_x + (src.rows / 2)), src.rows));
      // Finally showing all windows
      cv::imshow("Current Components (log|DFT|)", cc_fill);
      cv::imshow("Summed Components", static_cast<cv::Mat_<uint8_t>>(cc_sum));
      // Repurposing cc_sinusoid for both display conditions
      cv::imshow("Current Component (+ 128)",
                 static_cast<cv::Mat_<uint8_t>>(cc_sinusoid + 128));
      // Normalizing from 0 to 1 and then scaling up by 255
      cv::normalize(cc_sinusoid, cc_sinusoid, 0, 1, cv::NORM_MINMAX);
      cv::imshow("Current Component (Scaled)",
                 static_cast<cv::Mat_<uint8_t>>(cc_sinusoid * 255));
      cc_max.release();
      // Resetting the cc matrix to 0s so that the next time around it's only
      // one magnitude point once again
      cc = 0;
    }
    // Setting the current highest magnitude value to 0 to cross it off the list
    // on the next loop around
    dft_mag.at<double>(max_loc.y, max_loc.x) = 0;
    // Updating the idx value
    idx = max;
  }
  return EXIT_SUCCESS;
}
