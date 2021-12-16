rit_add_executable(seam_carving
  SOURCES
    seam_carving.cpp
)

target_link_libraries(seam_carving
  Boost::filesystem 
  Boost::program_options
  rit::ipcv_seam_carving
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
