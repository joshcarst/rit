rit_add_executable(video_magnification
  SOURCES
    video_magnification.cpp
)

target_link_libraries(video_magnification
  Boost::filesystem 
  Boost::program_options
  opencv_core
  opencv_highgui
  opencv_imgproc
)
