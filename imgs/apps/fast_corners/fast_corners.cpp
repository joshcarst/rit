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
  int difference_threshold = 50;
  int contiguous_threshold = 12;
  bool nonmaximal_suppression = false;

  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
      "verbose,v", po::bool_switch(&verbose), "verbose [default is silent]")(
      "source-filename,i", po::value<string>(&src_filename), "source filename")(
      "difference-threshold,d", po::value<int>(&difference_threshold),
      "brightness threshold [default is 50]")(
      "contiguous-threshold,c", po::value<int>(&contiguous_threshold),
      "contiguous pixel threshold [default is 12]")(
      "nonmaximal-suppression,s", po::bool_switch(&nonmaximal_suppression),
      "use non-maximal supression [default is false]");

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
    cout << "Difference threshold: " << difference_threshold << endl;
    cout << "Contiguous threshold: " << contiguous_threshold << endl;
    if (nonmaximal_suppression) {
      cout << "Use non-maximal suppression: "
           << "Yes" << endl;
    } else {
      cout << "Use non-maximal suppression: "
           << "No" << endl;
    }
  }

  clock_t startTime = clock();

  bool status = false;
  cv::Mat dst;
  status = ipcv::Fast(src, dst, difference_threshold, contiguous_threshold,
                      nonmaximal_suppression);

  // Not sure how much of this should be done in the app file vs the
  // implementation but we doing it

  // Making a copy of dst exactly as it comes from ipcv::Harris for outputting
  // the coordinates later
  cv::Mat dst_dupe;
  dst.copyTo(dst_dupe);

  // Setting dst to three-channel ints so we can put red dots on there
  cv::cvtColor(dst, dst, cv::COLOR_GRAY2BGR);

  // Dilating the individual white pixels to be slightly larger so they resemble
  // dots
  cv::dilate(dst, dst, cv::Mat(), cv::Point(-1, -1));

  // Making a copy of src for us to modify and overlay dots on
  cv::Mat src_dupe;
  src.copyTo(src_dupe);

  // Using the dilated dst matrix as a mask on our src matrix and putting red
  // pixels wherever the mask lets them through
  src_dupe.setTo(cv::Scalar(0, 0, 255), dst);
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

    cout << endl;
    for (int row = 0; row < dst_dupe.rows; row++) {
      for (int col = 0; col < dst_dupe.cols; col++) {
        if (dst_dupe.at<int>(row, col) == 255) {
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
