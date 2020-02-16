rit_add_executable(threshold 
  SOURCES
    threshold.cpp
)

target_link_libraries(threshold 
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_otsus_threshold 
  rit::ipcv_utils 
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
