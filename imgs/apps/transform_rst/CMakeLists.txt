rit_add_executable(transform_rst
  SOURCES
    transform_rst.cpp
)

target_link_libraries(transform_rst
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_geometric_transformation 
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
