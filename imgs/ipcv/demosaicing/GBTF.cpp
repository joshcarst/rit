/** Implementation file for gradient-based threshold-free CFA demosaic function
 *
 *  \file ipcv/demosaicing/GBTF.cpp
 *  \author Josh Carstens, So Sure of Himself (jc@mail.rit.edu)
 *  \date 03 Mar 2021
 *  \note Here's a preview of my next album bc I like u guys so much u deserve
 * to hear
 * https://drive.google.com/drive/folders/1nACYAHjI-DPuEi61Pr26Pu_ACZ88dOCn?usp=sharing
 */

#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "GBTF.h"

using namespace std;
using namespace cv;

namespace ipcv {

void GBTF(const Mat& src, Mat& dst, string pattern) {
  // Carl's bayer masking code but modified
  // The mask is hardcoded to RGGB and the image is shifted around by 1 pixel to
  // fit
  Mat src_dupe = src.clone();
  int b_row = 1;
  int b_col = 1;
  int r_row = 0;
  int r_col = 0;
  if (pattern == "GBRG") {
    copyMakeBorder(src_dupe, src_dupe, 1, 1, 0, 0, cv::BORDER_REFLECT_101);
  } else if (pattern == "GRBG") {
    copyMakeBorder(src_dupe, src_dupe, 0, 0, 1, 1, cv::BORDER_REFLECT_101);
  } else if (pattern == "BGGR") {
    copyMakeBorder(src_dupe, src_dupe, 1, 1, 1, 1, cv::BORDER_REFLECT_101);
  } else if (pattern == "RGGB") {
    // Nothing to do here but it's a valid option
  } else {
    cerr << "Invalid CFA pattern provided: " << pattern << endl;
    exit(EXIT_FAILURE);
  }
  Mat b_mask = Mat::zeros(src_dupe.size(), CV_16UC1);
  for (int r = b_row; r < src_dupe.rows; r += 2) {
    for (int c = b_col; c < src_dupe.cols; c += 2) {
      b_mask.at<uint16_t>(r, c) = 1;
    }
  }
  Mat r_mask = Mat::zeros(src_dupe.size(), CV_16UC1);
  for (int r = r_row; r < src_dupe.rows; r += 2) {
    for (int c = r_col; c < src_dupe.cols; c += 2) {
      r_mask.at<uint16_t>(r, c) = 1;
    }
  }

  // Create green (G) filter location mask
  Mat g_mask = Mat::ones(src_dupe.size(), CV_16UC1);
  cv::subtract(g_mask, b_mask, g_mask);
  cv::subtract(g_mask, r_mask, g_mask);

  // Initialize interpolated B, G, and R channels
  Mat B;
  cv::multiply(src_dupe, b_mask, B);
  Mat G;
  cv::multiply(src_dupe, g_mask, G);
  Mat R;
  cv::multiply(src_dupe, r_mask, R);

  Mat channels[3];
  channels[0] = B;
  channels[1] = G;
  channels[2] = R;

  // Merging to convert to 32F and then immediately splitting again so no seg
  // faults. Efficient I know
  cv::merge(channels, 3, src_dupe);
  src_dupe.convertTo(src_dupe, CV_32F);
  vector<Mat> OrigChannels(3), OutputChannels(3);
  split(src_dupe, OrigChannels);
  split(src_dupe, OutputChannels);

  // Giving the frame a 2 pixel repeating border so interpolation can occur on
  // the edges
  copyMakeBorder(OrigChannels[0], OrigChannels[0], 2, 2, 2, 2,
                 cv::BORDER_DEFAULT);
  copyMakeBorder(OrigChannels[1], OrigChannels[1], 2, 2, 2, 2,
                 cv::BORDER_DEFAULT);
  copyMakeBorder(OrigChannels[2], OrigChannels[2], 2, 2, 2, 2,
                 cv::BORDER_DEFAULT);

  Mat Horiz_Map(src_dupe.size(), CV_32F, cv::Scalar(0));
  Mat Vert_Map(src_dupe.size(), CV_32F, cv::Scalar(0));
  float Horiz_R, Vert_R, Horiz_G, Vert_G, Horiz_B, Vert_B;

  // Creating horizontal and vertical red/blue pixel estimations at green pixel
  // locations Using Hamilton and Adams' formula
  for (int row = 0; row < src_dupe.rows; row++) {
    for (int col = 0; col < src_dupe.cols; col++) {
      // i and j are the actual pixel locations because we added the 2 pixel
      // border
      int i = row + 2;
      int j = col + 2;
      // Red cases
      if (row % 2 == 0 && col % 2 == 0) {
        Horiz_G = (OrigChannels[1].at<float>(i, j - 1) +
                   OrigChannels[1].at<float>(i, j + 1)) *
                      0.5 +
                  (2 * OrigChannels[2].at<float>(i, j) -
                   OrigChannels[2].at<float>(i, j - 2) -
                   OrigChannels[2].at<float>(i, j + 2)) *
                      0.25;
        Vert_G = (OrigChannels[1].at<float>(i - 1, j) +
                  OrigChannels[1].at<float>(i + 1, j)) *
                     0.5 +
                 (2 * OrigChannels[2].at<float>(i, j) -
                  OrigChannels[2].at<float>(i - 2, j) -
                  OrigChannels[2].at<float>(i + 2, j)) *
                     0.25;
        Horiz_Map.at<float>(row, col) =
            Horiz_G - OrigChannels[2].at<float>(i, j);
        Vert_Map.at<float>(row, col) = Vert_G - OrigChannels[2].at<float>(i, j);
        // Blue cases
      } else if (row % 2 == 1 && col % 2 == 1) {
        Horiz_G = (OrigChannels[1].at<float>(i, j - 1) +
                   OrigChannels[1].at<float>(i, j + 1)) *
                      0.5 +
                  (2 * OrigChannels[0].at<float>(i, j) -
                   OrigChannels[0].at<float>(i, j - 2) -
                   OrigChannels[0].at<float>(i, j + 2)) *
                      0.25;
        Vert_G = (OrigChannels[1].at<float>(i - 1, j) +
                  OrigChannels[1].at<float>(i + 1, j)) *
                     0.5 +
                 (2 * OrigChannels[0].at<float>(i, j) -
                  OrigChannels[0].at<float>(i - 2, j) -
                  OrigChannels[0].at<float>(i + 2, j)) *
                     0.25;
        Horiz_Map.at<float>(row, col) =
            Horiz_G - OrigChannels[0].at<float>(i, j);
        Vert_Map.at<float>(row, col) = Vert_G - OrigChannels[0].at<float>(i, j);
        // Green cases where red is above/below
      } else if (row % 2 == 1 && col % 2 == 0) {
        Horiz_B = (OrigChannels[0].at<float>(i, j - 1) +
                   OrigChannels[0].at<float>(i, j + 1)) *
                      0.5 +
                  (2 * OrigChannels[1].at<float>(i, j) -
                   OrigChannels[1].at<float>(i, j - 2) -
                   OrigChannels[1].at<float>(i, j + 2)) *
                      0.25;
        Vert_R = (OrigChannels[2].at<float>(i - 1, j) +
                  OrigChannels[2].at<float>(i + 1, j)) *
                     0.5 +
                 (2 * OrigChannels[1].at<float>(i, j) -
                  OrigChannels[1].at<float>(i - 2, j) -
                  OrigChannels[1].at<float>(i + 2, j)) *
                     0.25;
        Horiz_Map.at<float>(row, col) =
            OrigChannels[1].at<float>(i, j) - Horiz_B;
        Vert_Map.at<float>(row, col) = OrigChannels[1].at<float>(i, j) - Vert_R;
        // Green cases where blue is above/below
      } else {
        Horiz_R = (OrigChannels[2].at<float>(i, j - 1) +
                   OrigChannels[2].at<float>(i, j + 1)) *
                      0.5 +
                  (2 * OrigChannels[1].at<float>(i, j) -
                   OrigChannels[1].at<float>(i, j - 2) -
                   OrigChannels[1].at<float>(i, j + 2)) *
                      0.25;
        Vert_B = (OrigChannels[0].at<float>(i - 1, j) +
                  OrigChannels[0].at<float>(i + 1, j)) *
                     0.5 +
                 (2 * OrigChannels[1].at<float>(i, j) -
                  OrigChannels[1].at<float>(i - 2, j) -
                  OrigChannels[1].at<float>(i + 2, j)) *
                     0.25;
        Horiz_Map.at<float>(row, col) =
            OrigChannels[1].at<float>(i, j) - Horiz_R;
        Vert_Map.at<float>(row, col) = OrigChannels[1].at<float>(i, j) - Vert_B;
      }
    }
  }

  // 4 pixel border to prepare for future neighborhood interpolation
  copyMakeBorder(Vert_Map, Vert_Map, 4, 4, 4, 4, cv::BORDER_DEFAULT);
  copyMakeBorder(Horiz_Map, Horiz_Map, 4, 4, 4, 4, cv::BORDER_DEFAULT);

  // Color difference gradients.
  Mat Vert_Gradient;
  Mat Horiz_Gradient;

  // Creating quick kernels for subtracting sides from each other
  float kernel[3] = {-1, 0, 1};
  Mat Horiz_Kernel(1, 3, CV_32F, kernel);
  Mat Vert_Kernel(3, 1, CV_32F, kernel);

  // Essentially taking the difference of the pixels on either side of the
  // target difference map pixel
  cv::filter2D(Horiz_Map, Horiz_Gradient, -1, Horiz_Kernel);
  cv::filter2D(Vert_Map, Vert_Gradient, -1, Vert_Kernel);

  // Finally taking the absolute value
  Horiz_Gradient = cv::abs(Horiz_Gradient);
  Vert_Gradient = cv::abs(Vert_Gradient);

  // Matrices that will store the difference values that inform red pixel values
  // at green pixel locations and so on for blue
  Mat Delta_GR(src_dupe.size(), CV_32F, cv::Scalar(0));
  Mat Delta_GB(src_dupe.size(), CV_32F, cv::Scalar(0));

  // Values go in order of N, S, W, E
  float Weight[4];

  // The little numbers on the weight summations in the paper
  int ShiftAmount[4][2] = {{-4, -2}, {0, -2}, {-2, -4}, {-2, 0}};
  int Shift_X, Shift_Y;
  float Weight_Sum;

  // Calculating weights and then forming a final green difference estimation
  for (int row = 0; row < src_dupe.rows; row++) {
    // Offset the column on every other row to match the green pixel locations
    int col = 0;
    if (row % 2 == 1) {
      col = 1;
    }
    for (; col < src_dupe.cols; col += 2) {
      Weight[0] = 0.0;  // N
      Weight[1] = 0.0;  // S
      Weight[2] = 0.0;  // W
      Weight[3] = 0.0;  // E
      Weight_Sum = 0.0;

      for (int nswe = 0; nswe < 4; nswe++) {
        Shift_X = ShiftAmount[nswe][0];
        Shift_Y = ShiftAmount[nswe][1];

        // 5 x 5 neighborhood
        for (int i = 0; i < 5; i++) {
          for (int j = 0; j < 5; j++) {
            // Vertical cases
            if (nswe < 2) {
              Weight[nswe] += Vert_Gradient.at<float>(4 + row + Shift_X + i,
                                                      4 + col + Shift_Y + j);
              // Horizontal cases
            } else {
              Weight[nswe] += Horiz_Gradient.at<float>(4 + row + Shift_X + i,
                                                       4 + col + Shift_Y + j);
            }
          }
        }

        // Square final value
        Weight[nswe] *= Weight[nswe];
        // Make reciprocal but only if it's not 0 bc div/0 is bad
        if (Weight[nswe] != 0) {
          Weight[nswe] = 1.0 / Weight[nswe];
        } else {
          Weight[nswe] = 0;
        }
        // Add on to sum
        Weight_Sum += Weight[nswe];
      }

      // Compensating for 4 pixel border
      int i = row + 4;
      int j = col + 4;
      // Green pixels at red locations
      if (row % 2 == 0) {
        // Once again checking for div/0
        if (Weight_Sum != 0) {
          Delta_GR.at<float>(row, col) =
              (Weight[0] * 0.2 *
                   (Vert_Map.at<float>(i - 4, j) +
                    Vert_Map.at<float>(i - 3, j) +
                    Vert_Map.at<float>(i - 2, j) +
                    Vert_Map.at<float>(i - 1, j) + Vert_Map.at<float>(i, j)) +
               Weight[1] * 0.2 *
                   (Vert_Map.at<float>(i, j) + Vert_Map.at<float>(i + 1, j) +
                    Vert_Map.at<float>(i + 2, j) +
                    Vert_Map.at<float>(i + 3, j) +
                    Vert_Map.at<float>(i + 4, j)) +
               Weight[2] * 0.2 *
                   (Horiz_Map.at<float>(i, j - 4) +
                    Horiz_Map.at<float>(i, j - 3) +
                    Horiz_Map.at<float>(i, j - 2) +
                    Horiz_Map.at<float>(i, j - 1) + Horiz_Map.at<float>(i, j)) +
               Weight[3] * 0.2 *
                   (Horiz_Map.at<float>(i, j) + Horiz_Map.at<float>(i, j + 1) +
                    Horiz_Map.at<float>(i, j + 2) +
                    Horiz_Map.at<float>(i, j + 3) +
                    Horiz_Map.at<float>(i, j + 4))) /
              Weight_Sum;
        } else {
          Delta_GR.at<float>(row, col) = 0;
        }
        // Adding difference value on to known R pixel value to get final
        // interpolated G value
        OutputChannels[1].at<float>(row, col) =
            OutputChannels[2].at<float>(row, col) +
            Delta_GR.at<float>(row, col);  // R + Delta_GB
        // Green pixels at red locations
      } else {
        if (Weight_Sum != 0) {
          Delta_GB.at<float>(row, col) =
              (Weight[0] * 0.2 *
                   (Vert_Map.at<float>(i - 4, j) +
                    Vert_Map.at<float>(i - 3, j) +
                    Vert_Map.at<float>(i - 2, j) +
                    Vert_Map.at<float>(i - 1, j) + Vert_Map.at<float>(i, j)) +
               Weight[1] * 0.2 *
                   (Vert_Map.at<float>(i, j) + Vert_Map.at<float>(i + 1, j) +
                    Vert_Map.at<float>(i + 2, j) +
                    Vert_Map.at<float>(i + 3, j) +
                    Vert_Map.at<float>(i + 4, j)) +
               Weight[2] * 0.2 *
                   (Horiz_Map.at<float>(i, j - 4) +
                    Horiz_Map.at<float>(i, j - 3) +
                    Horiz_Map.at<float>(i, j - 2) +
                    Horiz_Map.at<float>(i, j - 1) + Horiz_Map.at<float>(i, j)) +
               Weight[3] * 0.2 *
                   (Horiz_Map.at<float>(i, j) + Horiz_Map.at<float>(i, j + 1) +
                    Horiz_Map.at<float>(i, j + 2) +
                    Horiz_Map.at<float>(i, j + 3) +
                    Horiz_Map.at<float>(i, j + 4))) /
              Weight_Sum;
        } else {
          Delta_GB.at<float>(row, col) = 0;
        }
        OutputChannels[1].at<float>(row, col) =
            OutputChannels[0].at<float>(row, col) +
            Delta_GB.at<float>(row, col);  // B + Delta_GB
      }
    }
  }

  // Filter from Paliy et al. to determine red pixel values at blue locations
  // and vice versa
  float RB_Filter_Values[49] = {
      0,        0, -0.03125, 0, -0.03125, 0, 0,        0, 0, 0, 0, 0, 0, 0,
      -0.03125, 0, 0.3125,   0, 0.3125,   0, -0.03125, 0, 0, 0, 0, 0, 0, 0,
      -0.03125, 0, 0.3125,   0, 0.3125,   0, -0.03125, 0, 0, 0, 0, 0, 0, 0,
      0,        0, -0.03125, 0, -0.03125, 0, 0};
  Mat Filter_RB(7, 7, CV_32FC1, RB_Filter_Values);

  // 3 pixel border for more averaging purposes
  copyMakeBorder(Delta_GR, Delta_GR, 3, 3, 3, 3, cv::BORDER_DEFAULT);
  copyMakeBorder(Delta_GB, Delta_GB, 3, 3, 3, 3, cv::BORDER_DEFAULT);

  for (int row = 0; row < src.rows; row++) {
    // Similar trick as above
    int col = 0;
    if (row % 2 == 1) {
      col = 1;
    }
    for (; col < src.cols; col += 2) {
      // Blue pixels at red locations
      if (row % 2 == 0) {
        OutputChannels[0].at<float>(row, col) =
            OutputChannels[1].at<float>(row, col) -
            cv::sum(Delta_GB(cv::Range(3 + row - 3, 3 + row + 3 + 1),
                             cv::Range(3 + col - 3, 3 + col + 3 + 1))
                        .mul(Filter_RB))[0];
        // Red pixels at blue locations
      } else {
        OutputChannels[2].at<float>(row, col) =
            OutputChannels[1].at<float>(row, col) -
            cv::sum(Delta_GR(cv::Range(3 + row - 3, 3 + row + 3 + 1),
                             cv::Range(3 + col - 3, 3 + col + 3 + 1))
                        .mul(Filter_RB))[0];
      }
    }
  }

  // 1 pixel border for bilinear interpolation
  copyMakeBorder(OutputChannels[0], OrigChannels[0], 1, 1, 1, 1,
                 cv::BORDER_DEFAULT);
  copyMakeBorder(OutputChannels[1], OrigChannels[1], 1, 1, 1, 1,
                 cv::BORDER_DEFAULT);
  copyMakeBorder(OutputChannels[2], OrigChannels[2], 1, 1, 1, 1,
                 cv::BORDER_DEFAULT);

  for (int row = 0; row < src_dupe.rows; row++) {
    int col = 0;
    if (row % 2 == 0) {
      col = 1;
    }
    for (; col < src_dupe.cols; col += 2) {
      int i = row + 1;
      int j = col + 1;
      // Red pixels at green locations
      OutputChannels[2].at<float>(row, col) =
          OutputChannels[1].at<float>(row, col) -
          (OrigChannels[1].at<float>(i - 1, j) -
           OrigChannels[2].at<float>(i - 1, j)) /
              4.0 -
          (OrigChannels[1].at<float>(i + 1, j) -
           OrigChannels[2].at<float>(i + 1, j)) /
              4.0 -
          (OrigChannels[1].at<float>(i, j - 1) -
           OrigChannels[2].at<float>(i, j - 1)) /
              4.0 -
          (OrigChannels[1].at<float>(i, j + 1) -
           OrigChannels[2].at<float>(i, j + 1)) /
              4.0;
      // Blue pixels at green locations
      OutputChannels[0].at<float>(row, col) =
          OutputChannels[1].at<float>(row, col) -
          (OrigChannels[1].at<float>(i - 1, j) -
           OrigChannels[0].at<float>(i - 1, j)) /
              4.0 -
          (OrigChannels[1].at<float>(i + 1, j) -
           OrigChannels[0].at<float>(i + 1, j)) /
              4.0 -
          (OrigChannels[1].at<float>(i, j - 1) -
           OrigChannels[0].at<float>(i, j - 1)) /
              4.0 -
          (OrigChannels[1].at<float>(i, j + 1) -
           OrigChannels[0].at<float>(i, j + 1)) /
              4.0;
    }
  }

  // Combining channels for final output
  merge(OutputChannels, dst);

  // Shifting pixels back to their original location
  if (pattern == "GBRG") {
    dst = dst(Rect(0, 1, src_dupe.cols, src_dupe.rows - 2));
  } else if (pattern == "GRBG") {
    dst = dst(Rect(1, 0, src_dupe.cols - 2, src_dupe.rows));
  } else if (pattern == "BGGR") {
    dst = dst(Rect(1, 1, src_dupe.cols - 2, src_dupe.rows - 2));
  } else if (pattern == "RGGB") {
  } else {
    cerr << "Invalid CFA pattern provided: " << pattern << endl;
    exit(EXIT_FAILURE);
  }

  // Converting to 8U for display purposes
  dst.convertTo(dst, CV_8U);

  return;
}
}  // namespace ipcv