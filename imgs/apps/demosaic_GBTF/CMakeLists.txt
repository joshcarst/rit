rit_add_executable(demosaic_GBTF 
  SOURCES
    demosaic_GBTF.cpp
)

target_link_libraries(demosaic_GBTF 
  Boost::filesystem 
  Boost::program_options 
  rit::ipcv_demosaicing
  opencv_core
  opencv_highgui
  opencv_imgcodecs
)
