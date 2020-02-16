rit_add_executable(histogram_enhance
  SOURCES
    histogram_enhance.cpp
)

target_link_libraries(histogram_enhance
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_histogram_enhancement 
  rit::ipcv_utils 
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
