rit_add_executable(plot_histogram 
  SOURCES
    plot_histogram.cpp
)

target_link_libraries(plot_histogram 
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_utils 
  rit::plot 
  opencv_core
  opencv_imgcodecs
)
