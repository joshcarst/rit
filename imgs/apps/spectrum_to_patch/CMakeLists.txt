rit_add_executable(spectrum_to_patch
  SOURCES
    spectrum_to_patch.cpp
)

target_link_libraries(spectrum_to_patch
  opencv_core
  opencv_highgui
  rit::color_spectrum
  rit::numerical_interpolation
  rit::plot
)