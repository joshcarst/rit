rit_add_executable(demosaic_bilinear 
  SOURCES
    demosaic_bilinear.cpp
)

target_link_libraries(demosaic_bilinear 
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_demosaicing
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
