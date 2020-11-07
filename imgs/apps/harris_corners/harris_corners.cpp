#include <ctime>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include "opencv2/imgproc.hpp"

#include "imgs/ipcv/corners/Corners.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  bool verbose = false;
  string src_filename = "";
  float sigma = 1;
  float k = 0.04;

  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
      "verbose,v", po::bool_switch(&verbose), "verbose [default is silent]")(
      "source-filename,i", po::value<string>(&src_filename), "source filename")(
      "sigma,s", po::value<float>(&sigma),
      "standard deviation for blur [default is 1]")(
      "parameter,k", po::value<float>(&k),
      "free parameter for Harris response [default is 0.04]");

  po::positional_options_description positional_options;
  positional_options.add("source-filename", -1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
                .options(options)
                .positional(positional_options)
                .run(),
            vm);
  po::notify(vm);

  if (vm.count("help")) {
    cout << "Usage: " << argv[0] << " [options] source-filename" << endl;
    cout << options << endl;
    return EXIT_SUCCESS;
  }

  if (!boost::filesystem::exists(src_filename)) {
    cerr << "*** ERROR *** ";
    cerr << "Provided source file does not exists" << endl;
    return EXIT_FAILURE;
  }

  cv::Mat src = cv::imread(src_filename, cv::IMREAD_COLOR);

  if (verbose) {
    cout << "Source filename: " << src_filename << endl;
    cout << "Size: " << src.size() << endl;
    cout << "Channels: " << src.channels() << endl;
    cout << "Sigma: " << sigma << endl;
    cout << "k: " << k << endl;
  }

  clock_t startTime = clock();

  bool status = false;
  cv::Mat dst;
  status = ipcv::Harris(src, dst, sigma, k);

  // Not sure how much of this should be done in the app file vs the implementation but we doing it

  // Making a copy of dst exactly as it comes from ipcv::Harris for outputting the coordinates later
  cv::Mat dst_dupe;
  dst.copyTo(dst_dupe);

  // Setting dst to three-channel ints so we can put red dots on there
  cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);
  dst.convertTo(dst, CV_8UC3);

  // Dilating the individual white pixels to be slightly larger so they resemble dots
  cv::dilate(dst, dst, cv::Mat(), cv::Point(-1,-1));

  // Making a copy of src for us to modify and overlay dots on
  cv::Mat src_dupe;
  src.copyTo(src_dupe);

  // Using the dilated dst matrix as a mask on our src matrix and putting red pixels wherever the mask lets them through
  src_dupe.setTo(cv::Scalar(0,0,255), dst);
  dst = src_dupe;

  clock_t endTime = clock();

  if (verbose) {
    cout << "Elapsed time: "
         << (endTime - startTime) / static_cast<double>(CLOCKS_PER_SEC)
         << " [s]" << endl;
  }

  if (status) {
    cv::imshow(src_filename, src);
    cv::imshow(src_filename + " [Corners]", dst);
    cv::waitKey(0);

  // Quickly outputting pixel positions for all the white pixels from the original dst matrix
  cout << endl;
  for (int row = 0; row < dst_dupe.rows; row++) {
    for (int col = 0; col < dst_dupe.cols; col++) {
      if (dst_dupe.at<float>(row, col) == 255) {
        cout << "Corner pixel at: (" << row << ", " << col << ")" << endl;
      }
    }
  }

  } else {
    cerr << "*** ERROR *** ";
    cerr << "An error occurred while computing corners" << endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
