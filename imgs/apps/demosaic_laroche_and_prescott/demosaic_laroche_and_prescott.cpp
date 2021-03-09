#include <ctime>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "imgs/ipcv/demosaicing/Demosaic.h"
#include "imgs/ipcv/utils/Utils.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  bool verbose = false;
  string src_filename = "";
  string dst_filename = "";
  string pattern = "GBRG";
  int max_value = 65535;

  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
      "verbose,v", po::bool_switch(&verbose), "verbose [default is silent]")(
      "source-filename,i", po::value<string>(&src_filename), "source filename")(
      "destination-filename,o", po::value<string>(&dst_filename),
      "destination filename")("pattern,p", po::value<string>(&pattern),
                              "pattern [default is GBRG]")(
      "max-value,m", po::value<int>(&max_value),
      "maximum value [default is 65535]");

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
    cerr << "Provided source file does not exists" << endl;
    return EXIT_FAILURE;
  }

  cv::Mat src = cv::imread(src_filename, cv::IMREAD_UNCHANGED);

  if (verbose) {
    cout << "Source filename: " << src_filename << endl;
    cout << "Size: " << src.size() << endl;
    cout << "Channels: " << src.channels() << endl;
    cout << "Pattern: " << pattern << endl;
    cout << "Maximum value: " << max_value << endl;
    cout << "Destination filename: " << dst_filename << endl;
  }

  clock_t startTime = clock();

  cv::Mat dst = ipcv::LarochePrescott(src, pattern, max_value);

  clock_t endTime = clock();

  if (verbose) {
    cout << "Elapsed time: "
         << (endTime - startTime) / static_cast<double>(CLOCKS_PER_SEC)
         << " [s]" << endl;
  }

  cv::Mat truth = cv::imread("../data/images/raw/simulated/truth.ppm", cv::IMREAD_UNCHANGED);
  truth.convertTo(truth, CV_8U);
  dst.convertTo(dst, CV_8U);
  cv::Mat truth_copy = truth.clone();

  // Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
  cv::GaussianBlur(truth, truth, cv::Size(3, 3), 0, 0, cv::BORDER_DEFAULT);
  // Convert the image to grayscale
  cv::cvtColor(truth, truth, cv::COLOR_BGR2GRAY);
  cv::Mat truth_grad, truth_grad_x, truth_grad_y;
  cv::Mat truth_abs_grad_x, truth_abs_grad_y;
  int ddepth = CV_16S;
  cv::Sobel(truth, truth_grad_x, ddepth, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
  cv::Sobel(truth, truth_grad_y, ddepth, 0, 1, 3, 1, 0, cv::BORDER_DEFAULT);
  // converting back to CV_8U
  cv::convertScaleAbs(truth_grad_x, truth_abs_grad_x);
  cv::convertScaleAbs(truth_grad_y, truth_abs_grad_y);
  cv::addWeighted(truth_abs_grad_x, 0.5, truth_abs_grad_y, 0.5, 0, truth_grad);

  cv::Mat DeltaE;
  auto DeltaE_result = ipcv::DeltaE(dst, truth_copy, 255, truth_grad, DeltaE, 2000, "graphic_arts");

  std::vector<double> channel_psnr;
  auto psnr = ipcv::Psnr(dst, truth_copy, 255, truth_grad, channel_psnr);

  std::vector<double> channel_rmse;
  auto rmse = ipcv::Rmse(dst, truth_copy, truth_grad, channel_rmse);

  cout << "DeltaE: " << DeltaE_result << endl;
  cout << "PSNR: " << endl;
  cout << psnr << endl;
  if (!channel_psnr.empty()) {
    cout << "PSNR (Channel 0) = " << channel_psnr[0] << endl;
    cout << "PSNR (Channel 1) = " << channel_psnr[1] << endl;
    cout << "PSNR (Channel 2) = " << channel_psnr[2] << endl;
  }
  cout << "RMSE: " << endl;
  cout << rmse << endl;
  if (!channel_rmse.empty()) {
    cout << "RMSE (Channel 0) = " << channel_rmse[0] << endl;
    cout << "RMSE (Channel 1) = " << channel_rmse[1] << endl;
    cout << "RMSE (Channel 2) = " << channel_rmse[2] << endl;
  }

  if (dst_filename.empty()) {
    cv::imshow(src_filename, src);
    cv::imshow(src_filename + " [CFA Interpolated]", DeltaE);
    cv::waitKey(0);
  } else {
    cv::imwrite(dst_filename, DeltaE);
  }

  return EXIT_SUCCESS;
}
