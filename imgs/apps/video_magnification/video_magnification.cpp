/** Application/implementation file for Eulerian Video Magnification
 *
 *  \file apps/video_magnification/video_magnification.cpp
 *  \author Josh Carstens, Got Cysts on His Neck (jc@mail.rit.edu)
 *  \date 6 Apr 2021
 *  \note I'll say this again, if you've even opened this file and read this
 * code pls DM me on Discord the confirmation phrase "tomato costume"
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <list>
#include <mutex>
#include <regex>
#include <condition_variable>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
  bool verbose = false;
  string src_filename = "";
  string dst_filename = "null.avi";
  int pyr_levels = 5;
  double magnification = 10;
  double wavelength = 16;
  double h_freq = 3;
  double l_freq = 0.4;
  double chrominance = 0.1;
  double oomph = 1;

  po::options_description options("Options");
  options.add_options()("help,h", "display this message")(
      "verbose,v", po::bool_switch(&verbose), "verbose [default is silent]")(
      "source-filename,i", po::value<string>(&src_filename), "source filename")(
      "destination-filename,d", po::value<string>(&dst_filename),
      "destination filename")("pyr_levels,l", po::value<int>(&pyr_levels),
                              "image pyramid pyr_levels")(
      "magnification,m", po::value<double>(&magnification),
      "magnification factor (alpha)")("wavelength,w",
                                      po::value<double>(&wavelength),
                                      "spatial wavelength (lambda)")(
      "high,b", po::value<double>(&h_freq),
      "high cutoff frequency (omega_h, lowpass)")(
      "low,s", po::value<double>(&l_freq),
      "low cutoff frequency (omega_l, highpass)")(
      "chrominance,c", po::value<double>(&chrominance),
      "chrominance attenuation (I and Q in YIQ color space)")(
      "oomph,o", po::value<double>(&oomph),
      "give it a little oomph (additional magnification scaling)");

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

  if (verbose) {
    cout << "Source filename: " << src_filename << endl;
    cout << "Magnification factor: " << magnification << endl;
    cout << "Spatial wavelength [hz]: " << wavelength << endl;
    cout << "High cutoff frequency [hz]: " << h_freq << endl;
    cout << "Low cutoff frequency [hz]: " << l_freq << endl;
    cout << "Chrominance attenuation: " << chrominance << endl;
    cout << "Oomph factor: " << oomph << endl;
  }

  bool in_done = false;
  mutex in_mutex;
  condition_variable in_notif;
  list<vector<cv::Mat>> in_queue;

  bool out_done = false;
  mutex out_mutex;
  condition_variable out_notif;
  list<tuple<cv::Mat, cv::Mat>> out_queue;

  cv::VideoCapture cap(src_filename);
  auto width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
  auto height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
  auto fps = cap.get(cv::CAP_PROP_FPS);
  cv::VideoWriter video(dst_filename,
                        cv::VideoWriter::fourcc('X', 'V', 'I', 'D'), fps,
                        cv::Size(width, height));

  auto in_thread = thread([&]() {
    cv::Mat frame;
    while (true) {
      if (!cap.read(frame)) {
        in_done = true;
        in_notif.notify_all();
        return;
      }

      auto in = frame.clone();

      in.convertTo(in, CV_32FC3, 1.0 / 255.0f);
      cv::cvtColor(in, in, cv::COLOR_BGR2Lab);

      vector<cv::Mat> pyramid;
      {
        auto current_frame = in;
        for (int l = 0; l < pyr_levels; l++) {
          cv::Mat down, up;

          pyrDown(current_frame, down);
          pyrUp(down, up, current_frame.size());

          pyramid.push_back(current_frame - up);
          current_frame = down;
        }

        pyramid.push_back(current_frame);
        pyramid.push_back(in);
      }

      unique_lock<mutex> lock(in_mutex);
      in_queue.push_back(pyramid);
      in_notif.notify_one();
    }
  });

  auto out_thread = thread([&]() {
    tuple<cv::Mat, cv::Mat> out;
    while (true) {
      {
        unique_lock<mutex> lock(out_mutex);
        out_notif.wait(
            lock, [&]() { return out_done || !out_queue.empty(); });
        if (out_done && out_queue.empty()) {
          return;
        }

        out = out_queue.front();
        out_queue.pop_front();
      }

      auto src = get<0>(out);
      auto dst = get<1>(out);

      cv::cvtColor(src, src, cv::COLOR_Lab2BGR);
      src.convertTo(src, CV_8UC3, 255.0, 1.0 / 255.0);

      cv::cvtColor(dst, dst, cv::COLOR_Lab2BGR);
      dst.convertTo(dst, CV_8UC3, 255.0, 1.0 / 255.0);

      if (dst_filename == "null.avi") {
        cv::imshow("In", src);
        cv::imshow("Out", dst);
        cv::waitKey(30);
      } else {
        video.write(dst);
      }
    }
    if (dst_filename != "null.avi") {
      video.release();
    }
  });

  auto frame_idx = 0;
  vector<cv::Mat> low_pass;
  vector<cv::Mat> high_pass;
  vector<cv::Mat> band_pass;

  while (true) {
    auto t_start = chrono::high_resolution_clock::now();

    vector<cv::Mat> pyramid;
    {
      unique_lock<mutex> lock(in_mutex);
      in_notif.wait(
          lock, [&]() { return in_done || !in_queue.empty(); });
      if (in_done && in_queue.empty()) {
        unique_lock<mutex> lock(out_mutex);
        out_done = true;
        out_notif.notify_all();

        break;
      }

      pyramid = in_queue.front();
      in_queue.pop_front();
    }

    cv::Mat in = pyramid[pyr_levels + 1];

    if (frame_idx == 0) {
      for (int l = 0; l < pyramid.size(); l++) {
        band_pass.push_back(pyramid[l].clone());
        low_pass.push_back(pyramid[l].clone());
        high_pass.push_back(pyramid[l].clone());
      }
    }

    if (frame_idx > 0) {
      auto delta = wavelength / 8.0 / (1.0 + magnification);
      auto lambda = sqrt((float)(width * width + height * height)) / 3;

      vector<thread> workers;
      for (int pyr_level = pyr_levels; pyr_level >= 0; pyr_level--) {
        workers.push_back(thread([&, pyr_level, lambda]() {
          if (pyr_level == pyr_levels || pyr_level == 0) {
            band_pass[pyr_level] *= 0;
            return;
          }

          low_pass[pyr_level] =
              (1 - h_freq) * low_pass[pyr_level] + h_freq * pyramid[pyr_level];
          high_pass[pyr_level] =
              (1 - l_freq) * high_pass[pyr_level] + l_freq * pyramid[pyr_level];
          band_pass[pyr_level] = low_pass[pyr_level] - high_pass[pyr_level];

          auto current_frame_mag = (lambda / delta / 8 - 1) * oomph;
          band_pass[pyr_level] *= min(magnification, current_frame_mag);
        }));

        lambda /= 2.0;
      }

      for_each(workers.begin(), workers.end(),
                    [](thread& t) { t.join(); });
    }

    cv::Mat motion;
    {
      auto current_frame = band_pass[pyr_levels];
      for (int pyr_level = pyr_levels - 1; pyr_level >= 0; --pyr_level) {
        cv::Mat up;
        pyrUp(current_frame, up, band_pass[pyr_level].size());
        current_frame = up + band_pass[pyr_level];
      }
      motion = current_frame;
    }

    if (frame_idx > 0) {
      cv::Mat channel[3];
      split(motion, channel);
      channel[1] = channel[1] * chrominance;
      channel[2] = channel[2] * chrominance;
      cv::merge(channel, 3, motion);

      motion = in + motion;
    }

    auto t_now = chrono::high_resolution_clock::now();
    chrono::milliseconds elapsed =
        chrono::duration_cast<chrono::milliseconds>(t_now - t_start);

    if (verbose) {
    cout << "Frame #" << frame_idx << " processed in " << elapsed.count() << " ms :)"
              << endl;
    }

    frame_idx++;

    unique_lock<mutex> lock(out_mutex);
    out_queue.push_back(tuple<cv::Mat, cv::Mat>(in, motion));
    out_notif.notify_one();
  }

  cout << endl;
  if (dst_filename == "null.avi") {
    cout << "Displaying..." << endl;
  } else {
    cout << "Saving..." << endl;
  }

  in_thread.join();
  out_thread.join();

  return EXIT_SUCCESS;
}
