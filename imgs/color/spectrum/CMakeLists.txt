rit_add_library(color_spectrum
  SOURCES
    Spectrum.cpp
  HEADERS
    Spectrum.h
)

target_link_libraries(color_spectrum
  PUBLIC
    opencv_core
    rit::utils_file_csvfile
    rit::numerical_interpolation
)