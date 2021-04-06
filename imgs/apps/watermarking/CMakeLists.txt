rit_add_executable(watermarking
  SOURCES
    watermarking.cpp
)

target_link_libraries(watermarking
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_watermarking
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
