/** Implementation file for image filtering
 *
 *  \file ipcv/spatial_filtering/Filter2D.cpp
 *  \author Josh Carstens, Man Once Scorned (jc@mail.rit.edu)
 *  \date 10 Oct 2020
 *  \note Yes, this is awful. I know the edge pixels are messed up. It's okay.
 */

#include "Filter2D.h"

#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace std;

namespace ipcv {

/** Correlates an image with the provided kernel
 *
 *  \param[in] src          source cv::Mat of CV_8UC3
 *  \param[out] dst         destination cv::Mat of ddepth type
 *  \param[in] ddepth       desired depth of the destination image
 *  \param[in] kernel       convolution kernel (or rather a correlation
 *                          kernel), a single-channel floating point matrix
 *  \param[in] anchor       anchor of the kernel that indicates the relative
 *                          position of a filtered point within the kernel;
 *                          the anchor should lie within the kernel; default
 *                          value (-1,-1) means that the anchor is at the
 *                          kernel center
 *  \param[in] delta        optional value added to the filtered pixels
 *                          before storing them in dst
 *  \param[in] border_mode  pixel extrapolation method
 *  \param[in] border_value value to use for constant border mode
 */
bool Filter2D(const cv::Mat& src, cv::Mat& dst, const int ddepth,
              const cv::Mat& kernel, const cv::Point anchor, const int delta,
              const BorderMode border_mode, const uint8_t border_value) {
  // Um ya I hardcoded only the cases for a 3x3 and 5x5 kernel, what of it
  if (kernel.rows == 3) {
    // Right off the bat we're converting the matrix to signed float values to
    // interact with the potential negative values in the kernel
    cv::Mat src_signed = cv::Mat(src.size(), CV_32FC3);
    src.convertTo(src_signed, CV_32FC3);

    // Each of these matrices are the original image shifted by a pixel in each
    // direction U is upper, B is bottom, C is center, and u know what L/R are
    cv::Mat UL, UC, UR, CL, CR, BL, BC, BR, delta_matrix, dst_signed;
    UL.create(src.size(), CV_32FC3);
    UC.create(src.size(), CV_32FC3);
    UR.create(src.size(), CV_32FC3);
    CL.create(src.size(), CV_32FC3);
    CR.create(src.size(), CV_32FC3);
    BL.create(src.size(), CV_32FC3);
    BC.create(src.size(), CV_32FC3);
    BR.create(src.size(), CV_32FC3);
    dst.create(src.size(), CV_8UC3);
    dst_signed.create(src.size(), CV_32FC3);

    // Making a matrix of the size of the image filled with the delta value to
    // add to the dst later
    if (delta != 0) {
      delta_matrix.create(src.size(), CV_32FC3);
      delta_matrix = cv::Scalar(delta, delta, delta);
      dst_signed += delta_matrix;
    }

    // Making translation matrices to use with warpAffine
    cv::Mat UL_t = (cv::Mat_<double>(2, 3) << 1, 0, 1, 0, 1, 1);
    cv::Mat UC_t = (cv::Mat_<double>(2, 3) << 1, 0, 1, 0, 1, 0);
    cv::Mat UR_t = (cv::Mat_<double>(2, 3) << 1, 0, 1, 0, 1, -1);
    cv::Mat CL_t = (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, 1);
    cv::Mat CR_t = (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, -1);
    cv::Mat BL_t = (cv::Mat_<double>(2, 3) << 1, 0, -1, 0, 1, 1);
    cv::Mat BC_t = (cv::Mat_<double>(2, 3) << 1, 0, -1, 0, 1, 0);
    cv::Mat BR_t = (cv::Mat_<double>(2, 3) << 1, 0, -1, 0, 1, -1);

    // Creating the shifted image matrices
    cv::warpAffine(src_signed, UL, UL_t, src.size());
    cv::warpAffine(src_signed, UC, UC_t, src.size());
    cv::warpAffine(src_signed, UR, UR_t, src.size());
    cv::warpAffine(src_signed, CL, CL_t, src.size());
    cv::warpAffine(src_signed, CR, CR_t, src.size());
    cv::warpAffine(src_signed, BL, BL_t, src.size());
    cv::warpAffine(src_signed, BC, BC_t, src.size());
    cv::warpAffine(src_signed, BR, BR_t, src.size());

    // Weighing each shifted matrix by the kernel value
    UL *= kernel.at<float>(0, 0);
    UC *= kernel.at<float>(1, 0);
    UR *= kernel.at<float>(2, 0);
    CL *= kernel.at<float>(0, 1);
    // Here we just repurpose the converted src matrix as the center shifted
    // matrix
    src_signed *= kernel.at<float>(1, 1);
    CR *= kernel.at<float>(2, 1);
    BL *= kernel.at<float>(0, 2);
    BC *= kernel.at<float>(1, 2);
    BR *= kernel.at<float>(2, 2);

    // Adding everything onto the dst matrix in signed float form and converting
    // back to 8U
    dst_signed += UL + UC + UR + CL + src_signed + CR + BL + BC + BR;
    dst_signed.convertTo(dst, CV_8UC3);

    // This is where it really starts to hurt. I'd like to repent. I know loops
    // could probably be used but I'm used to suffering
  } else if (kernel.rows == 5) {
    cv::Mat src_signed = cv::Mat(src.size(), CV_32FC3);
    src.convertTo(src_signed, CV_32FC3);

    // Here we have UU for upper-upper, MU for middle-upper, and so forth
    cv::Mat UULL, UUML, UUC, UUMR, UURR, MULL, MUML, MUC, MUMR, MURR, CLL, CML,
        CMR, CRR, MBLL, MBML, MBC, MBMR, MBRR, BBLL, BBML, BBC, BBMR, BBRR,
        delta_matrix, dst_signed;
    UULL.create(src.size(), CV_32FC3);
    UUML.create(src.size(), CV_32FC3);
    UUC.create(src.size(), CV_32FC3);
    UUMR.create(src.size(), CV_32FC3);
    UURR.create(src.size(), CV_32FC3);
    MULL.create(src.size(), CV_32FC3);
    MUML.create(src.size(), CV_32FC3);
    MUC.create(src.size(), CV_32FC3);
    MUMR.create(src.size(), CV_32FC3);
    MURR.create(src.size(), CV_32FC3);
    CLL.create(src.size(), CV_32FC3);
    CML.create(src.size(), CV_32FC3);
    CMR.create(src.size(), CV_32FC3);
    CRR.create(src.size(), CV_32FC3);
    MBLL.create(src.size(), CV_32FC3);
    MBML.create(src.size(), CV_32FC3);
    MBC.create(src.size(), CV_32FC3);
    MBMR.create(src.size(), CV_32FC3);
    MBRR.create(src.size(), CV_32FC3);
    BBLL.create(src.size(), CV_32FC3);
    BBML.create(src.size(), CV_32FC3);
    BBC.create(src.size(), CV_32FC3);
    BBMR.create(src.size(), CV_32FC3);
    BBRR.create(src.size(), CV_32FC3);
    dst.create(src.size(), CV_8UC3);
    dst_signed.create(src.size(), CV_32FC3);

    if (delta != 0) {
      delta_matrix.create(src.size(), CV_32FC3);
      delta_matrix = cv::Scalar(delta, delta, delta);
      dst_signed += delta_matrix;
    }

    // Yes I know it hurts. Lean into the pain. You start to feel numb.
    cv::Mat UULL_t = (cv::Mat_<double>(2, 3) << 1, 0, 2, 0, 1, 2);
    cv::Mat UUML_t = (cv::Mat_<double>(2, 3) << 1, 0, 2, 0, 1, 1);
    cv::Mat UUC_t = (cv::Mat_<double>(2, 3) << 1, 0, 2, 0, 1, 0);
    cv::Mat UUMR_t = (cv::Mat_<double>(2, 3) << 1, 0, 2, 0, 1, -1);
    cv::Mat UURR_t = (cv::Mat_<double>(2, 3) << 1, 0, 2, 0, 1, -2);
    cv::Mat MULL_t = (cv::Mat_<double>(2, 3) << 1, 0, 1, 0, 1, 2);
    cv::Mat MUML_t = (cv::Mat_<double>(2, 3) << 1, 0, 1, 0, 1, 1);
    cv::Mat MUC_t = (cv::Mat_<double>(2, 3) << 1, 0, 1, 0, 1, 0);
    cv::Mat MUMR_t = (cv::Mat_<double>(2, 3) << 1, 0, 1, 0, 1, -1);
    cv::Mat MURR_t = (cv::Mat_<double>(2, 3) << 1, 0, 1, 0, 1, -2);
    cv::Mat CLL_t = (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, 2);
    cv::Mat CML_t = (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, 1);
    cv::Mat CMR_t = (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, -1);
    cv::Mat CRR_t = (cv::Mat_<double>(2, 3) << 1, 0, 0, 0, 1, -2);
    cv::Mat MBLL_t = (cv::Mat_<double>(2, 3) << 1, 0, -1, 0, 1, 2);
    cv::Mat MBML_t = (cv::Mat_<double>(2, 3) << 1, 0, -1, 0, 1, 1);
    cv::Mat MBC_t = (cv::Mat_<double>(2, 3) << 1, 0, -1, 0, 1, 0);
    cv::Mat MBMR_t = (cv::Mat_<double>(2, 3) << 1, 0, -1, 0, 1, -1);
    cv::Mat MBRR_t = (cv::Mat_<double>(2, 3) << 1, 0, -1, 0, 1, -2);
    cv::Mat BBLL_t = (cv::Mat_<double>(2, 3) << 1, 0, -2, 0, 1, 2);
    cv::Mat BBML_t = (cv::Mat_<double>(2, 3) << 1, 0, -2, 0, 1, 1);
    cv::Mat BBC_t = (cv::Mat_<double>(2, 3) << 1, 0, -2, 0, 1, 0);
    cv::Mat BBMR_t = (cv::Mat_<double>(2, 3) << 1, 0, -2, 0, 1, -1);
    cv::Mat BBRR_t = (cv::Mat_<double>(2, 3) << 1, 0, -2, 0, 1, -2);

    // There's not much to say about this. It's not good for you to look at it
    // too long.
    cv::warpAffine(src_signed, UULL, UULL_t, src.size());
    cv::warpAffine(src_signed, UUML, UUML_t, src.size());
    cv::warpAffine(src_signed, UUC, UUC_t, src.size());
    cv::warpAffine(src_signed, UUMR, UUMR_t, src.size());
    cv::warpAffine(src_signed, UURR, UURR_t, src.size());
    cv::warpAffine(src_signed, MULL, MULL_t, src.size());
    cv::warpAffine(src_signed, MUML, MUML_t, src.size());
    cv::warpAffine(src_signed, MUC, MUC_t, src.size());
    cv::warpAffine(src_signed, MUMR, MUMR_t, src.size());
    cv::warpAffine(src_signed, MURR, MURR_t, src.size());
    cv::warpAffine(src_signed, CLL, CLL_t, src.size());
    cv::warpAffine(src_signed, CML, CML_t, src.size());
    cv::warpAffine(src_signed, CMR, CMR_t, src.size());
    cv::warpAffine(src_signed, CRR, CRR_t, src.size());
    cv::warpAffine(src_signed, MBLL, MBLL_t, src.size());
    cv::warpAffine(src_signed, MBML, MBML_t, src.size());
    cv::warpAffine(src_signed, MBC, MBC_t, src.size());
    cv::warpAffine(src_signed, MBMR, MBMR_t, src.size());
    cv::warpAffine(src_signed, MBRR, MBRR_t, src.size());
    cv::warpAffine(src_signed, BBLL, BBLL_t, src.size());
    cv::warpAffine(src_signed, BBML, BBML_t, src.size());
    cv::warpAffine(src_signed, BBC, BBC_t, src.size());
    cv::warpAffine(src_signed, BBMR, BBMR_t, src.size());
    cv::warpAffine(src_signed, BBRR, BBRR_t, src.size());

    // "My eyes are burning in ways I haven't felt since learning of javascript"
    // -my cousin, a CS major
    UULL *= kernel.at<float>(0, 0);
    UUML *= kernel.at<float>(1, 0);
    UUC *= kernel.at<float>(2, 0);
    UUMR *= kernel.at<float>(3, 0);
    UURR *= kernel.at<float>(4, 0);
    MULL *= kernel.at<float>(0, 1);
    MUML *= kernel.at<float>(1, 1);
    MUC *= kernel.at<float>(2, 1);
    MUMR *= kernel.at<float>(3, 1);
    MURR *= kernel.at<float>(4, 1);
    CLL *= kernel.at<float>(0, 2);
    CML *= kernel.at<float>(1, 2);
    src_signed *= kernel.at<float>(2, 2);
    CMR *= kernel.at<float>(3, 2);
    CRR *= kernel.at<float>(4, 2);
    MBLL *= kernel.at<float>(0, 3);
    MBML *= kernel.at<float>(1, 3);
    MBC *= kernel.at<float>(2, 3);
    MBMR *= kernel.at<float>(3, 3);
    MBRR *= kernel.at<float>(4, 3);
    BBLL *= kernel.at<float>(0, 4);
    BBML *= kernel.at<float>(1, 4);
    BBC *= kernel.at<float>(2, 4);
    BBMR *= kernel.at<float>(3, 4);
    BBRR *= kernel.at<float>(4, 4);

    dst_signed += UULL + UUML + UUC + UUMR + UURR + MULL + MUML + MUC + MUMR +
                  MURR + CLL + CML + src_signed + CMR + CRR + MBLL + MBML +
                  MBC + MBMR + MBRR + BBLL + BBML + BBC + BBMR + BBRR;
    dst_signed.convertTo(dst, CV_8UC3);

  } else {
    cout << "Sorry, not today!" << endl;
  }

  return true;
}
}  // namespace ipcv
