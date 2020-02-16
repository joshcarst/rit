rit_add_executable(transform_q2q
  SOURCES
    transform_q2q.cpp
)

target_link_libraries(transform_q2q
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_geometric_transformation 
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
