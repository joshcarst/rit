/** Application/implementation file for character recognition
 *
 *  \file ipcv/spatial_filtering/Filter2D.cpp
 *  \author Josh Carstens, Spooked Out (jc@mail.rit.edu)
 *  \date 21 Oct 2020
 *  \note I may have used a late credit for this one but
 * Oneohtrix Point Never has a new album coming out so I'm okay
 */

// Not even sure how many of these I need at this point
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

#include "imgs/ipcv/utils/Utils.h"
#include "imgs/plot/plot.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  // Default variables
  bool verbose = false;
  string src_filename = "";
  double threshold = 0.97;
  string filter_type = "matching";
  string character_path = "";
  string extension = ".tif";

  // Pretty sure all this Boost stuff doesn't need comments
  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
      "verbose,v", po::bool_switch(&verbose), "verbose [default is silent]")(
      "source-filename,i", po::value<string>(&src_filename), "source filename")(
      "filter-type,f", po::value<string>(&filter_type),
      "filter type (matching|vector) [default is matching]")(
      "threshold,t", po::value<double>(&threshold),
      "threshold [default is 0.97]")("character-path,c",
                                     po::value<string>(&character_path),
                                     "character set path");

  po::positional_options_description positional_options;
  positional_options.add("source-filename", 1);
  positional_options.add("character-path", 1);

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
    cout << "character-path ";
    cout << endl;
    cout << options << endl;
    return EXIT_SUCCESS;
  }

  if (filter_type != "matching" && filter_type != "vector") {
    cerr << "*** ERROR *** ";
    cerr << "Provided filter type is not supported" << endl;
    return EXIT_FAILURE;
  }

  // Reading in the source image
  // Like you tend to do in IPCV class
  cv::Mat src = cv::imread(src_filename, cv::IMREAD_GRAYSCALE);
  if (!boost::filesystem::exists(src_filename)) {
    cerr << "*** ERROR *** ";
    cerr << "Provided source file does not exists" << endl;
    return EXIT_FAILURE;
  }

  if (!boost::filesystem::exists(character_path)) {
    cerr << "*** ERROR *** ";
    cerr << "Provided character path does not exists" << endl;
    return EXIT_FAILURE;
  }

  // Making a list of all of the character files like in Carl's list_files
  const auto& directory_entries =
      filesystem::directory_iterator(character_path);

  vector<string> character_paths;
  for (const auto& directory_entry : directory_entries) {
    if (directory_entry.path().extension() == extension) {
      character_paths.push_back(directory_entry.path());
    }
  }
  sort(character_paths.begin(), character_paths.end());

  if (verbose) {
    cout << "Source filename: " << src_filename << endl;
    cout << "Size: " << src.size() << endl;
    cout << "Channels: " << src.channels() << endl;
    cout << "Filter type: " << filter_type << endl;
    cout << "Threshold: " << threshold << endl;
    cout << "Character set path: " << character_path << endl;
  }

  clock_t startTime = clock();

  // This is pretty much the implementation part

  // Two-row matrix: First row is what letter it be, second row is how many
  // of that letter we sniped
  cv::Mat match(2, character_paths.size(), CV_32FC1, cv::Scalar(0));
  // What we're gonna be storing the scanned letters in eventually
  vector<char> text_output(src.total());
  // Set to keep track of spaces
  bool char_exists = false;

  // Doing a ~ inverts the image, pretty cool n concise
  src = ~src;
  // Converting to float so we can have normalized decimal values
  src.convertTo(src, CV_32F);
  src /= 255;
  // Looping through every character in the specified path
  for (size_t idx = 0; idx < character_paths.size(); idx++) {
    // Reading in that character
    cv::Mat current_char =
        cv::imread(character_paths[idx], cv::IMREAD_GRAYSCALE);
    // Saving the filename of that character to the corresponding table position
    match.at<float>(0, idx) =
        stoi(character_paths[idx].substr(character_paths[idx].length() - 6, 2),
             nullptr);
    // Doing the same float prep operations as we did to the src
    current_char = ~current_char;
    current_char.convertTo(current_char, CV_32F);
    current_char /= 255;
    // Normalizing according to the histogram sum of each character
    if (filter_type == "matching") {
      current_char /= cv::sum(current_char)[0];
    }
    // Looping through the src to scan for letters
    for (int row = 0; row < src.rows; row++) {
      for (int col = 0; col < src.cols; col++) {
        // Making sure we don't go over the edge of the frame
        if (row + current_char.rows < src.rows &&
            col + current_char.cols < src.cols) {
          // Cropping the src into kernel-sized boxes
          cv::Rect cropped_char_area(col, row, current_char.cols,
                                     current_char.rows);
          cv::Mat cropped_char = src(cropped_char_area);
          // Summing the cropped boxes to see if they're blank
          double cropped_char_sum = cv::sum(cropped_char)[0];
          // Designating a space if it's blank and updating the bool to reflect
          if (char_exists == true && cropped_char_sum == 0) {
            text_output[row * src.cols + col] = ' ';
            char_exists = false;
          }
          // The cosine angle vector something or other method
          if (filter_type == "vector") {
            // Taking the dot products to find the magnitude of each character
            double char_dot = sqrt(current_char.dot(current_char));
            double cropped_char_dot = sqrt(cropped_char.dot(cropped_char));
            // idk math
            double vector_angle =
                cropped_char.dot(current_char) / (char_dot * cropped_char_dot);
            // Assigning corresponding characters from matching table to the
            // text output and adding a count to the table if it's within threshold
            if (vector_angle > threshold) {
              text_output[row * src.cols + col] =
                  match.at<float>(0, idx);
              match.at<float>(1, idx) += 1;
              char_exists = true;
            }
          // The real scary histogram matching method
          // In the Halloween spirit
          } else {
            double sum = 0;
            // Normalizing each character box but making sure to not div by 0
            if (cropped_char_sum != 0) {
              cropped_char /= cropped_char_sum;
            }
            // Looping through every pixel in each kernel
            for (int row_2 = 0; row_2 < current_char.rows; row_2++) {
              for (int col_2 = 0; col_2 < current_char.cols; col_2++) {
                // Looking for filled in pixels
                if (current_char.at<float>(row_2, col_2) > 0 &&
                    cropped_char.at<float>(row_2, col_2) > 0) {
                  // Adding to the counter if they exist
                  sum += current_char.at<float>(row_2, col_2);
                  sum += cropped_char.at<float>(row_2, col_2);
                }
              }
            }
            // Once again assigning text outputs and counting occurrences
            if (sum - 1 >= threshold) {
              text_output[row * src.cols + col] =
                  match.at<float>(0, idx);
              match.at<float>(1, idx) += 1;
              char_exists = true;
            }
          }
        }
      }
    }
    // Printing occurrences as we go
    if (verbose) {
    char print_char = match.at<float>(0, idx);
    cout << print_char << ": " << match.at<float>(1, idx) << endl;
    }
  }

  clock_t endTime = clock();

  if (verbose) {
    cout << "Elapsed time: "
         << (endTime - startTime) / static_cast<double>(CLOCKS_PER_SEC)
         << " [s]" << endl;
  }

  // Idk I guess plot2d doesn't like to print rows of a cv::Mat
  vector<float> match_x;
  match.row(0).copyTo(match_x);
  vector<float> match_y;
  match.row(1).copyTo(match_y);
  plot::plot2d::Params params;
  params.set_x_label("Character");
  params.set_y_label("Frequency");
  params.set_x_min(39);
  params.set_x_max(90);
  plot::plot2d::Plot2d(match_x, match_y, params);

  // Printing the final text output
  for (size_t idx = 0; idx < text_output.size(); idx++) {
    cout << text_output[idx];
  }
  cout << endl;
}
