rit_add_executable(bilateral_filter 
  SOURCES
    bilateral_filter.cpp
)

target_link_libraries(bilateral_filter 
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_bilateral_filtering
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
