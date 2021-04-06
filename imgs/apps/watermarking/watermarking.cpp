#include <ctime>
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "imgs/ipcv/watermarking/Watermarking.h"

using namespace std;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  bool verbose = false;
  string src_filename = "";
  string dst_filename = "";
  string key_filename = "";

  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
      "verbose,v", po::bool_switch(&verbose), "verbose [default is silent]")(
      "source-filename,i", po::value<string>(&src_filename), "source filename")(
      "destination-filename,o", po::value<string>(&dst_filename),
      "destination filename")(
      "key-filename,k", po::value<string>(&key_filename), "key filename");

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
    cerr << "Provided source file does not exist" << endl;
    return EXIT_FAILURE;
  }

  cv::Mat src = cv::imread(src_filename, cv::IMREAD_COLOR);

  if (!boost::filesystem::exists(key_filename)) {
    cerr << "Provided key file does not exist" << endl;
    return EXIT_FAILURE;
  }
  
  cv::Mat key = cv::imread(key_filename, cv::IMREAD_COLOR);

  cv::Mat dst;
  dst.create(src.size(), src.type());

	cv::Mat decode;
	decode.create(key.size(), key.type());

  if (verbose) {
    cout << "Source filename: " << src_filename << endl;
    cout << "Source Size: " << src.size() << endl;
    cout << "Key filename: " << key_filename << endl;
    cout << "Key Size: " << key.size() << endl;
    cout << "Destination filename: " << dst_filename << endl;
  }

  clock_t startTime = clock();

  ipcv::watermarking(src, dst, key, decode);

  clock_t endTime = clock();

  if (verbose) {
    cout << "Elapsed time: "
         << (endTime - startTime) / static_cast<double>(CLOCKS_PER_SEC)
         << " [s]" << endl;
  }


  if (dst_filename.empty()) {
    cv::imshow(src_filename, src);
    cv::imshow(src_filename + " [watermarked]", dst);
    cv::imshow(key_filename, key);
    cv::imshow(key_filename + " [decoded]", decode);
    cv::waitKey(0);
  } else {
    cv::imwrite(dst_filename, dst);
  }

  return EXIT_SUCCESS;
}
